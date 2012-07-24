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

#include <Python.h>
#include "stdinc.h"
#include "python/pythonwrap.h"
#include "python/parserwrap.h"
#include "python/clientwrap.h"

template<class T> PyTypeObject PythonWrap<T>::type_object;
template<class T> PyMethodDef *PythonWrap<T>::methods;
template<class T> PyMemberDef *PythonWrap<T>::members;
template<class T> PyGetSetDef *PythonWrap<T>::getsetters;
template<class T> reprfunc PythonWrap<T>::str;

template void PythonWrap<ParserWrap>::init(const char *);
template void PythonWrap<ClientWrap>::init(const char *);

template PythonWrap<typename ClientWrap>;
template PythonWrap<typename ParserWrap>;

template<class T>
void
PythonWrap<T>::init(const char *name)
{
  type_object.ob_refcnt = 1;
  type_object.tp_alloc = alloc;
  type_object.tp_free = free;
  type_object.tp_new = create;
  type_object.tp_dealloc = dealloc;
  type_object.tp_basicsize = sizeof(T);
  type_object.tp_name = name;
  type_object.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
  type_object.tp_methods = PythonWrap<T>::methods;
  type_object.tp_members = PythonWrap<T>::members;
  type_object.tp_getset = PythonWrap<T>::getsetters;
  type_object.tp_str = PythonWrap<T>::str;

  if(PyType_Ready(&type_object) < 0)
  {
    PyErr_Print();
    throw runtime_error("Unable to create type");
  }
}
 
template<class T>
PyObject *
PythonWrap<T>::alloc(PyTypeObject *type, Py_ssize_t items)
{
  char *ret;
  PyObject *obj;

  ret = new char[_PyObject_VAR_SIZE(type, items)]();

  obj = reinterpret_cast<PyObject *>(ret);
  static_cast<void>(PyObject_INIT(obj, type));

  return obj;
}

template<class T>
void
PythonWrap<T>::free(void *ptr)
{
  char *obj = static_cast<char *>(ptr);

  delete[] obj;
}

template<class T>
PyObject *
PythonWrap<T>::create(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  PyObject *ptr = type->tp_alloc(type, 0);
  T *obj = new(ptr) T(args, kwds);

  return reinterpret_cast<PyObject*>(obj);
}

template<class T>
void
PythonWrap<T>::dealloc(PyObject *obj)
{
  T *ptr = reinterpret_cast<T *>(obj);
  ptr->~T();
  obj->ob_type->tp_free(obj);
}

template<class T>
T *PythonWrap<T>::wrap(void *arg)
{
  PyObject *obj, *args;
  T *wrapped;

  obj = PyCObject_FromVoidPtr(arg, NULL);

  args = Py_BuildValue("(O)", obj);

  wrapped = reinterpret_cast<T *>(PyObject_CallObject(T::get_type(), args));
  if(wrapped == NULL)
    return NULL;

  return wrapped;
}
