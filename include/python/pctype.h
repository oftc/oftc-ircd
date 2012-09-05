/*
  Copyright (c) 2012 Stuart Walsh

  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation
  files (the "Software"), to deal in the Software without
  restriction, including without limitation the rights to use,
  copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following
  conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
  OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef PCTYPE_H_INC
#define PCTYPE_H_INC

#include "Python.h"
#include <map>
#include <string>
#include "python/pythonutil.h"
#include "python/pstring.h"
#include "python/pdict.h"
#include "python/ptuple.h"
#include "python/pmethod.h"
#include "python/pint.h"
#include "event.h"

using std::map;
using std::string;

enum PropertyFlag
{
  NoFlags = 0x0,
  BoolArg = 0x1,
  IntArg = 0x2,
  StringArg = 0x4,
  ClientArg = 0x8,
  ChannelArg = 0x10,
  ServerArg = 0x20
};

struct Property : public PyGetSetDef 
{
  int number;
  PropertyFlag flags;
};

class PEvent;

template<class Outer, class Inner>
class PCType : public PyObject
{
private:
protected:
  Inner inner;
public:
  typedef function<PObject(Outer *)> NoArgsMethod;
  typedef function<PObject(Outer *, const PTuple&)> VarArgsMethod;
  typedef function<PObject(Outer *, const PTuple&, const PDict&)> KeywordArgsMethod;
  typedef map<string, PMethod<Outer> > MethodMap;
  typedef map<string, Property> PropertyMap;

  PCType()
  {
    PyObject_Init(this, &type_object());
  }

  PCType(const Inner ptr) : inner(ptr)
  {
    PyObject_Init(this, &type_object());
  }

  PCType(const PTuple& args, const PDict& kwargs)
  {
  }

  virtual ~PCType() {}

  operator PyObject *()
  {
    return this;
  }

  operator Inner()
  {
    return inner;
  }

  operator PObject()
  {
    return PObject(this);
  }

  virtual PString str() const
  {
    stringstream ss;

    ss << "(" << type_object().tp_name << " object at " << std::hex << std::showbase << this << ")";
    return PString(ss.str());
  }

  virtual PCType<Outer, Inner>* incref()
  {
    Py_INCREF(this);
    return this;
  }
  
  virtual PObject get(const Property prop)
  {
    return PObject::None();
  }

  virtual int set(const Property prop, const PObject& value)
  {
    if(prop.flags & BoolArg && !PBool::check(value))
    {
      PyErr_SetString(PyExc_TypeError, "Value must be a bool");
      return -1;
    }

    if(prop.flags & IntArg && !PInt::check(value))
    {
      PyErr_SetString(PyExc_TypeError, "Value must be an integer");
      return -1;
    }

    if(prop.flags & StringArg && !PString::check(value))
    {
      PyErr_SetString(PyExc_TypeError, "Value must be a string");
      return -1;
    }

    if((prop.flags & ClientArg || prop.flags & ServerArg) && !PClient::check(value))
    {
      PyErr_SetString(PyExc_TypeError, "Value must be a client");
      return -1;
    }

    if(prop.flags & ChannelArg && !PChannel::check(value))
    {
      PyErr_SetString(PyExc_TypeError, "Value must be a channel");
      return -1;
    }

    return 0;
  }

  virtual PObject getattro(const PString& name)
  {
    auto it = methods().find(name);

    if(it == methods().end())
    {
      auto pit = properties().find(name);

      if(pit == properties().end())
        return PyObject_GenericGetAttr(this, name);

      return get(pit->second).incref();
    }

    PTuple args(2);
    PObject ret;

    args.set_item(0, this->incref());
    args.set_item(1, PyCObject_FromVoidPtr(it->second, NULL));

    ret = PyCFunction_NewEx(it->second, args, NULL);

    return ret;
  }

  virtual int setattro(const PString& name, const PObject& value)
  {
    auto it = properties().find(name);

    if(it == properties().end())
      return PyObject_GenericSetAttr(this, name, value);

    return set(it->second, value);
  }

  virtual int compare(const PObject& right)
  {
    return PyObject_Compare(this, right);
  }

  inline Inner get_inner() const { return inner; }

  // Statics

  static bool check(PyObject *obj)
  {
    return Py_TYPE(obj) == &type_object();
  }

  static PyObject *create(PyTypeObject *type, PyObject *args, PyObject *kwds)
  {
    Outer *obj = new Outer(args, kwds);
    PyObject_Init(obj, type);

    Logging::trace << "Created Python object: " << type->tp_name << " at: " << static_cast<void *>(obj) << Logging::endl;

    return static_cast<PyObject *>(obj);
  }

  static PyObject *alloc(PyTypeObject *type, Py_ssize_t items)
  {
    return NULL;
  }

  static void free(void *ptr)
  {
    PyObject *tmp = static_cast<PyObject *>(ptr);
    Outer *pobj = static_cast<Outer *>(tmp);

    Logging::trace << "Destroyed python object: " << ptr << Logging::endl;

    delete pobj;
  }

  static void dealloc(PyObject *obj)
  {
    obj->ob_type->tp_free(obj);
  }

  static void add_method(const char *name, const char *doc, NoArgsMethod method)
  {
    methods()[name] = PMethod<Outer>(name, reinterpret_cast<PyCFunction>(noargs_callback), doc, method);
  }

  static void add_method(const char *name, const char *doc, VarArgsMethod method)
  {
    methods()[name] = PMethod<Outer>(name, varargs_callback, doc, method);
  }

  static void add_method(const char *name, const char *doc, KeywordArgsMethod method)
  {
    methods()[name] = PMethod<Outer>(name, reinterpret_cast<PyCFunction>(kwargs_callback), doc, method);
  }

  static void add_method(const char *name, const char *doc, PyCFunction method)
  {
    methods()[name] = PMethod<Outer>(name, METH_VARARGS | METH_STATIC, method, doc);
  }

  static void add_method(const char *name, const char *doc, PyCFunctionWithKeywords method)
  {
    methods()[name] = PMethod<Outer>(name, METH_VARARGS | METH_STATIC | METH_KEYWORDS, reinterpret_cast<PyCFunction>(method), doc);
  }

  static void add_property(const char *name, const char *doc, int number, PropertyFlag flags)
  {
    Property prop;

    prop.name = const_cast<char *>(name);
    prop.doc = const_cast<char *>(doc);
    prop.flags = flags;
    prop.number = number;
    prop.set = NULL;
    prop.closure = NULL;
    prop.get = NULL;

    properties()[name] = prop;
  }
  
  static PyObject *getattro_callback(PyObject *self, PyObject *name)
  {
    PCType<Outer, Inner> *base = static_cast<PCType<Outer, Inner> *>(self);

    PyObject *tmp = base->getattro(name);

    return tmp;
  }

  static int setattro_callback(PyObject *self, PyObject *name, PyObject *value)
  {
    PCType<Outer, Inner> *base = static_cast<PCType<Outer, Inner> *>(self);

    return base->setattro(name, value);
  }

  static PyObject *noargs_callback(PyObject *self)
  {
    PTuple data(self);
    PyObject *ret;

    PMethod<Outer> method = *static_cast<PMethod<Outer>*>(PyCObject_AsVoidPtr(data[1]));
    PyObject *tmp = data[0];
    Outer *ptr = static_cast<Outer *>(tmp);

    ret = method(ptr);

    return ret;
  }

  static PyObject *varargs_callback(PyObject *self, PyObject *args)
  {
    PTuple data(self);
    PyObject *ret;

    PMethod<Outer> method = *static_cast<PMethod<Outer>*>(PyCObject_AsVoidPtr(data[1]));
    PyObject *tmp = data[0];
    Outer *ptr = static_cast<Outer *>(tmp);

    ret = method(ptr, args);

    return ret;
  }

  static PyObject *kwargs_callback(PyObject *self, PyObject *args, PyObject *kwargs)
  {
    PTuple data(self);
    PyObject *ret;


    PMethod<Outer> method = *static_cast<PMethod<Outer>*>(PyCObject_AsVoidPtr(data[1]));
    PyObject *tmp = data[0];
    Outer *ptr = static_cast<Outer *>(tmp);

    ret = method(ptr, args, kwargs);

    return ret;
  }

  static PyObject *str_callback(PyObject *self)
  {
    PCType<Outer, Inner> *base = static_cast<PCType<Outer, Inner> *>(self);

    return base->str();
  }

  static int compare_callback(PyObject *left, PyObject *right)
  {
    PCType<Outer, Inner> *base = static_cast<PCType<Outer, Inner> *>(left);

    return base->compare(right);
  }
  
  static void init(const PObject& module)
  {
    PyTypeObject& type = type_object();

    type.ob_refcnt = 1;
    type.tp_alloc = alloc;
    type.tp_free = free;
    type.tp_new = create;
    type.tp_dealloc = dealloc;
    type.tp_getattro = getattro_callback;
    type.tp_setattro = setattro_callback;
    type.tp_str = str_callback;
    type.tp_flags |= Py_TPFLAGS_DEFAULT;
    type.tp_compare = compare_callback;
    if(type.tp_name == NULL)
      type.tp_name = (string(typeid(Outer).name()) + string("Wrap")).c_str();

    int methodlen = methods().size() + 1;
    PyMethodDef *pymeth = new PyMethodDef[methodlen];

    int i = 0;

    for(auto it = methods().cbegin(); it != methods().cend(); it++, i++)
    {
      PyMethodDef *def = &pymeth[i];
      
      *def = it->second;
    }

    pymeth[i].ml_doc = NULL;
    pymeth[i].ml_flags = 0;
    pymeth[i].ml_meth = NULL;
    pymeth[i].ml_name = NULL;

    type.tp_methods = pymeth;

    i = 0;
    int propertylen = properties().size() + 1;
    PyGetSetDef *pyprop = new PyGetSetDef[propertylen];

    for(auto it = properties().cbegin(); it != properties().cend(); it++, i++)
    {
      PyGetSetDef *def = &pyprop[i];

      *def = it->second;
    }

    pyprop[i].doc = NULL;
    pyprop[i].closure = NULL;
    pyprop[i].get = NULL;
    pyprop[i].set = NULL;
    pyprop[i].name = NULL;

    type.tp_getset = pyprop;

    if(PyType_Ready(&type) != 0)
    {
      PythonUtil::log_error();
      return;
    }

    PyModule_AddObject(module, type.tp_name, reinterpret_cast<PyObject *>(&type));
  }

  static PyTypeObject& type_object()
  {
    static PyTypeObject type;
  
    return type;
  }

  static MethodMap& methods()
  {
    static MethodMap method_map;

    return method_map;
  }

  static PropertyMap& properties()
  {
    static PropertyMap property_map;
    
    return property_map;
  }
};

#endif
