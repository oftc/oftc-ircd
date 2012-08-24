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

#ifndef PYTHONWRAP_H_INC
#define PYTHONWRAP_H_INC
 /*
#include "Python.h"
#include "python/pythonbase.h"
#include "python/pythonutil.h"

#define PY_METHOD(name, func, flags, doc) { name, reinterpret_cast<PyCFunction>(func), flags, doc }
#define PY_METHOD_END { NULL, NULL, 0, NULL }

#define PY_GETSET(name, type, doc, flags) { const_cast<char *>(name), reinterpret_cast<getter>(type::get_wrap), \
  reinterpret_cast<setter>(type::set_wrap), const_cast<char *>(doc), reinterpret_cast<void *>(flags) }
#define PY_GETSET_END { NULL, NULL, NULL, NULL, NULL }

template <class Outer, class Inner> class PythonWrap : public PythonBase
{
protected:
  Inner wrapped;
public:
  PythonWrap(PyObject *args, PyObject *kwds)
  {
    PyObject *obj;
    Inner ptr;

    if(PyTuple_Size(args) == 0)
      return;

    if(!PyArg_ParseTuple(args, "O", &obj))
    {
      PythonUtil::log_error();
      return;
    }

    ptr = *(reinterpret_cast<Inner *>(PyCObject_AsVoidPtr(obj)));
    wrapped = ptr;

    Logging::trace << "Created PythonWrap: " << this << Logging::endl;
  }

  const Inner& get_wrapped() const
  {
    return wrapped;
  }

  static PyTypeObject& type_object()
  {
    type_object.ob_refcnt = 1;
    static PyTypeObject type;

    return type;
  }

  static void init(PyObject *module)
  {
    PyTypeObject& type = type_object();

    type.ob_refcnt = 1;
    type.tp_alloc = alloc;
    if(type.tp_free == NULL)
      type.tp_free = free;
    type.tp_new = create;
    type.tp_dealloc = dealloc;
    type.tp_basicsize = sizeof(Outer);
    if(type.tp_name == NULL)
      type.tp_name = typeid(Inner).name();
    type.tp_flags |= Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;

    if(PyType_Ready(&type) < 0)
    {
      PythonUtil::log_error();
      throw runtime_error("Unable to create type");
    }

    Py_INCREF(&type);
    PyModule_AddObject(module, type.tp_name, reinterpret_cast<PyObject *>(&type));
  }

  static PyObject *alloc(PyTypeObject *type, Py_ssize_t items)
  {
    char *ret;
    PyObject *obj;

    ret = new char[_PyObject_VAR_SIZE(type, items)]();

    obj = reinterpret_cast<PyObject *>(ret);
    static_cast<void>(PyObject_INIT(obj, type));

    Logging::trace << "Created Python object: " << type->tp_name << " at: " << static_cast<void*>(obj) << Logging::endl;

    return obj;
  }

  static Outer *call(PyObject *args)
  {
    return reinterpret_cast<Outer *>(PyObject_CallObject(reinterpret_cast<PyObject *>(&type_object()), args));
  }

  static bool check(PyObject *arg)
  {
    return Py_TYPE(arg) == &type_object();  
  }

  static PyObject *create(PyTypeObject *type, PyObject *args, PyObject *kwds)
  {
    PyObject *ptr = type->tp_alloc(type, 0);
    Outer *obj = new(ptr) Outer(args, kwds);

    return reinterpret_cast<PyObject *>(obj);
  }

  static void dealloc(PyObject *obj)
  {
    Outer *ptr = static_cast<Outer *>(obj);
    ptr->~Outer();
    obj->ob_type->tp_free(obj);
  }

  static void init(const char *name)
  {
    PyTypeObject& type = type_object();

    type.tp_name = name;
    type.tp_new = create;
    type.tp_dealloc = dealloc;

    PythonBase::init();
  }

  static Outer *wrap(const Inner *arg)
  {
    PyObject *obj, *args;
    Outer *wrapped;

    obj = PyCObject_FromVoidPtr(const_cast<Inner *>(arg), NULL);

    args = Py_BuildValue("(O)", obj);

    wrapped = call(args);
    Py_DECREF(obj);

    return wrapped;
  }
};*/

#endif
