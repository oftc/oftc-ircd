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
#include "python/clientwrap.h"

template class PythonWrap<ClientWrap>;

static PyMethodDef client_methods[] =
{
  { "send", reinterpret_cast<PyCFunction>(ClientWrap::send),
    METH_KEYWORDS | METH_VARARGS, "Send the client a message" },
  { "add", reinterpret_cast<PyCFunction>(ClientWrap::add), 
    METH_NOARGS, "Register the client" },
  { "is_registered", reinterpret_cast<PyCFunction>(ClientWrap::is_registered),
    METH_NOARGS, "Check if a client is registered or not" },
  { NULL, NULL, 0, NULL }
};

static PyMemberDef client_members[] =
{
  { NULL, 0, 0, 0, NULL }
};

static PyGetSetDef client_getsetters[] = {
  { const_cast<char*>("Name"), reinterpret_cast<getter>(ClientWrap::get_wrap), 
    reinterpret_cast<setter>(ClientWrap::set_wrap), const_cast<char*>("Name"), 
    reinterpret_cast<void *>(const_cast<char *>("name")) },
  { const_cast<char*>("Username"), reinterpret_cast<getter>(ClientWrap::get_wrap), 
    reinterpret_cast<setter>(ClientWrap::set_wrap), const_cast<char*>("Username"), 
    reinterpret_cast<void *>(const_cast<char *>("username")) },
  { const_cast<char*>("Realname"), reinterpret_cast<getter>(ClientWrap::get_wrap), 
    reinterpret_cast<setter>(ClientWrap::set_wrap), const_cast<char*>("Real Name"), 
    reinterpret_cast<void *>(const_cast<char *>("realname")) },

  { NULL, NULL, NULL, NULL, NULL }
};

ClientWrap::ClientWrap(PyObject *args, PyObject *kwds)
{
  PyObject *client_obj;
  ClientPtr ptr;

  PyArg_ParseTuple(args, "O", &client_obj);

  ptr = *(reinterpret_cast<ClientPtr*>(PyCObject_AsVoidPtr(client_obj)));
  client = ptr;
}

ClientWrap::~ClientWrap()
{
}

// Statics

void
ClientWrap::init()
{
  PythonWrap<ClientWrap>::methods = client_methods;
  PythonWrap<ClientWrap>::members = client_members;
  PythonWrap<ClientWrap>::getsetters = client_getsetters;
  PythonWrap<ClientWrap>::init("Client");
}

PyObject *
ClientWrap::get_wrap(ClientWrap *self, void *closure)
{
  string prop = string(static_cast<char *>(closure));
  string value;

  if(prop == "name")
    value = self->client->get_name();
  else if(prop == "username")
    value = self->client->get_username();
  else if(prop == "realname")
    value = self->client->get_realname();

  PyObject *name = PyString_FromString(value.c_str());

  return name;
}

int
ClientWrap::set_wrap(ClientWrap *self, PyObject *value, void *closure)
{
  string prop = string(static_cast<char *>(closure));

  if(value == NULL)
  {
    PyErr_SetString(PyExc_TypeError, "Cannot delete the Name attribute");
    return -1;
  }

  if(!PyString_Check(value))
  {
    PyErr_SetString(PyExc_TypeError, "Name must be a string");
    return -1;
  }

  if(prop == "name")
    self->client->set_name(PyString_AsString(value));
  else if(prop == "username")
    self->client->set_username(PyString_AsString(value));
  else if(prop == "realname")
    self->client->set_realname(PyString_AsString(value));

  return 0;
}

PyObject *
ClientWrap::add(ClientWrap *self, PyObject *args)
{
  self->client->add(self->client);

  Py_RETURN_NONE;
}

PyObject *
ClientWrap::send(ClientWrap *self, PyObject *args, PyObject *kwargs)
{
  PyObject *fmt, *result, *meth, *fargs;

  fmt = PyTuple_GetItem(args, 0);

  if(!PyString_Check(fmt))
  {
    PyErr_SetString(PyExc_TypeError, "you must pass a string as the first parameter");
    return NULL;
  }

  PyDict_SetItemString(kwargs, "client", reinterpret_cast<PyObject *>(self));

  meth = PyObject_GetAttrString(fmt, "format");

  fargs = PyTuple_New(0);
  result = PyObject_Call(meth, fargs, kwargs);

  Py_DECREF(meth);

  if(result == NULL)
  {
    return NULL;
  }

  self->client->send(PyString_AsString(result));

  Py_DECREF(result);
  Py_DECREF(fargs);

  Py_RETURN_NONE;
}

PyObject *
ClientWrap::is_registered(ClientWrap *self, PyObject *args)
{
  if(self->client->is_registered())
    Py_RETURN_TRUE;
  else
    Py_RETURN_FALSE;
}
