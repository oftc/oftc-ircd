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
#include "python/collectionwrap.h"
#include "python/membershipwrap.h"
#include "python/pythonutil.h"
#include "numeric.h"
#include "client.h"
#include "server.h"

ClientWrap *ClientWrap::me;
PyObject *ClientWrap::connected;
PyObject *ClientWrap::registering;
PyObject *ClientWrap::closing;
PyObject *ClientWrap::disconnected;
PyObject *ClientWrap::nick_changing;
PyObject *ClientWrap::nick_changed;

static PyMethodDef client_methods[] =
{
  PY_METHOD("add", ClientWrap::add, METH_STATIC, "Register the client"),
  PY_METHOD("add_name", ClientWrap::add_name, METH_STATIC, "Add a name to the names list"),
  PY_METHOD("find_by_name", ClientWrap::find_by_name, METH_STATIC, "find a client by name"),
  PY_METHOD("close", ClientWrap::close, METH_OLDARGS, "Close a client connection"),
  PY_METHOD("del_name", ClientWrap::del_name, METH_STATIC, "delete a client name from the names list"),
  PY_METHOD("is_registered", ClientWrap::is_registered, METH_NOARGS, "Check if a client is registered or not"),
  PY_METHOD("is_ssl", ClientWrap::is_ssl, METH_NOARGS, "Check if a client is connected with SSL"),
  PY_METHOD("numeric", ClientWrap::numeric, METH_VARARGS, "Send the client a numeric"),
  PY_METHOD("remove_channel", ClientWrap::remove_channel, METH_OLDARGS, "Remove the client from the specified channel"),
  PY_METHOD("send", ClientWrap::send, METH_KEYWORDS | METH_VARARGS, "Send the client a message"),
  PY_METHOD("send_channels_common", ClientWrap::send_channels_common, METH_KEYWORDS | METH_VARARGS, "Send a message to the users that share channels with this client"),
  PY_METHOD_END
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
  Channels    = 0x00000009,
  ServerProp  = 0x0000000a,
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
  PY_GETSET("Name", ClientWrap, "Name", Name | StringArg),
  PY_GETSET("Username", ClientWrap, "Username", Username | StringArg | ClientOnly),
  PY_GETSET("Realname", ClientWrap, "Real Name", Realname | StringArg | ClientOnly),
  PY_GETSET("Host", ClientWrap, "Host", Host | StringArg),
  PY_GETSET("Info", ClientWrap, "Info", Info | StringArg | ServerOnly),
  PY_GETSET("Invisible", ClientWrap, "Invisible", Invisible | BoolArg | ClientOnly),
  PY_GETSET("Idletime", ClientWrap, "Idle Time", Idletime | IntArg | ClientOnly),
  PY_GETSET("LastMessage", ClientWrap, "Idle Time", LastMessage | IntArg | ClientOnly),
  PY_GETSET("Channels", ClientWrap, "Channel list", Channels | ClientOnly),
  PY_GETSET("Server", ClientWrap, "Server client belongs to", ServerProp | ClientOnly),
  PY_GETSET_END
};

ClientWrap::ClientWrap(PyObject *args, PyObject *kwds) : PythonWrap(args, kwds)
{
}

ClientWrap::~ClientWrap()
{
}

// Statics

void ClientWrap::init(PyObject *module)
{
  PyTypeObject& type = type_object();

  type.tp_methods = client_methods;
  type.tp_getset = client_getsetters;
  type.tp_compare = reinterpret_cast<cmpfunc>(compare);
  type.tp_str = reinterpret_cast<reprfunc>(str);
  type.tp_name = "Client";
  PythonWrap::init(module);

  ClientPtr ptr = Server::get_me();
  me = ClientWrap::wrap(&ptr);
  if(me == NULL)
  {
    PythonUtil::log_error();
    throw runtime_error("Python failed to initialise");
  }

  connected = EventWrap::register_event(fire_connected);
  registering = EventWrap::register_event(fire_registering);
  disconnected = EventWrap::register_event(fire_disconnected);
  closing = EventWrap::register_event(fire_closing);
  nick_changing = EventWrap::register_event(fire_nick_changing);
  nick_changed = EventWrap::register_event(fire_nick_changed);

  PyDict_SetItemString(type.tp_dict, "Me", me);
  PyDict_SetItemString(type.tp_dict, "connected", connected);
  PyDict_SetItemString(type.tp_dict, "registering", registering);
  PyDict_SetItemString(type.tp_dict, "disconnected", disconnected);
  PyDict_SetItemString(type.tp_dict, "closing", closing);
  PyDict_SetItemString(type.tp_dict, "nick_changing", nick_changing);
  PyDict_SetItemString(type.tp_dict, "nick_changed", nick_changed);

  Client::connected += function<bool(ClientPtr)>(on_connected);
  Client::registering += function<bool(ClientPtr)>(on_registering);
  Client::disconnected += function<bool(ClientPtr)>(on_disconnected);
  BaseClient::closing += function<bool(ClientPtr, string)>(on_closing);
  Client::nick_changing += function<bool(ClientPtr, irc_string)>(on_nick_changing);
  Client::nick_changed += function<bool(ClientPtr, string)>(on_nick_changed);
}

PyObject *ClientWrap::get_wrap(ClientWrap *self, void *closure)
{
  int prop = *reinterpret_cast<int *>(&closure);
  PyObject *value;
  shared_ptr<Client> client_ptr;
  ServerPtr server_ptr;

  if(prop & ClientOnly)
  {
    if(!Client::is_client(self->get_wrapped()))
    {
      PyErr_SetString(PyExc_TypeError, "Property only applicable to Client types");
      return NULL;
    }

    client_ptr = dynamic_pointer_cast<Client>(self->get_wrapped());
  }

  if(prop & ServerOnly)
  {
    if(!Client::is_server(self->get_wrapped()))
    {
      PyErr_SetString(PyExc_TypeError, "Property only applicable to Server types");
      return NULL;
    }

    server_ptr = dynamic_pointer_cast<Server>(self->get_wrapped());
  }

  switch(prop & PropMask)
  {
  case Name:
    value = PyString_FromString(self->get_wrapped()->get_name().c_str());
    break;
  case Host:
    value = PyString_FromString(self->get_wrapped()->get_host().c_str());
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
  case Channels:
    {
      ClientMemberList channels = client_ptr->get_channels();
      value = CollectionWrap<ClientMemberList, MembershipWrap>::wrap(&channels);
    }
    break;
  case ServerProp:
    {
      ClientPtr tmp = client_ptr->get_server();
      value = ClientWrap::wrap(&tmp);
    }
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
  int prop = *reinterpret_cast<int *>(&closure);
  shared_ptr<Client> client_ptr;
  ServerPtr server_ptr;

  if(value == NULL)
  {
    PyErr_SetString(PyExc_TypeError, "Cannot delete the attribute");
    return -1;
  } 

  if(prop & ClientOnly)
  {
    if(!Client::is_client(self->get_wrapped()))
    {
      PyErr_SetString(PyExc_TypeError, "Property only applicable to Client types");
      return -1;
    }

    client_ptr = dynamic_pointer_cast<Client>(self->get_wrapped());
  }

  if(prop & ServerOnly)
  {
    if(!Client::is_server(self->get_wrapped()))
    {
      PyErr_SetString(PyExc_TypeError, "Property only applicable to Server types");
      return -1;
    }

    server_ptr = dynamic_pointer_cast<Server>(self->get_wrapped());
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
    self->get_wrapped()->set_name(PyString_AsString(value));
    break;
  case Username:
    client_ptr->set_username(PyString_AsString(value));
    break;
  case Realname:
    client_ptr->set_realname(PyString_AsString(value));
    break;
  case Host:
    self->get_wrapped()->set_host(PyString_AsString(value));
    break;
  case Invisible:
    client_ptr->set_invisible(PyObject_IsTrue(value) != 0);
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
  if(!ClientWrap::check(client))
  {
    PyErr_SetString(PyExc_TypeError, "argument must be a Client object");
    return NULL;
  }

  Client::add(client->get_wrapped());

  Py_RETURN_NONE;
}

PyObject *ClientWrap::add_name(PyObject *self, ClientWrap *client)
{
  if(!ClientWrap::check(client))
  {
    PyErr_SetString(PyExc_TypeError, "Argument must be a Client");
    return NULL;
  }
  Client::add_name(client->get_wrapped());

  Py_RETURN_NONE;
}

PyObject *ClientWrap::close(ClientWrap *self, PyObject *args)
{
  if(!PyString_Check(args))
  {
    PyErr_SetString(PyExc_TypeError, "argument must be a string");
    return NULL;
  }
  self->get_wrapped()->close(PyString_AsString(args));

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
  if(!ClientWrap::check(client))
  {
    PyErr_SetString(PyExc_TypeError, "argument must be a Client object");
    return NULL;
  }

  Client::del_name(client->get_wrapped());

  Py_RETURN_NONE;
}

PyObject *ClientWrap::send(ClientWrap *self, PyObject *args, PyObject *kwargs)
{
  PyObject *result = PythonUtil::send_format(self, args, kwargs);

  if(result == NULL)
    return NULL;

  self->get_wrapped()->send(PyString_AsString(result));

  Py_DECREF(result);

  Py_RETURN_NONE;
}

PyObject *ClientWrap::send_channels_common(ClientWrap *self, PyObject *args, PyObject *kwargs)
{
  PyObject *result = PythonUtil::send_format(self, args, kwargs);

  if(result == NULL)
    return NULL;

  shared_ptr<Client> ptr = dynamic_pointer_cast<Client>(self->get_wrapped());
  ptr->send_channels_common(PyString_AsString(result));

  Py_DECREF(result);

  Py_RETURN_NONE;
}

PyObject *ClientWrap::is_registered(ClientWrap *self, PyObject *args)
{
  if(self->get_wrapped()->is_registered())
    Py_RETURN_TRUE;
  else
    Py_RETURN_FALSE;
}

PyObject *ClientWrap::is_ssl(ClientWrap *self, PyObject *args)
{
  if(self->get_wrapped()->is_ssl())
    Py_RETURN_TRUE;
  else
    Py_RETURN_FALSE;
}

PyObject *ClientWrap::str(ClientWrap *self)
{
  return PyString_FromString(self->get_wrapped()->str().c_str());
}

PyObject *ClientWrap::numeric(ClientWrap *self, PyObject *args)
{
  PyObject *item;
  string format;
  stringstream output;
  int index = 0;
  int numeric;
  shared_ptr<Client> ptr;

  if(!Client::is_client(self->get_wrapped()))
  {
    PyErr_SetString(PyExc_RuntimeError, "Cannot send a numeric to a non-client");
    return NULL;
  }

  ptr = dynamic_pointer_cast<Client>(self->get_wrapped());

  item = PyTuple_GetItem(args, index++);
  if(!PyInt_Check(item))
  {
    PyErr_SetString(PyExc_TypeError, "First argument must be an integer");
    return NULL;
  }

  numeric = PyInt_AsLong(item);
  format = Numeric::raw_format(numeric);
  for(auto i = format.begin(); i != format.end(); i++)
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
      case 'c':
        if(!PyString_Check(item))
        {
          stringstream ss;
          ss << "expected string argument for argument " << index;
          PyErr_SetString(PyExc_TypeError, ss.str().c_str());
          return NULL;
        }
        if(*i == 's')
          output << PyString_AsString(item);
        else
          output << PyString_AsString(item)[0];
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

PyObject *ClientWrap::remove_channel(ClientWrap *self, ChannelWrap *channel)
{
  shared_ptr<Client> ptr = dynamic_pointer_cast<Client>(self->get_wrapped());

  if(!ChannelWrap::check(channel))
  {
    PyErr_SetString(PyExc_TypeError, "argument must be a Channel");
    return NULL;
  }

  ptr->remove_channel(channel->get_wrapped());

  Py_RETURN_NONE;
}

int ClientWrap::compare(ClientWrap *self, ClientWrap *other)
{
  if(self->ob_type != &type_object() || other->ob_type != &type_object())
    return PyObject_Compare(self, other);

  if(self->get_wrapped() == other->get_wrapped())
    return 0;

  if(self->get_wrapped()->get_name() > other->get_wrapped()->get_name())
    return 1;
  else
    return -1;
}

PyObject *ClientWrap::fire_connected(EventWrap *, PyObject *args)
{
  ClientWrap *client;

  if(!PyArg_ParseTuple(args, "O", &client))
    return NULL;

  if(Client::connected(client->get_wrapped()))
    return Py_True;
  else
    return Py_False;
}

PyObject *ClientWrap::fire_disconnected(EventWrap *, PyObject *args)
{
  ClientWrap *client;

  if(!PyArg_ParseTuple(args, "O", &client))
    return NULL;

  if(Client::disconnected(client->get_wrapped()))
    return Py_True;
  else
    return Py_False;
}

PyObject *ClientWrap::fire_registering(EventWrap *, PyObject *args)
{
  ClientWrap *client;

  if(!PyArg_ParseTuple(args, "O", &client))
    return NULL;

  if(Client::registering(client->get_wrapped()))
    return Py_True;
  else
    return Py_False;
}

PyObject *ClientWrap::fire_closing(EventWrap *event, PyObject *args)
{
  ClientWrap *client;
  char *reason;

  if(!PyArg_ParseTuple(args, "Os", &client, &reason))
    return NULL;

  if(Client::closing(client->get_wrapped(), reason))
    return Py_True;
  else
    return Py_False;
}

PyObject *ClientWrap::fire_nick_changing(EventWrap *event, PyObject *args)
{
  ClientWrap *client;
  char *nick;

  if(!PyArg_ParseTuple(args, "Os", &client, &nick))
    return NULL;

  if(Client::nick_changing(client->get_wrapped(), nick))
    return Py_True;
  else
    return Py_False;
}

PyObject *ClientWrap::fire_nick_changed(EventWrap *event, PyObject *args)
{
  ClientWrap *client;
  char *old_str;

  if(!PyArg_ParseTuple(args, "Os", &client, &old_str))
    return NULL;

  if(Client::nick_changed(client->get_wrapped(), old_str))
    return Py_True;
  else
    return Py_False;
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

bool ClientWrap::on_registering(ClientPtr client)
{
  PyObject *args, *ptr;
  bool ret;

  ptr = wrap(&client);

  args = Py_BuildValue("(O)", ptr);

  ret = handle_event(registering, args);

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

bool ClientWrap::on_closing(ClientPtr client, string reason)
{
  PyObject *args, *ptr;
  bool ret;

  ptr = wrap(&client);

  args = Py_BuildValue("(Os)", ptr, reason.c_str());

  ret = handle_event(closing, args);

  Py_DECREF(args);
  Py_DECREF(ptr);

  return ret;
}

bool ClientWrap::on_nick_changing(ClientPtr client, irc_string nick)
{
  PyObject *args, *ptr;
  bool ret;

  ptr = wrap(&client);

  args = Py_BuildValue("(Os)", ptr, nick.c_str());

  ret = handle_event(nick_changing, args);

  Py_DECREF(args);
  Py_DECREF(ptr);

  return ret;
}

bool ClientWrap::on_nick_changed(ClientPtr client, string old_str)
{
  PyObject *args, *ptr;
  bool ret;

  ptr = wrap(&client);

  args = Py_BuildValue("(Os)", ptr, old_str.c_str());

  ret = handle_event(nick_changed, args);

  Py_DECREF(args);
  Py_DECREF(ptr);

  return ret;
}
