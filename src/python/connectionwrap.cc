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
#include "python/pythonwrap.h"
#include "python/eventwrap.h"
#include "python/connectionwrap.h"
#include "python/pythonloader.h"

template<> PyTypeObject PythonWrap<ConnectionWrap>::type_object = {};

PyObject *ConnectionWrap::ip_connecting;
PyObject *ConnectionWrap::dns_finished;

static PyMethodDef connection_methods[] =
{
  { NULL, NULL, 0, NULL }
};

static PyMemberDef connection_members[] =
{
  { NULL, 0, 0, 0, NULL }
};

static PyGetSetDef connection_getsetters[] = 
{
  { NULL, NULL, NULL, NULL, NULL }
};

ConnectionWrap::ConnectionWrap(PyObject *args, PyObject *kwds)
{
  PyObject *connection_obj;
  ConnectionPtr ptr;

  PyArg_ParseTuple(args, "O", &connection_obj);

  ptr = *(reinterpret_cast<ConnectionPtr*>(PyCObject_AsVoidPtr(connection_obj)));
  connection = ptr;

  Logging::trace << "Created ConnectionWrap: " << this << Logging::endl;
}

ConnectionWrap::~ConnectionWrap()
{
  Logging::trace << "Destroyed ConnectionWrap: " << this << Logging::endl;
}

// Statics

void ConnectionWrap::init(PyObject *module)
{
  PythonWrap<ConnectionWrap>::type_object.tp_methods = connection_methods;
  PythonWrap<ConnectionWrap>::type_object.tp_members = connection_members;
  PythonWrap<ConnectionWrap>::type_object.tp_getset = connection_getsetters;
  PythonWrap<ConnectionWrap>::init(module, "Connection");

  ip_connecting = PythonWrap<EventWrap>::call(NULL);
  dns_finished = PythonWrap<EventWrap>::call(NULL);

  PyDict_SetItemString(type_object.tp_dict, "ip_connecting", ip_connecting);
  PyDict_SetItemString(type_object.tp_dict, "dns_finished", dns_finished);

  Connection::ip_connecting += function<bool(ConnectionPtr)>(on_ip_connecting);
  Connection::dns_finished += function<bool(ConnectionPtr)>(on_dns_finished);
}

bool ConnectionWrap::on_ip_connecting(ConnectionPtr connection)
{
  PyObject *args, *ptr;
  bool ret;

  ptr = wrap(&connection);
  args = Py_BuildValue("(O)", ptr);

  ret = handle_event(ip_connecting, args);

  Py_DECREF(args);
  Py_DECREF(ptr);

  return ret;
}

bool ConnectionWrap::on_dns_finished(ConnectionPtr connection)
{
  PyObject *args, *ptr;
  bool ret;

  ptr = wrap(&connection);

  args = Py_BuildValue("(O)", ptr);

  ret = handle_event(dns_finished, args);

  Py_DECREF(args);
  Py_DECREF(ptr);

  return ret;
}

