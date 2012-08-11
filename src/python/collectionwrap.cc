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
#include <structmember.h>
#include <list>
#include "stdinc.h"
#include "python/collectionwrap.h"
#include "python/channelwrap.h"
#include "channel.h"

using std::list;

static PyMethodDef collection_methods[] =
{
  { NULL, NULL, 0, NULL }
};

static PyMemberDef collection_members[] =
{
  { NULL, 0, 0, 0, NULL }
};

static PyGetSetDef collection_getsetters[] = 
{
  { NULL, NULL, NULL, NULL, NULL }
};

template class CollectionWrap<list<ChannelPtr>, ChannelWrap>;
template CollectionWrap<list<ChannelPtr>, ChannelWrap> *CollectionWrap<list<ChannelPtr>, ChannelWrap>::wrap(void *);

template<class T, class W>
CollectionWrap<T, W>::CollectionWrap(PyObject *args, PyObject *kwds)
{
  PyObject *list_ptr;

  if(args != NULL)
  {
    PyArg_ParseTuple(args, "O", &list_ptr);

    iterable = *reinterpret_cast<T *>(PyCObject_AsVoidPtr(list_ptr));

    curr = iterable.begin();
  }

  Logging::trace << "Created CollectionWrap: " << this << Logging::endl;
}

template<class T, class W>
CollectionWrap<T, W>::~CollectionWrap()
{
  Logging::trace << "Destroyed CollectionWrap: " << this << Logging::endl;
}

template<class T, class W>
void CollectionWrap<T, W>::reset()
{
  curr = iterable.begin();
}

template<class T, class W>
W *CollectionWrap<T, W>::next()
{
  typename T::value_type item;
  if(curr == iterable.end())
    return NULL;

  item = *curr;
  curr++;

  PyObject *obj, *args;
  W *wrapped;

  obj = PyCObject_FromVoidPtr(&item, NULL);

  args = Py_BuildValue("(O)", obj);

  wrapped = PythonWrap<W>::call(args);
  Py_DECREF(obj);
  if(wrapped == NULL)
    return NULL;

  return wrapped;
}

// Statics

template<class T, class W>
void CollectionWrap<T, W>::init(PyObject *module)
{
  PythonWrap<CollectionWrap>::type_object.tp_methods = collection_methods;
  PythonWrap<CollectionWrap>::type_object.tp_members = collection_members;
  PythonWrap<CollectionWrap>::type_object.tp_getset = collection_getsetters;
  PythonWrap<CollectionWrap>::type_object.tp_iter = reinterpret_cast<getiterfunc>(iter);
  PythonWrap<CollectionWrap>::type_object.tp_iternext = reinterpret_cast<iternextfunc>(iter_next);
  PythonWrap<CollectionWrap>::type_object.tp_flags |= Py_TPFLAGS_HAVE_ITER;
  PythonWrap<CollectionWrap>::init(module, "Collection");
}

template<class T, class W>
CollectionWrap<T, W> *CollectionWrap<T, W>::wrap(void *arg)
{
  PyObject *obj, *args;
  CollectionWrap<T, W> *wrapped;

  obj = PyCObject_FromVoidPtr(arg, NULL);

  args = Py_BuildValue("(O)", obj);

  wrapped = reinterpret_cast<CollectionWrap<T, W> *>(PyObject_CallObject(reinterpret_cast<PyObject *>(&type_object), args));
  Py_DECREF(obj);
  if(wrapped == NULL)
    return NULL;

  return wrapped;
}

template<class T, class W>
CollectionWrap<T, W> *CollectionWrap<T, W>::iter(CollectionWrap<T, W> *self)
{
  self->reset();
  Py_INCREF(self);
  return self;
}

template<class T, class W>
W *CollectionWrap<T, W>::iter_next(CollectionWrap<T, W> *iterator)
{
  return iterator->next();
}
