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

#include "python/pbool.h"
#include "stdinc.h"

PBool::PBool() : PObject(0, 0, 0)
{
  object = PyObject_New(PyObject, &PyBool_Type);
  Logging::trace << "bool: " << object << " [+++] (" << object->ob_refcnt << ")" << Logging::endl;
}

PBool::PBool(bool arg) : PObject(0, 0, 0)
{
  object = PyBool_FromLong(arg);
  Logging::trace << "bool: " << object << " [+++] (" << object->ob_refcnt << ")" << Logging::endl;
}

PBool::~PBool()
{
  Logging::trace << "bool: " << object << " [---] (" << (object->ob_refcnt - 1) << ")" << Logging::endl;
}

const PBool& PBool::incref()
{
  PObject::incref();
  return *this;
}

PBool::operator bool() const
{
  return PyObject_IsTrue(object) != 0;
}

bool PBool::check(const PObject& item)
{
  return PyBool_Check(item) != 0;
}
