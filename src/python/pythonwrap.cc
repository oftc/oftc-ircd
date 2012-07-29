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
#include "python/eventwrap.h"

// Static initialisers
template<class T> PyTypeObject PythonWrap<T>::type_object;

template void PythonWrap<ParserWrap>::init(const char *name);
template void PythonWrap<EventWrap>::init(const char *name);
template void PythonWrap<ClientWrap>::init(const char *name);

template ParserWrap *PythonWrap<ParserWrap>::wrap(void *);
template EventWrap *PythonWrap<EventWrap>::wrap(void *);
template ClientWrap *PythonWrap<ClientWrap>::wrap(void *);

template PyTypeObject *PythonWrap<ParserWrap>::get_type_object();
template PyTypeObject *PythonWrap<EventWrap>::get_type_object();
template PyTypeObject *PythonWrap<ClientWrap>::get_type_object();

template bool PythonWrap<ParserWrap>::handle_event(PyObject *event, PyObject *args);
template bool PythonWrap<EventWrap>::handle_event(PyObject *event, PyObject *args);
template bool PythonWrap<ClientWrap>::handle_event(PyObject *event, PyObject *args);

template<class T>
void PythonWrap<T>::init(const char *name)
{
  type_object.ob_refcnt = 1;
  type_object.tp_alloc = alloc;
  type_object.tp_free = free;
  type_object.tp_new = create;
  type_object.tp_dealloc = dealloc;
  type_object.tp_basicsize = sizeof(T);
  type_object.tp_name = name;
  type_object.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;

  if(PyType_Ready(&type_object) < 0)
  {
    PyErr_Print();
    throw runtime_error("Unable to create type");
  }
}
 
template<class T>
PyObject *PythonWrap<T>::alloc(PyTypeObject *type, Py_ssize_t items)
{
  char *ret;
  PyObject *obj;

  ret = new char[_PyObject_VAR_SIZE(type, items)]();

  obj = reinterpret_cast<PyObject *>(ret);
  static_cast<void>(PyObject_INIT(obj, type));

  Logging::debug << "Created Python object: " << type->tp_name << " at: " << static_cast<void*>(obj) << Logging::endl;

  return obj;
}

template<class T>
void PythonWrap<T>::free(void *ptr)
{
  char *obj = static_cast<char *>(ptr);

  Logging::debug << "Destroyed python object: " << ptr << Logging::endl;

  delete[] obj;
}

template<class T>
PyObject *PythonWrap<T>::create(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  PyObject *ptr = type->tp_alloc(type, 0);
  T *obj = new(ptr) T(args, kwds);

  return reinterpret_cast<PyObject*>(obj);
}

template<class T>
void PythonWrap<T>::dealloc(PyObject *obj)
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
  Py_DECREF(obj);
  if(wrapped == NULL)
    return NULL;

  return wrapped;
}

template<class T>
bool PythonWrap<T>::handle_event(PyObject *event, PyObject *args)
{
  PyObject *handler, *ret;
  
  handler = PyObject_GetAttrString(event, "handler");

  if(handler == NULL || !PyCallable_Check(handler))
  {
    // No handler, return true to say that everything is ok, we just dont have
    // anything attached
    return true;
  }

  ret = PyObject_CallObject(handler, args);
  if(ret == NULL)
  {
    PyErr_Print();
    return false;
  }
  else if(ret == Py_True)
  {
    Py_DECREF(Py_True);
    return true;
  }
  else
  {
    Py_DECREF(Py_False);
    return false;
  }
}

template<class T>
PyTypeObject *PythonWrap<T>::get_type_object() 
{ 
  return &type_object; 
}

template<class T>
PyObject *PythonWrap<T>::get_type() 
{ 
  return reinterpret_cast<PyObject *>(&type_object); 
}
