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

#ifndef PSEQUENCE_H_INC
#define PSEQUENCE_H_INC

#include "Python.h"
#include "stdinc.h"
#include "python/pobject.h"
#include "python/pythonutil.h"

template<class T>
class PSequence : public PObject
{
public:
  PSequence() { }

  PSequence(PyObject *ptr) : PObject(ptr)
  {
  }

  PSequence(int) : PObject(0)
  {
  }

  PSequence(PSequence<PObject>& copy) : PObject(copy)
  {
  }

  using PObject::operator=;

  const T operator[] (int index) const
  {
    return PySequence_GetItem(object, index);
  }

  virtual void set_item(int index, const T& item)
  {
    if(PySequence_SetItem(object, index, item) == -1)
    {
      PythonUtil::log_error();
      throw runtime_error("Error setting sequence item");
    }
  }
};

#endif
