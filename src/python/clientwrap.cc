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
#include "logging.h"

template class PythonWrap<ClientWrap>;

template void PythonWrap<ClientWrap>::init(const char *);

static PyMethodDef client_methods[] =
{
  { NULL, NULL, 0, NULL }
};

static PyMemberDef client_members[] =
{
  { NULL, 0, 0, 0, NULL }
};

static PyGetSetDef client_getsetters[] = {
  { "Name", ClientWrap::get_name_wrap, ClientWrap::set_name_wrap, "Name", NULL},
  { NULL, NULL, NULL, NULL, NULL }
};

ClientWrap::ClientWrap(Client &_client) : client(_client)
{
  Logging::debug << "ClientWrap(): " << (void *)this << Logging::endl;
}

ClientWrap::ClientWrap(PyObject *args, PyObject *kwds) : client(Client())
{
  PyObject *client_obj;
  Client *ptr;

  PyArg_ParseTuple(args, "O", &client_obj);

  ptr = (reinterpret_cast<Client*>(PyCapsule_GetPointer(client_obj, "client_ptr")));
  client = *ptr;

  Logging::debug << "ClientWrap(py): " << (void *)this << Logging::endl;
}

ClientWrap::~ClientWrap()
{
  Logging::debug << "~ClientWrap: " << (void *)this << Logging::endl;
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
ClientWrap::get_name_wrap(PyObject *self, void *closure)
{
  ClientWrap *client = reinterpret_cast<ClientWrap*>(self);
  PyObject *name = PyString_FromString(client->get_name());

  return name;
}

int
ClientWrap::set_name_wrap(PyObject *self, PyObject *value, void *closure)
{
  ClientWrap *client = reinterpret_cast<ClientWrap*>(self);

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

  client->set_name(PyString_AsString(value));

  return 0;
}