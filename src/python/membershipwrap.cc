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

#include <Python.h>
#include <structmember.h>
#include "stdinc.h"
#include "python/membershipwrap.h"
#include "python/pythonutil.h"
#include "channel.h"
#include "client.h"

template<> PyTypeObject PythonWrap<MembershipWrap, Membership>::type_object = {};
template<> PyTypeObject PythonWrap<MembershipWrap, ChannelMemberList>::type_object = {};
template<> PyTypeObject PythonWrap<MembershipWrap, ClientMemberList>::type_object = {};

static PyMethodDef membership_methods[] =
{
  PY_METHOD_END
};

enum Property
{
  Channel     = 0x00000001,
  Client      = 0x00000002,
  Flags       = 0x00000003,
  PropMask    = 0x000000ff
};

enum PropertyFlag
{
  StringArg   = 0x00000100,
  BoolArg     = 0x00000200,
  IntArg      = 0x00000400
};

static PyGetSetDef membership_getsetters[] = 
{
  PY_GETSET("Channel", MembershipWrap, "Channel", Channel),
  PY_GETSET("Client", MembershipWrap, "Client", Client),
  PY_GETSET("Flags", MembershipWrap, "Flags", Flags | IntArg),
  PY_GETSET_END
};

MembershipWrap::MembershipWrap(PyObject *args, PyObject *kwds) : PythonWrap(args, kwds)
{
}

MembershipWrap::~MembershipWrap()
{
}

// Statics

void MembershipWrap::init(PyObject *module)
{
  PythonWrap<MembershipWrap, Membership>::type_object.tp_methods = membership_methods;
  PythonWrap<MembershipWrap, Membership>::type_object.tp_getset = membership_getsetters;
  PythonWrap<MembershipWrap, Membership>::init(module, "Membership");
}

PyObject *MembershipWrap::get_wrap(MembershipWrap *self, void *closure)
{
  int prop = *reinterpret_cast<int *>(&closure);
  PyObject *value;
  Membership ms;

  switch(prop & PropMask)
  {
  case Channel:
    ms = self->get_wrapped();
    value = ChannelWrap::wrap(&ms.channel);
    break;
  case Client:
    ms = self->get_wrapped();
    value = ClientWrap::wrap(&ms.client);
    break;
  case Flags:
    value = PyInt_FromLong(self->get_wrapped().flags);
    break;
  default:
    Logging::warning << "Unknown property requested: " << prop << Logging::endl;
    PyErr_SetString(PyExc_AttributeError, "Unknown property");
    return NULL;
  }

  return value;
}

int MembershipWrap::set_wrap(MembershipWrap *self, PyObject *value, void *closure)
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
