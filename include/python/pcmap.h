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

#ifndef PCMAP_H_INC
#define PCMAP_H_INC

#include "Python.h"
#include "python/pctype.h"
#include "python/pcollection.h"

template<class Map, class WrappedKey, class WrappedValue>
class PCMap : public PCollection
{
private:
  typedef PCMap<Map, WrappedKey, WrappedValue> PCMapType;
  Map inner_map;
  typename Map::const_iterator current;
public:
  PCMap()
  {
  }

  PCMap(Map ptr) : PCollection(PTuple(), PDict()), inner_map(ptr)
  {
  }

  PCMap(PTuple args, PDict kwargs) : PCollection(args, kwargs)
  {
  }

  ~PCMap()
  {
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

  Py_ssize_t get_length()
  {
    return inner_map.size();
  }

  PObject iter()
  {
    current = inner_map.begin();
    Py_INCREF(this);
    return this;
  }

  PObject next()
  {
    if(current == inner_map.end())
      return NULL;

    WrappedValue *value = new WrappedValue(current->second);
    current++;

    return value;
  }
};

#endif
