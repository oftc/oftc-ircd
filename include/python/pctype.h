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

using std::map;
using std::string;

template<class Outer, class Inner>
class PCType : public PyObject
{
protected:
  Inner inner;
public:
  typedef function<PObject(Outer)> NoArgsMethod;
  typedef function<PObject(Outer, const PTuple)> VarArgsMethod;
  typedef function<PObject(Outer, const PTuple, const PDict)> KeywordArgsMethod;
  typedef map<string, PMethod<Outer> > MethodMap;

  PCType()
  {
    PyObject_Init(this, &type_object());
  }

  PCType(PTuple args, PDict kwargs)
  {
  }

  virtual ~PCType() {}

  operator PyObject *()
  {
    return this;
  }

  virtual PString str()
  {
    stringstream ss;

    ss << "(" << type_object().tp_name << " object at " << std::hex << std::showbase << this << ")";
    return PString(ss.str());
  }

  static PyObject *create(PyTypeObject *type, PyObject *args, PyObject *kwds)
  {
    PyObject *ptr = type->tp_alloc(type, 0);
    Outer *obj = new(ptr) Outer(PTuple(args), PDict(kwds));

    return static_cast<PyObject *>(obj);
  }

  static PyObject *alloc(PyTypeObject *type, Py_ssize_t items)
  {
    char *ret;
    PyObject *obj;

    ret = new char[_PyObject_VAR_SIZE(type, items)]();

    obj = reinterpret_cast<PyObject *>(ret);
    static_cast<void>(PyObject_INIT(obj, type));

    Logging::trace << "Created Python object: " << type->tp_name << " at: " << static_cast<void *>(obj) << Logging::endl;

    return obj;
  }

  static void free(void *ptr)
  {
    char *obj = static_cast<char *>(ptr);

    Logging::trace << "Destroyed python object: " << ptr << Logging::endl;

    delete[] obj;
  }

  static void dealloc(PyObject *obj)
  {
    Outer *ptr = reinterpret_cast<Outer *>(obj);
    ptr->~Outer();
    obj->ob_type->tp_free(obj);
  }

  static void add_method(const char *name, const char *doc, NoArgsMethod method)
  {
    methods()[name] = PMethod<Outer>(name, noargs_callback, doc, method);
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

  static PyObject *getattro_callback(PyObject *self, PyObject *name)
  {
    return PyObject_GenericGetAttr(self, name);
  }

  static PyObject *getattr_callback(PyObject *self, char *name)
  {
    return PyObject_GetAttrString(self, name);
  }

  static PyObject *noargs_callback(PyObject *self)
  {
    Py_RETURN_NONE;
  }

  static PyObject *varargs_callback(PyObject *self, PyObject *args)
  {
    Py_RETURN_NONE;
  }

  static PyObject *kwargs_callback(PyObject *self, PyObject *args, PyObject *kwargs)
  {
    Py_RETURN_NONE;
  }
  
  static void init()
  {
    PyTypeObject& type = type_object();

    type.ob_refcnt = 1;
    type.tp_alloc = alloc;
    type.tp_free = free;
    type.tp_new = create;
    type.tp_dealloc = dealloc;
    type.tp_getattro = getattro_callback;
    type.tp_getattr = getattr_callback;
    type.tp_flags |= Py_TPFLAGS_DEFAULT;
    if(type.tp_name == NULL)
      type.tp_name = (string(typeid(Outer).name()) + string("Wrap")).c_str();

    int methodlen = methods().size() + 1;
    PyMethodDef *pymeth = new PyMethodDef[methodlen];

    int i = 0;

    for(auto it = methods().begin(); it != methods().end(); it++, i++)
    {
      PyMethodDef *def = &pymeth[i];
      
      it->second.fill_pymethod(*def);
    }

    pymeth[i].ml_doc = NULL;
    pymeth[i].ml_flags = 0;
    pymeth[i].ml_meth = NULL;
    pymeth[i].ml_name = NULL;

    type.tp_methods = pymeth;

    if(PyType_Ready(&type) != 0)
    {
      PythonUtil::log_error();
      return;
    }
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
};

#endif
