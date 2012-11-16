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

#include "Python.h"
#include "stdinc.h"
#include "python/pstring.h"

PString::PString() : PObject(0, 0, 0)
{
  object = PyObject_CallObject(reinterpret_cast<PyObject *>(&PyString_Type), NULL);
  Logging::trace << "string: " << object << " [+++] (" << object->ob_refcnt << ")" << Logging::endl;
}

PString::PString(PyObject *str) : PObject(str)
{
  Logging::trace << "string: " << object << " [+++] (" << object->ob_refcnt << ")" << Logging::endl;
}

PString::PString(string str) : PObject(0, 0, 0)
{
  object = PyString_FromString(str.c_str());
  Py_INCREF(object);
  Logging::trace << "string: " << object << " [+++] (" << object->ob_refcnt << ")" << Logging::endl;

}

PString::PString(irc_string str) : PObject(0, 0, 0)
{
  object = PyString_FromString(str.c_str());
  Py_INCREF(object);
  Logging::trace << "string: " << object << " [+++] (" << object->ob_refcnt << ")" << Logging::endl;
}

PString::~PString()
{
  Logging::trace << "string: " << object << " [---] (" << (object->ob_refcnt - 1) << ")" << Logging::endl;
}

const char *PString::c_str() const
{
  if(object == Py_None)
    return "(null)";
  return PyString_AsString(object);
}
