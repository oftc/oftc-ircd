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
#include "python/channelwrap.h"
#include "python/connectionwrap.h"
#include "python/collectionwrap.h"
#include "python/pythonutil.h"

// Static initialisers
template<class T> PyTypeObject PythonWrap<T>::type_object;

template void PythonWrap<ParserWrap>::init(PyObject *, const char *);
template void PythonWrap<EventWrap>::init(PyObject *, const char *);
template void PythonWrap<ClientWrap>::init(PyObject *, const char *);
template void PythonWrap<ChannelWrap>::init(PyObject *, const char *);
template void PythonWrap<ConnectionWrap>::init(PyObject *, const char *);
template void PythonWrap<CollectionWrap<list<ChannelPtr>, ChannelWrap> >::init(PyObject *, const char *);

template ParserWrap *PythonWrap<ParserWrap>::wrap(void *);
template EventWrap *PythonWrap<EventWrap>::wrap(void *);
template ClientWrap *PythonWrap<ClientWrap>::wrap(void *);
template ChannelWrap *PythonWrap<ChannelWrap>::wrap(void *);
template ConnectionWrap *PythonWrap<ConnectionWrap>::wrap(void *);

template bool PythonWrap<ParserWrap>::handle_event(PyObject *, PyObject *);
template bool PythonWrap<EventWrap>::handle_event(PyObject *, PyObject *);
template bool PythonWrap<ClientWrap>::handle_event(PyObject *, PyObject *);
template bool PythonWrap<ChannelWrap>::handle_event(PyObject *, PyObject *);
template bool PythonWrap<ConnectionWrap>::handle_event(PyObject *, PyObject *);
//template bool PythonWrap<CollectionWrap>::handle_event(PyObject *, PyObject *);

template bool PythonWrap<ParserWrap>::check(PyObject *);
template bool PythonWrap<EventWrap>::check(PyObject *);
template bool PythonWrap<ClientWrap>::check(PyObject *);
template bool PythonWrap<ChannelWrap>::check(PyObject *);
template bool PythonWrap<ConnectionWrap>::check(PyObject *);
//template bool PythonWrap<CollectionWrap>::check(PyObject *);

template<class T>
void PythonWrap<T>::init(PyObject *module, const char *name)
{
  type_object.ob_refcnt = 1;
  type_object.tp_alloc = alloc;
  type_object.tp_free = free;
  type_object.tp_new = create;
  type_object.tp_dealloc = dealloc;
  type_object.tp_basicsize = sizeof(T);
  type_object.tp_name = name;
  type_object.tp_flags |= Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;

  if(PyType_Ready(&type_object) < 0)
  {
    PythonUtil::log_error();
    throw runtime_error("Unable to create type");
  }

  Py_INCREF(&type_object);
  PyModule_AddObject(module, type_object.tp_name, reinterpret_cast<PyObject *>(&type_object));
}
 
template<class T>
PyObject *PythonWrap<T>::alloc(PyTypeObject *type, Py_ssize_t items)
{
  char *ret;
  PyObject *obj;

  ret = new char[_PyObject_VAR_SIZE(type, items)]();

  obj = reinterpret_cast<PyObject *>(ret);
  static_cast<void>(PyObject_INIT(obj, type));

  Logging::trace << "Created Python object: " << type->tp_name << " at: " << static_cast<void*>(obj) << Logging::endl;

  return obj;
}

template<class T>
void PythonWrap<T>::free(void *ptr)
{
  char *obj = static_cast<char *>(ptr);

  Logging::trace << "Destroyed python object: " << ptr << Logging::endl;

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

  wrapped = PythonWrap<T>::call(args);
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
    PythonUtil::log_error();
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
bool PythonWrap<T>::check(PyObject *arg)
{
  return Py_TYPE(arg) == &type_object;   
}

template<class T>
T *PythonWrap<T>::call(PyObject *args)
{
  return reinterpret_cast<T*>(PyObject_CallObject(reinterpret_cast<PyObject *>(&type_object), args));
}
