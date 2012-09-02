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

#ifndef PCLIST_H_INC
#define PCLIST_H_INC

#include "Python.h"
#include "python/pctype.h"
#include "python/pcollection.h"

template<class List, class WrappedValue>
class PCList : public PCollection
{
private:
  typedef PCList<List, WrappedValue> PListType;
  List& inner_list;
  typename List::const_iterator current;
public:
  PCList()
  {
  }

  PCList(List& ptr) : PCollection(PTuple(), PDict()), inner_list(ptr)
  {
  }

  PCList(PTuple args, PDict kwargs)
  {
  }

  ~PCList()
  {
  }

  PObject append(PTuple args)
  {
    PyObject *tmp = args[0];
    WrappedValue *value = static_cast<WrappedValue *>(tmp);

    inner_list.push_back(*value);
    
    return PObject::None();
  }

  PObject remove(PTuple args)
  {
    PyObject *tmp = args[0];
    WrappedValue *value = static_cast<WrappedValue *>(tmp);

    inner_list.remove(*value);

    return PObject::None();
  }

  PObject iter()
  {
    current = inner_list.begin();
    Py_INCREF(this);
    return this;
  }

  PObject next()
  {
    if(current == inner_list.end())
      return NULL;

    WrappedValue *value = new WrappedValue(*current);
    current++;

    return value;
  }
};

#endif
