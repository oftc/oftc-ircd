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

#include "python/ptupleitem.h"
#include "python/pythonutil.h"
#include "stdinc.h"

PTupleItem::PTupleItem() : PObject(0, 0, 0)
{
}

PTupleItem::PTupleItem(int i, PyObject *obj) : PObject(0, 0, 0), index(i), tuple(obj) 
{
  object = PyTuple_GetItem(tuple, index);
  Py_XINCREF(object);
  Logging::trace << "tupleitem: " << object << " [+++] (" << object->ob_refcnt << ")" << Logging::endl;
}

PTupleItem::~PTupleItem()
{
  if(object != NULL)
    Logging::trace << "tupleitem: " << object << " [---] (" << object->ob_refcnt - 1 << ")" << Logging::endl;
}

void PTupleItem::operator =(PObject foo)
{
  PyTuple_SetItem(tuple, index, foo);
  Py_XDECREF(object);
  object = PyTuple_GetItem(tuple, index);
  Py_XINCREF(object);
}
