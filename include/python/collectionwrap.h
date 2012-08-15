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

    CollectionWrap<T, W>::type_object.tp_iter = reinterpret_cast<getiterfunc>(iter);
    CollectionWrap<T, W>::type_object.tp_iternext = reinterpret_cast<iternextfunc>(iter_next);
    CollectionWrap<T, W>::type_object.tp_as_sequence = &seq_methods;
    CollectionWrap<T, W>::type_object.tp_flags |= Py_TPFLAGS_HAVE_ITER;
    PythonWrap<CollectionWrap<T, W>, T>::init(module, (string("Collection ") + typeid(T).name()).c_str());
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
    return NULL;
  }

  // Ctor/dtors
  CollectionWrap(PyObject *args, PyObject *kwargs) 
  {
  }

  // methods
  Py_ssize_t length() const
  {
    return this->wrapped.size();
  }

  void reset()
  {
    curr = this->wrapped.begin();
  }

  W *next()
  {
    typename T::mapped_type item;

    if(curr == this->wrapped.end())
      return NULL;

    item = curr->second;
    curr++;

    W *wrapped;

    wrapped = W::wrap(&item);

    if(wrapped == NULL)
      return NULL;

    return wrapped;
  }
};

#endif
