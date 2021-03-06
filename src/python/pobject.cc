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

#include "python/pobject.h"
#include "stdinc.h"
#include "python/pstring.h"

PObject::PObject() : object(NULL)
{
  object = PyObject_CallObject(reinterpret_cast<PyObject *>(&PyBaseObject_Type), NULL);
  Logging::trace << "python object new " << object << Logging::endl;
}

PObject::PObject(PyObject *ptr)
{
  object = ptr;
  Py_XINCREF(object);
}

PObject::PObject(const PObject& copy)
{
  object = copy.object;
  Py_XINCREF(object);
  Logging::trace << "python object incref(copyctor) " << object << Logging::endl;
}

PObject::~PObject()
{
  Py_XDECREF(object);
}

PObject& PObject::operator=(const PObject& right)
{
  Logging::trace << "python object defref(assign) " << object << Logging::endl;
  Py_XDECREF(object);
  object = right;
  Py_XINCREF(object);
  Logging::trace << "python object incref(assign) " << object << Logging::endl;
  return *this;
}

PObject& PObject::operator=(const PyObject *right)
{
  Py_XDECREF(object);
  object = const_cast<PyObject *>(right);
  Py_XINCREF(object);
  return *this;
}

PObject PObject::operator()(const PObject& args)
{
  if(!PyCallable_Check(object))
  {
    PyErr_SetString(PyExc_TypeError, "object is not callable");
    return NULL;
  }

  return PyObject_CallObject(object, args);
}

PObject PObject::operator()(const PObject& args, const PObject& kwargs)
{
  if(!PyCallable_Check(object))
  {
    PyErr_SetString(PyExc_TypeError, "object is not callable");
    return NULL;
  }

  return PyObject_Call(object, args, kwargs);
}

PString PObject::str() const
{
  PyObject *str = PyObject_Str(object);
  return PString(str);
}

const PObject& PObject::incref()
{
  Py_XINCREF(object);
  return *this;
}

void PObject::decref()
{
  Py_XDECREF(object);
}


// statics

const PObject PObject::None()
{
  Py_INCREF(Py_None);
  return Py_None;
}
