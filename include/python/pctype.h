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
#include "python/pythonutil.h"
#include "python/pstring.h"
#include "python/pdict.h"
#include "python/ptuple.h"

template<class Outer, class Inner>
class PCType : public PyObject
{
protected:
  Inner inner;
public:
  PCType()
  {
    PyObject_Init(this, &type_object());
  }

  PCType(PTuple args, PDict kwargs)
  {
  }

  virtual ~PCType() = 0 { }

  operator PyObject * ()
  {
    return this;
  }

  virtual PString str()
  {
    stringstream ss;

    ss << "(" << type_object().tp_name << " object at " << std::hex << this << ")";
    return PString(ss.str());
  }

  static PyObject *create(PyTypeObject *type, PyObject *args, PyObject *kwds)
  {
    PyObject *ptr = type->tp_alloc(type, 0);
    Outer *obj = new(ptr) Outer(PTuple(args), PDict(kwds));

    return reinterpret_cast<PyObject*>(obj);
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

  static void init()
  {
    PyTypeObject& type = type_object();

    type.ob_refcnt = 1;
    type.tp_alloc = alloc;
    type.tp_free = free;
    type.tp_new = create;
    type.tp_dealloc = dealloc;
    type.tp_flags |= Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
    if(type.tp_name == NULL)
      type.tp_name = (string(typeid(Outer).name()) + string("Wrap")).c_str();

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
};

#endif
