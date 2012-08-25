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

#ifndef POBJECT_H_INC
#define POBJECT_H_INC

#include "Python.h"
#include "stdinc.h"

class PString;

class PObject
{
protected:
  PyObject *object;
public:
  PObject();
  PObject(PyObject *);
  PObject(int a) { }; // empty constructor only for use in derived classes
  PObject(const PObject &);
  ~PObject();

  virtual operator PyObject *() const
  {
    return object;
  }

  virtual PObject& operator=(const PObject& right)
  {
    Logging::trace << "python object defref(assign) " << object << Logging::endl;
    Py_XDECREF(object);
    object = right;
    Py_XINCREF(object);
    Logging::trace << "python object incref(assign) " << object << Logging::endl;
    return *this;
  }

  virtual PObject& operator=(const PyObject *right)
  {
    return *this;
  }

  virtual PObject operator()(const PObject& args)
  {
    if(!PyCallable_Check(object))
      Py_RETURN_NONE;

    return PyObject_CallObject(object, args);
  }

  virtual PString str();
};

#endif
