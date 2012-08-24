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

#if 0
#include <Python.h>
#include <structmember.h>
#include "stdinc.h"
#include "python/nuhmaskwrap.h"
#include "python/pythonwrap.h"
#include "nuhmask.h"

static PyMethodDef mask_methods[] =
{
  PY_METHOD_END
};

enum Property
{
  PropMask    = 0x000000ff
};

enum PropertyFlag
{
  StringArg   = 0x00000100,
  BoolArg     = 0x00000200,
  IntArg      = 0x00000400
};

static PyGetSetDef mask_getsetters[] = 
{
  PY_GETSET_END
};

NuhMaskWrap::NuhMaskWrap(PyObject *args, PyObject *kwds) : PythonWrap(args, kwds)
{
}

NuhMaskWrap::~NuhMaskWrap()
{
}

// Statics

void NuhMaskWrap::init(PyObject *module)
{
  PyTypeObject& type = type_object();

  type.tp_methods = mask_methods;
  type.tp_getset = mask_getsetters;
  type.tp_name = "Channel";
  PythonWrap::init(module);
}

PyObject *NuhMaskWrap::get_wrap(NuhMaskWrap *self, void *closure)
{
  int prop = *reinterpret_cast<int *>(&closure);
  PyObject *value;

  switch(prop & PropMask)
  {
  default:
    Logging::warning << "Unknown property requested: " << prop << Logging::endl;
    PyErr_SetString(PyExc_AttributeError, "Unknown property");
    return NULL;
  }

  return value;
}

int NuhMaskWrap::set_wrap(NuhMaskWrap *self, PyObject *value, void *closure)
{
  int prop = *reinterpret_cast<int *>(&closure);

  if(value == NULL)
  {
    PyErr_SetString(PyExc_TypeError, "Cannot delete the attribute");
    return -1;
  } 

  if(prop & StringArg)
  {
    if(!PyString_Check(value))
    {
      PyErr_SetString(PyExc_TypeError, "Property must be a string value");
      return -1;
    }
  }

  if(prop & BoolArg)
  {
    if(!PyBool_Check(value))
    {
      PyErr_SetString(PyExc_TypeError, "Property must be a bool value");
      return -1;
    }
  }

  if(prop & IntArg)
  {
    if(!PyInt_Check(value))
    {
      PyErr_SetString(PyExc_TypeError, "Property must be an int value");
      return -1;
    }
  }

  switch(prop & PropMask)
  {
  default:
    Logging::warning << "Unknown property requested: " << prop << Logging::endl;
    PyErr_SetString(PyExc_AttributeError, "Unknown property");
    return -1;
  }

  return 0;
}
#endif
