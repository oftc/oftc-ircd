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

#include "stdinc.h"
#include "python/pobject.h"
#include "python/pstring.h"

PObject::PObject()
{
  PyObject *obj = PyObject_New(PyObject, &PyBaseObject_Type);

  *this = obj;
}

PObject& PObject::operator=(const PyObject * const right)
{
  this->ob_refcnt = right->ob_refcnt;
  this->ob_type = right->ob_type;
  this->_ob_next = right->_ob_next;
  this->_ob_prev = right->_ob_prev;

  this->_ob_next->_ob_prev = this;
  this->_ob_prev->_ob_next = this;

  PyObject_Free(const_cast<PyObject *>(right));

  return *this;
}

PString PObject::str()
{
  return PString();
}
