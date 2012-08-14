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

#ifndef COLLECTIONWRAP_H_INC
#define COLLECTIONWRAP_H_INC

#include "Python.h"
#include <iterator>
#include <list>
#include "python/pythonwrap.h"

using std::list;

template <class T, class W> class CollectionWrap : public PythonWrap<CollectionWrap<T, W>, T>
{
private:
  typename T::const_iterator curr;
public:
  // Non Python methods
  static Py_ssize_t get_length(PyObject *self)
  {
    return reinterpret_cast<CollectionWrap<T, W> *>(self)->length();
  }

  static void init(PyObject *module)
  {
    static PySequenceMethods seq_methods = 
    {
      get_length
    };

    static PyMethodDef methods[] =
    {
      PY_METHOD("append", append, METH_OLDARGS, "Add an item to the collection"),
      PY_METHOD_END
    };

    type_object.tp_iter = reinterpret_cast<getiterfunc>(iter);
    type_object.tp_iternext = reinterpret_cast<iternextfunc>(iter_next);
    type_object.tp_as_sequence = &seq_methods;
    type_object.tp_flags |= Py_TPFLAGS_HAVE_ITER;
    PythonWrap::init(module, (string("Collection ") + typeid(T).name()).c_str());
  }

  static CollectionWrap<T, W> *wrap(void *arg)
  {
    PyObject *obj, *args;
    CollectionWrap<T, W> *wrapped;

    obj = PyCObject_FromVoidPtr(arg, NULL);

    args = Py_BuildValue("(O)", obj);

    wrapped = reinterpret_cast<CollectionWrap<T, W> *>(PyObject_CallObject(reinterpret_cast<PyObject *>(&PythonWrap<CollectionWrap, T>::type_object), args));
    Py_DECREF(obj);
    if(wrapped == NULL)
      return NULL;

    return wrapped;
  }

  static CollectionWrap<T, W> *iter(CollectionWrap<T, W> *self)
  {
    self->reset();
    Py_INCREF(self);
    return self;
  }

  static W *iter_next(CollectionWrap<T, W> *iterator)
  {
    return iterator->next();
  }

  static PyObject *append(CollectionWrap<T, W> *self, W *item)
  {
  }

  // Ctor/dtors
  CollectionWrap(PyObject *args, PyObject *kwargs) : PythonWrap(args, kwargs)
  {
  }

  // methods
  Py_ssize_t length() const
  {
    return get_wrapped().size();
  }

  void reset()
  {
    T tmp = get_wrapped();
    curr = tmp.begin();
  }

  W *next()
  {
    typename T::mapped_type item;
    T tmp = get_wrapped();

    if(curr == tmp.end())
      return NULL;

    item = curr->second;
    curr++;

    W *wrapped;

    wrapped = PythonWrap<W, T>::wrap(&item);

    if(wrapped == NULL)
      return NULL;

    return wrapped;
  }
};

#endif
