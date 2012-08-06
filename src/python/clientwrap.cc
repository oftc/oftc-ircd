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
#include "python/pythonloader.h"
#include "numeric.h"
#include "client.h"
#include "server.h"

template class PythonWrap<ClientWrap>;

ClientWrap *ClientWrap::me;
PyObject *ClientWrap::connected;
PyObject *ClientWrap::registered;
PyObject *ClientWrap::disconnected;

static PyMethodDef client_methods[] =
{
  { "add", reinterpret_cast<PyCFunction>(ClientWrap::add), 
    METH_STATIC, "Register the client" },
  { "add_name", reinterpret_cast<PyCFunction>(ClientWrap::add_name), 
    METH_STATIC, "Add a name to the names list" },
  { "find_by_name", reinterpret_cast<PyCFunction>(ClientWrap::find_by_name), 
    METH_STATIC, "find a client by name" },
  { "close", reinterpret_cast<PyCFunction>(ClientWrap::close), 
    0, "Close a client connection" },
  { "del_name", reinterpret_cast<PyCFunction>(ClientWrap::del_name), 
    METH_STATIC, "delete a client name from the names list" },
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

enum Property
{
  Name        = 0x00000001,
  Username    = 0x00000002,
  Realname    = 0x00000003,
  Host        = 0x00000004,
  Info        = 0x00000005,
  Invisible   = 0x00000006,
  Idletime    = 0x00000007,
  LastMessage = 0x00000008,
  PropMask    = 0x000000ff
};

enum PropertyFlag
{
  ClientOnly  = 0x00000100,
  ServerOnly  = 0x00000200,
  StringArg   = 0x00000400,
  BoolArg     = 0x00000800,
  IntArg      = 0x00001000
};

static PyGetSetDef client_getsetters[] = 
{
  { const_cast<char*>("Name"), reinterpret_cast<getter>(ClientWrap::get_wrap), 
    reinterpret_cast<setter>(ClientWrap::set_wrap), const_cast<char*>("Name"), 
    reinterpret_cast<void *>(Name | StringArg) },
  { const_cast<char*>("Username"), reinterpret_cast<getter>(ClientWrap::get_wrap), 
    reinterpret_cast<setter>(ClientWrap::set_wrap), const_cast<char*>("Username"), 
    reinterpret_cast<void *>(Username | StringArg | ClientOnly) },
  { const_cast<char*>("Realname"), reinterpret_cast<getter>(ClientWrap::get_wrap), 
    reinterpret_cast<setter>(ClientWrap::set_wrap), const_cast<char*>("Real Name"), 
    reinterpret_cast<void *>(Realname | StringArg | ClientOnly) },
  { const_cast<char*>("Host"), reinterpret_cast<getter>(ClientWrap::get_wrap), 
    reinterpret_cast<setter>(ClientWrap::set_wrap), const_cast<char*>("Host"), 
    reinterpret_cast<void *>(Host | StringArg) },
  { const_cast<char*>("Info"), reinterpret_cast<getter>(ClientWrap::get_wrap), 
    reinterpret_cast<setter>(ClientWrap::set_wrap), const_cast<char*>("Info"), 
    reinterpret_cast<void *>(Info | StringArg | ServerOnly) },
  { const_cast<char*>("Invisible"), reinterpret_cast<getter>(ClientWrap::get_wrap), 
    reinterpret_cast<setter>(ClientWrap::set_wrap), const_cast<char*>("Invisible"), 
    reinterpret_cast<void *>(Invisible | BoolArg | ClientOnly) },
  { const_cast<char*>("Idletime"), reinterpret_cast<getter>(ClientWrap::get_wrap), 
    reinterpret_cast<setter>(ClientWrap::set_wrap), const_cast<char*>("Idle Time"), 
    reinterpret_cast<void *>(Idletime | IntArg | ClientOnly) },
  { const_cast<char*>("LastMessage"), reinterpret_cast<getter>(ClientWrap::get_wrap), 
    reinterpret_cast<setter>(ClientWrap::set_wrap), const_cast<char*>("Idle Time"), 
    reinterpret_cast<void *>(LastMessage | IntArg | ClientOnly) },
  { NULL, NULL, NULL, NULL, NULL }
};

ClientWrap::ClientWrap(PyObject *args, PyObject *kwds)
{
  PyObject *client_obj;
  ClientPtr ptr;

  PyArg_ParseTuple(args, "O", &client_obj);

  ptr = *(reinterpret_cast<ClientPtr*>(PyCObject_AsVoidPtr(client_obj)));
  client = ptr;

  Logging::trace << "Created ClientWrap: " << this << Logging::endl;
}

ClientWrap::~ClientWrap()
{
  Logging::trace << "Destroyed ClientWrap: " << this << Logging::endl;
}

// Statics

void ClientWrap::init(PyObject *module)
{
  PythonWrap<ClientWrap>::type_object.tp_methods = client_methods;
  PythonWrap<ClientWrap>::type_object.tp_members = client_members;
  PythonWrap<ClientWrap>::type_object.tp_getset = client_getsetters;
  PythonWrap<ClientWrap>::type_object.tp_compare = reinterpret_cast<cmpfunc>(compare);
  PythonWrap<ClientWrap>::type_object.tp_str = reinterpret_cast<reprfunc>(str);
  PythonWrap<ClientWrap>::init(module, "Client");

  ClientPtr ptr = Server::get_me();
  me = ClientWrap::wrap(&ptr);
  if(me == NULL)
  {
    PythonLoader::log_error();
    throw runtime_error("Python failed to initialise");
  }

  connected = PythonWrap<EventWrap>::call(NULL);
  registered = PythonWrap<EventWrap>::call(NULL);
  disconnected = PythonWrap<EventWrap>::call(NULL);

  PyDict_SetItemString(type_object.tp_dict, "Me", me);
  PyDict_SetItemString(type_object.tp_dict, "connected", connected);
  PyDict_SetItemString(type_object.tp_dict, "registered", registered);
  PyDict_SetItemString(type_object.tp_dict, "disconnected", disconnected);

  Client::connected += function<bool(ClientPtr)>(on_connected);
  Client::registered += function<bool(ClientPtr)>(on_registered);
  Client::disconnected += function<bool(ClientPtr)>(on_disconnected);
}

PyObject *ClientWrap::get_wrap(ClientWrap *self, void *closure)
{
  int prop = reinterpret_cast<int>(closure);
  PyObject *value;
  shared_ptr<Client> client_ptr;
  shared_ptr<Server> server_ptr;

  if(prop & ClientOnly)
  {
    if(!Client::is_client(self->client))
    {
      PyErr_SetString(PyExc_TypeError, "Property only applicable to Client types");
      return NULL;
    }

    client_ptr = dynamic_pointer_cast<Client>(self->client);
  }

  if(prop & ServerOnly)
  {
    if(!Client::is_server(self->client))
    {
      PyErr_SetString(PyExc_TypeError, "Property only applicable to Server types");
      return NULL;
    }

    server_ptr = dynamic_pointer_cast<Server>(self->client);
  }

  switch(prop & PropMask)
  {
  case Name:
    value = PyString_FromString(self->client->get_name().c_str());
    break;
  case Host:
    value = PyString_FromString(self->client->get_host().c_str());
    break;
  case Username:
    value = PyString_FromString(client_ptr->get_username().c_str());
    break;
  case Realname:
    value = PyString_FromString(client_ptr->get_realname().c_str());
    break;
  case Info:
    value = PyString_FromString(server_ptr->get_info().c_str());
    break;
  case Invisible:
    value = PyBool_FromLong(client_ptr->is_invisible());
    break;
  case Idletime:
    value = PyInt_FromLong(client_ptr->get_idletime());
    break;
  default:
    Logging::warning << "Unknown property requested: " << prop << Logging::endl;
    PyErr_SetString(PyExc_AttributeError, "Unknown property");
    return NULL;
  }

  return value;
}

int ClientWrap::set_wrap(ClientWrap *self, PyObject *value, void *closure)
{
  int prop = reinterpret_cast<int>(closure);
  shared_ptr<Client> client_ptr;
  shared_ptr<Server> server_ptr;

  if(value == NULL)
  {
    PyErr_SetString(PyExc_TypeError, "Cannot delete the attribute");
    return -1;
  } 

  if(prop & ClientOnly)
  {
    if(!Client::is_client(self->client))
    {
      PyErr_SetString(PyExc_TypeError, "Property only applicable to Client types");
      return -1;
    }

    client_ptr = dynamic_pointer_cast<Client>(self->client);
  }

  if(prop & ServerOnly)
  {
    if(!Client::is_server(self->client))
    {
      PyErr_SetString(PyExc_TypeError, "Property only applicable to Server types");
      return -1;
    }

    server_ptr = dynamic_pointer_cast<Server>(self->client);
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
  case Name:
    self->client->set_name(PyString_AsString(value));
    break;
  case Username:
    client_ptr->set_username(PyString_AsString(value));
    break;
  case Realname:
    client_ptr->set_realname(PyString_AsString(value));
    break;
  case Host:
    self->client->set_host(PyString_AsString(value));
    break;
  case Invisible:
    client_ptr->set_invisible(value == Py_True);
    break;
  case LastMessage:
    client_ptr->set_last_message(PyInt_AsLong(value));
    break;
  default:
    Logging::warning << "Unknown property requested: " << prop << Logging::endl;
    PyErr_SetString(PyExc_AttributeError, "Unknown property");
    return -1;
  }

  return 0;
}

PyObject *ClientWrap::add(ClientWrap *self, ClientWrap *client)
{
  if(Py_TYPE(client) != &type_object)
  {
    PyErr_SetString(PyExc_TypeError, "argument must be a Client object");
    return NULL;
  }

  Client::add(client->client);

  Py_RETURN_NONE;
}

PyObject *ClientWrap::add_name(PyObject *self, ClientWrap *client)
{
  if(Py_TYPE(client) != &type_object)
  {
    PyErr_SetString(PyExc_TypeError, "Argument must be a Client");
    return NULL;
  }
  Client::add_name(client->client);

  Py_RETURN_NONE;
}

PyObject *ClientWrap::close(ClientWrap *self, PyObject *args)
{
  if(!PyString_Check(args))
  {
    PyErr_SetString(PyExc_TypeError, "argument must be a string");
    return NULL;
  }
  self->client->close(PyString_AsString(args));

  Py_RETURN_NONE;
}

PyObject *ClientWrap::find_by_name(PyObject *self, PyObject *name)
{
  if(!PyString_Check(name))
  {
    PyErr_SetString(PyExc_TypeError, "argument must be a string");
    return NULL;
  }

  ClientPtr ptr = Client::find_by_name(PyString_AsString(name));

  if(!ptr)
    Py_RETURN_NONE;

  return wrap(&ptr);
}

PyObject *ClientWrap::del_name(PyObject *self, ClientWrap *client)
{
  if(Py_TYPE(client) != &type_object)
  {
    PyErr_SetString(PyExc_TypeError, "argument must be a Client object");
    return NULL;
  }

  Client::del_name(client->client);

  Py_RETURN_NONE;
}

PyObject *ClientWrap::send(ClientWrap *self, PyObject *args, PyObject *kwargs)
{
  PyObject *fmt, *result, *meth, *fargs, *fdict;

  fmt = PyTuple_GetItem(args, 0);

  if(!PyString_Check(fmt))
  {
    PyErr_SetString(PyExc_TypeError, "you must pass a string as the first parameter");
    return NULL;
  }

  fdict = PyDict_New();
  PyDict_SetItemString(fdict, "client", self);
  PyDict_SetItemString(fdict, "me", me);

  if (kwargs != NULL)
    PyDict_Update(fdict, kwargs);

  meth = PyObject_GetAttrString(fmt, "format");

  fargs = PyTuple_New(0);
  result = PyObject_Call(meth, fargs, fdict);

  Py_DECREF(meth);
  Py_DECREF(fdict);
  Py_DECREF(fargs);

  if(result == NULL)
  {
    return NULL;
  }

  self->client->send(PyString_AsString(result));

  Py_DECREF(result);

  Py_RETURN_NONE;
}

PyObject *ClientWrap::is_registered(ClientWrap *self, PyObject *args)
{
  if(self->client->is_registered())
    Py_RETURN_TRUE;
  else
    Py_RETURN_FALSE;
}

PyObject *ClientWrap::str(ClientWrap *self)
{
  return PyString_FromString(self->client->str().c_str());
}

PyObject *ClientWrap::numeric(ClientWrap *self, PyObject *args)
{
  PyObject *item;
  string format;
  stringstream output;
  int index = 0;
  int numeric;
  shared_ptr<Client> ptr;

  if(!Client::is_client(self->client))
  {
    PyErr_SetString(PyExc_RuntimeError, "Cannot send a numeric to a non-client");
    return NULL;
  }

  ptr = dynamic_pointer_cast<Client>(self->client);

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

    c = *i;
    output << c;
  }

  ptr->send(output.str(), numeric);

  Py_RETURN_NONE;
}

int ClientWrap::compare(ClientWrap *self, ClientWrap *other)
{
  if(self->ob_type != &type_object || other->ob_type != &type_object)
    return PyObject_Compare(self, other);

  if(self->client == other->client)
    return 0;

  if(self->client->get_name() > other->client->get_name())
    return 1;
  else
    return -1;
}

bool ClientWrap::on_connected(ClientPtr client)
{
  PyObject *args, *ptr;
  bool ret;

  ptr = wrap(&client);
  args = Py_BuildValue("(O)", ptr);

  ret = handle_event(connected, args);

  Py_DECREF(args);
  Py_DECREF(ptr);

  return ret;
}

bool ClientWrap::on_registered(ClientPtr client)
{
  PyObject *args, *ptr;
  bool ret;

  ptr = wrap(&client);

  args = Py_BuildValue("(O)", ptr);

  ret = handle_event(registered, args);

  Py_DECREF(args);
  Py_DECREF(ptr);

  return ret;
}

bool ClientWrap::on_disconnected(ClientPtr client)
{
  PyObject *args, *ptr;
  bool ret;

  ptr = wrap(&client);

  args = Py_BuildValue("(O)", ptr);

  ret = handle_event(disconnected, args);

  Py_DECREF(args);
  Py_DECREF(ptr);

  return ret;
}
