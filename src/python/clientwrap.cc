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
#include "python/eventwrap.h"
#include "numeric.h"

template class PythonWrap<ClientWrap>;

ClientWrap *ClientWrap::me;
PyObject *ClientWrap::connected;
PyObject *ClientWrap::registered;
PyObject *ClientWrap::disconnected;

static PyMethodDef client_methods[] =
{
  { "add", reinterpret_cast<PyCFunction>(ClientWrap::add), 
    METH_STATIC, "Register the client" },
  { "is_registered", reinterpret_cast<PyCFunction>(ClientWrap::is_registered),
    METH_NOARGS, "Check if a client is registered or not" },
  { "numeric", reinterpret_cast<PyCFunction>(ClientWrap::numeric),
    METH_VARARGS, "Send the client a numeric" },
  { "send", reinterpret_cast<PyCFunction>(ClientWrap::send),
    METH_KEYWORDS | METH_VARARGS, "Send the client a message" },
  { NULL, NULL, 0, NULL }
};

static PyMemberDef client_members[] =
{
  { NULL, 0, 0, 0, NULL }
};

static PyGetSetDef client_getsetters[] = 
{
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
  PythonWrap<ClientWrap>::str = reinterpret_cast<reprfunc>(str);
  PythonWrap<ClientWrap>::init("Client");

  ClientPtr ptr = Client::get_me();
  me = ClientWrap::wrap(&ptr);
  if(me == NULL)
  {
    PyErr_Print();
    throw runtime_error("Python failed to initialise");
  }

  connected = PyObject_CallObject(EventWrap::get_type(), NULL);
  registered = PyObject_CallObject(EventWrap::get_type(), NULL);
  disconnected = PyObject_CallObject(EventWrap::get_type(), NULL);

  PyDict_SetItemString(type_object.tp_dict, "Me", reinterpret_cast<PyObject *>(me));
  PyDict_SetItemString(type_object.tp_dict, "connected", connected);
  PyDict_SetItemString(type_object.tp_dict, "registered", connected);
  PyDict_SetItemString(type_object.tp_dict, "disconnected", connected);

  Client::connected += function<bool(ClientPtr)>(on_connected);
  Client::registered += function<bool(ClientPtr)>(on_registered);
  Client::disconnected += function<bool(ClientPtr)>(on_disconnected);
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
  ClientWrap *client;

  
  if(!PyArg_Parse(args, "O", &client))
    return NULL;

  if(Py_TYPE(client) != &type_object)
  {
    PyErr_SetString(PyExc_TypeError, "argument must be a Client object");
    return NULL;
  }

  Client::add(client->client);

  Py_RETURN_NONE;
}

PyObject *
ClientWrap::send(ClientWrap *self, PyObject *args, PyObject *kwargs)
{
  PyObject *fmt, *result, *meth, *fargs, *fdict;

  fmt = PyTuple_GetItem(args, 0);

  if(!PyString_Check(fmt))
  {
    PyErr_SetString(PyExc_TypeError, "you must pass a string as the first parameter");
    return NULL;
  }

  fdict = PyDict_New();
  PyDict_SetItemString(fdict, "client", reinterpret_cast<PyObject *>(self));

  if (kwargs != NULL)
    PyDict_Update(fdict, kwargs);

  meth = PyObject_GetAttrString(fmt, "format");

  fargs = PyTuple_New(0);
  result = PyObject_Call(meth, fargs, fdict);

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

PyObject *
ClientWrap::str(ClientWrap *self)
{
  return PyString_FromString(self->client->str().c_str());
}

PyObject *
ClientWrap::numeric(ClientWrap *self, PyObject *args)
{
  PyObject *item;
  string format;
  stringstream output;
  int index = 0;
  int numeric;

  item = PyTuple_GetItem(args, index++);
  if(!PyInt_Check(item))
  {
    PyErr_SetString(PyExc_TypeError, "First argument must be an integer");
    return NULL;
  }

  numeric = PyInt_AsLong(item);
  format = Numeric::raw_format(numeric);
  for(string::const_iterator i = format.begin(); i != format.end(); i++)
  {
    char c = *i;

    if(c == '%')
    {
      i++;
      item = PyTuple_GetItem(args, index++);
      if(item == NULL)
      {
        PyErr_SetString(PyExc_TypeError, "Not enough arguments for numeric format");
        return NULL;
      }
      switch(*i)
      {
      case 's':
        if(!PyString_Check(item))
        {
          stringstream ss;
          ss << "expected string argument for argument " << index;
          PyErr_SetString(PyExc_TypeError, ss.str().c_str());
          return NULL;
        }
        output << PyString_AsString(item);
        break;
      case 'd':
        if(!PyInt_Check(item))
        {
          stringstream ss;
          ss << "expected int argument for argument " << index;
          PyErr_SetString(PyExc_TypeError, ss.str().c_str());
          return NULL;
        }

        output << PyInt_AsLong(item);
        break;
      default:
        throw runtime_error("format specifier not implemented yet");
        break;
      }

      i++;
      if(i == format.end())
        break;
    }

    output << c;
  }

  self->client->send(output.str(), numeric);

  Py_RETURN_NONE;
}

bool
ClientWrap::on_connected(ClientPtr client)
{
  PyObject *args;
  bool ret;

  args = Py_BuildValue("(O)", wrap(&client));

  ret = handle_event(connected, args);

  Py_DECREF(args);

  return ret;
}

bool
ClientWrap::on_registered(ClientPtr client)
{
  PyObject *args;
  bool ret;

  args = Py_BuildValue("(O)", wrap(&client));

  ret = handle_event(registered, args);

  Py_DECREF(args);

  return ret;
}

bool
ClientWrap::on_disconnected(ClientPtr client)
{
  PyObject *args;
  bool ret;

  args = Py_BuildValue("(O)", wrap(&client));

  ret = handle_event(disconnected, args);

  Py_DECREF(args);

  return ret;
}
