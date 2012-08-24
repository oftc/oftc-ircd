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

#ifndef MAPWRAP_H_INC
#define MAPWRAP_H_INC

#include "Python.h"
#include "python/pythonwrap.h"
#include "python/collectionwrap.h"

template<class Map, class WrappedKey, class WrappedValue>
class MapWrap : public CollectionWrap
{
private:
  typedef MapWrap<Map, WrappedKey, WrappedValue> MapWrapType;
  Map inner_map;
  typename Map::const_iterator current;
public:
  MapWrap()
  {
  }

  MapWrap(PyObject *args, PyObject *kwargs)
  {
  }

  ~MapWrap()
  {
  }

  void set_map(const Map& m)
  {
    inner_map = m;
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
    current = inner_map.begin();
    Py_INCREF(this);
    return this;
  }

  PyObject *next()
  {
    if(current == inner_map.end())
      return NULL;

    WrappedValue *value = WrappedValue::wrap(&current->second);
    current++;

    return value;
  }

  static PyObject *wrap(const Map& map)
  {
    MapWrapType *wrapped = new MapWrapType();
    PyObject_INIT(wrapped, &type_object());

    wrapped->set_map(map);

    return wrapped;
  }
};

#endif
