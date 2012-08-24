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

#ifndef LISTWRAP_H_INC
#define LISTWRAP_H_INC

#include "Python.h"
#include "python/pythonwrap.h"
#include "python/collectionwrap.h"

template<class List, class WrappedValue>
class ListWrap : public CollectionWrap
{
private:
  typedef ListWrap<List, WrappedValue> ListWrapType;
  List inner_list;
  typename List::const_iterator current;
public:
  ListWrap()
  {
  }

  ListWrap(PyObject *args, PyObject *kwargs)
  {
  }

  ~ListWrap()
  {
  }

  void set_list(const List& l)
  {
    inner_list = l;
  }

  PyObject *append(PyObject *args)
  {
   /* PyObject *key, *value;
    Wrapped *wrapped;

    PyArg_ParseTuple(args, "OO", &item);

    wrapped = item;
*/
    Py_RETURN_NONE;
  }

  PyObject *iter()
  {
    current = inner_list.begin();
    Py_INCREF(this);
    return this;
  }

  PyObject *next()
  {
    if(current == inner_list.end())
      return NULL;

    WrappedValue *value = WrappedValue::wrap(&(*current));
    current++;

    return value;
  }

  static PyObject *wrap(const List& list)
  {
    ListWrapType *wrapped = new ListWrapType();
    wrapped = PyObject_INIT(wrapped, &type_object());

    wrapped->set_list(list);

    return wrapped;
  }
};

#endif
