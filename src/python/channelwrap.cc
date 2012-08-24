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
#include "python/channelwrap.h"
#include "python/pythonutil.h"
#include "python/mapwrap.h"
#include "python/listwrap.h"
#include "python/membershipwrap.h"
#include "python/nuhmaskwrap.h"
#include "channel.h"

PyObject *ChannelWrap::joining;
PyObject *ChannelWrap::joined;

static PyMethodDef channel_methods[] =
{
  PY_METHOD("add", ChannelWrap::add, METH_STATIC, "Create a new channel"),
  PY_METHOD("add_member", ChannelWrap::add_member, METH_OLDARGS, "Add a client to a channel"),
  PY_METHOD("del", ChannelWrap::del, METH_STATIC, "delete a channel"),
  PY_METHOD("find", ChannelWrap::find, METH_STATIC, "find a channel by name"),
  PY_METHOD("is_member", ChannelWrap::is_member, METH_OLDARGS, "Test is a client is on a channel"),
  PY_METHOD("remove_member", ChannelWrap::remove_member, METH_OLDARGS, "Remove a client from the channel"),
  PY_METHOD("set_mode_char", ChannelWrap::set_mode_char, METH_VARARGS, "Set a mode on the channel from its mode char"),
  PY_METHOD("send_names", ChannelWrap::send_names, METH_OLDARGS, "Send names reply to a client"),
  PY_METHOD("send", ChannelWrap::send, METH_KEYWORDS, "Send a message to all clients on the channel"),
  PY_METHOD("supported_modes", ChannelWrap::supported_modes, METH_STATIC | METH_NOARGS, "Get the list of supported channel modes"),
  PY_METHOD_END
};

enum Property
{
  Name = 1,
  Members,
  InviteOnly,
  Moderated,
  NoExternal,
  Private,
  Secret,
  TopicOpsOnly,
  Secure,
  Bans,
  Invexes,
  Quiets,
  Excepts,
  PropMask    = 0x000000ff
};

enum PropertyFlag
{
  StringArg   = 0x00000100,
  BoolArg     = 0x00000200,
  IntArg      = 0x00000400
};

static PyGetSetDef channel_getsetters[] = 
{
  PY_GETSET("Name", ChannelWrap, "Channel Name", Name | StringArg),
  PY_GETSET("Members", ChannelWrap, "Channel Members", Members),
  PY_GETSET("InviteOnly", ChannelWrap, "Whether channel is invite only", InviteOnly | BoolArg),
  PY_GETSET("Moderated", ChannelWrap, "Whether channel is moderated", Moderated | BoolArg),
  PY_GETSET("NoExternal", ChannelWrap, "Whether channel is invite only", NoExternal | BoolArg),
  PY_GETSET("Private", ChannelWrap, "Whether channel is private", Private | BoolArg),
  PY_GETSET("Secret", ChannelWrap, "Whether channel is secret", Secret | BoolArg),
  PY_GETSET("TopicOpsOnly", ChannelWrap, "Whether channel topic is settable by ops only", TopicOpsOnly | BoolArg),
  PY_GETSET("Secure", ChannelWrap, "Whether channel is secure", Secure | BoolArg),
  PY_GETSET("Bans", ChannelWrap, "Channel ban list", Bans),
  PY_GETSET("Invexes", ChannelWrap, "Channel invex list", Invexes),
  PY_GETSET("Quiets", ChannelWrap, "Channel quiet list", Quiets),
  PY_GETSET("Excepts", ChannelWrap, "Channel except list", Excepts),
  PY_GETSET_END
};

ChannelWrap::ChannelWrap(PyObject *args, PyObject *kwds) : PythonWrap(args, kwds)
{
  if(PyTuple_Size(args) == 0)
  {
    wrapped = ChannelPtr(new Channel());
  }
}

ChannelWrap::~ChannelWrap()
{
}

// Statics

void ChannelWrap::init(PyObject *module)
{
  PyTypeObject& type = type_object();

  type.tp_methods = channel_methods;
  type.tp_getset = channel_getsetters;
  type.tp_compare = reinterpret_cast<cmpfunc>(compare);
  type.tp_str = reinterpret_cast<reprfunc>(str);
  type.tp_name = "Channel";
  PythonWrap::init(module);

  joining = EventWrap::register_event(fire_joining);
  joined = EventWrap::register_event(fire_joined);

  PyDict_SetItemString(type.tp_dict, "joining", joining);
  PyDict_SetItemString(type.tp_dict, "joined", joined);

  Channel::joining += function<bool(ChannelPtr, ClientPtr)>(on_joining);
  Channel::joined += function<bool(ChannelPtr, ClientPtr)>(on_joined);
}

PyObject *ChannelWrap::get_wrap(ChannelWrap *self, void *closure)
{
  int prop = *reinterpret_cast<int *>(&closure);
  PyObject *value;

  switch(prop & PropMask)
  {
  case Name:
    value = PyString_FromString(self->get_wrapped()->get_name().c_str());
    break;
  case Members:
    {
      ChannelMemberList channels = self->get_wrapped()->get_members();
      value = MapWrap<ChannelMemberList, ClientWrap, MembershipWrap>::wrap(channels);
    }
    break;
  case InviteOnly:
    value = PyBool_FromLong(self->get_wrapped()->is_invite_only());
    break;
  case Moderated:
    value = PyBool_FromLong(self->get_wrapped()->is_moderated());
    break;
  case NoExternal:
    value = PyBool_FromLong(self->get_wrapped()->is_no_external_msgs());
    break;
  case Private:
    value = PyBool_FromLong(self->get_wrapped()->is_private());
    break;
  case Secret:
    value = PyBool_FromLong(self->get_wrapped()->is_secret());
    break;
  case TopicOpsOnly:
    value = PyBool_FromLong(self->get_wrapped()->is_topic_op_only());
    break;
  case Secure:
    value = PyBool_FromLong(self->get_wrapped()->is_secure());
    break;
  case Bans:
    {
      NuhMaskList bans = self->get_wrapped()->get_bans();
      value = ListWrap<NuhMaskList, NuhMaskWrap>::wrap(bans);
    }
    break;
  default:
    Logging::warning << "Unknown property requested: " << prop << Logging::endl;
    PyErr_SetString(PyExc_AttributeError, "Unknown property");
    return NULL;
  }

  return value;
}

int ChannelWrap::set_wrap(ChannelWrap *self, PyObject *value, void *closure)
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
  case Name:
    self->get_wrapped()->set_name(PyString_AsString(value));
    break;
  default:
    Logging::warning << "Unknown property requested: " << prop << Logging::endl;
    PyErr_SetString(PyExc_AttributeError, "Unknown property");
    return -1;
  }

  return 0;
}

PyObject *ChannelWrap::add_member(ChannelWrap *self, ClientWrap *client)
{
  if(!ClientWrap::check(client) || !BaseClient::is_client(client->get_wrapped()))
  {
    PyErr_SetString(PyExc_TypeError, "Argument must be a Client type");
    return NULL;
  }

  self->get_wrapped()->add_member(client->get_wrapped());

  Py_RETURN_NONE;
}

PyObject *ChannelWrap::add(ChannelWrap *self, ChannelWrap *channel)
{
  if(!ChannelWrap::check(channel))
  {
    PyErr_SetString(PyExc_TypeError, "argument must be a Channel object");
    return NULL;
  }

  Channel::add(channel->get_wrapped());

  Py_RETURN_NONE;
}

PyObject *ChannelWrap::find(PyObject *self, PyObject *name)
{
  if(!PyString_Check(name))
  {
    PyErr_SetString(PyExc_TypeError, "argument must be a string");
    return NULL;
  }

  ChannelPtr ptr = Channel::find(PyString_AsString(name));

  if(!ptr)
    Py_RETURN_NONE;

  return wrap(&ptr);
}

PyObject *ChannelWrap::del(PyObject *self, ChannelWrap *channel)
{
  if(!ChannelWrap::check(channel))
  {
    PyErr_SetString(PyExc_TypeError, "argument must be a Channel object");
    return NULL;
  }

  Channel::del(channel->get_wrapped());

  Py_RETURN_NONE;
}

PyObject *ChannelWrap::send_names(ChannelWrap *self, ClientWrap *client)
{
  if(!ClientWrap::check(client) || !BaseClient::is_client(client->get_wrapped()))
  {
    PyErr_SetString(PyExc_TypeError, "argument must be a Client type");
    return NULL;
  }
  
  self->get_wrapped()->send_names(client->get_wrapped());

  Py_RETURN_NONE;
}

PyObject *ChannelWrap::send(ChannelWrap *self, PyObject *args, PyObject *kwargs)
{
  ClientWrap *client;
  PyObject *result = PythonUtil::send_format(self, args, kwargs);

  client = reinterpret_cast<ClientWrap *>(PyDict_GetItemString(kwargs, "client"));

  if(!ClientWrap::check(client) || !BaseClient::is_client(client->get_wrapped()))
  {
    PyErr_SetString(PyExc_TypeError, "client argument must be a Client type");
    return NULL;
  }

  if(result == NULL)
    return NULL;

  self->get_wrapped()->send(client->get_wrapped(), PyString_AsString(result));

  Py_DECREF(result);

  Py_RETURN_NONE;
}

PyObject *ChannelWrap::set_mode_char(ChannelWrap *self, PyObject *args)
{
  char c;
  PyObject *set;

  PyArg_ParseTuple(args, "cO", &c, &set);

  self->get_wrapped()->set_mode_char(c, PyObject_IsTrue(set) != 0);

  Py_RETURN_NONE;
}

PyObject *ChannelWrap::supported_modes(ChannelWrap *self, PyObject *arg)
{
  string modes = Channel::supported_modes();
  return PyString_FromString(modes.c_str());
}

PyObject *ChannelWrap::is_member(ChannelWrap *self, ClientWrap *client)
{
  if(!ClientWrap::check(client) || !BaseClient::is_client(client->get_wrapped()))
  {
    PyErr_SetString(PyExc_TypeError, "argument must be a Client type");
    return NULL;
  }

  if(self->get_wrapped()->is_member(client->get_wrapped()))
    Py_RETURN_TRUE;
  else
    Py_RETURN_FALSE;
}

PyObject *ChannelWrap::remove_member(ChannelWrap *self, ClientWrap *client)
{
  if(!ClientWrap::check(client) || !BaseClient::is_client(client->get_wrapped()))
  {
    PyErr_SetString(PyExc_TypeError, "Argument must be a Client type");
    return NULL;
  }

  self->get_wrapped()->remove_member(client->get_wrapped());

  Py_RETURN_NONE;
}

PyObject *ChannelWrap::str(ChannelWrap *self)
{
  return PyString_FromString(self->get_wrapped()->str().c_str());
}

int ChannelWrap::compare(ChannelWrap *self, ChannelWrap *other)
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

PyObject *ChannelWrap::fire_joining(EventWrap *, PyObject *args)
{
  ChannelWrap *channel;
  ClientWrap *client;

  if(!PyArg_ParseTuple(args, "OO", &channel, &client))
    return NULL;

  if(Channel::joining(channel->get_wrapped(), client->get_wrapped()))
    Py_RETURN_TRUE;
  else
    Py_RETURN_FALSE;
}

PyObject *ChannelWrap::fire_joined(EventWrap *, PyObject *args)
{
  ChannelWrap *channel;
  ClientWrap *client;

  if(!PyArg_ParseTuple(args, "OO", &channel, &client))
    return NULL;

  if(Channel::joined(channel->get_wrapped(), client->get_wrapped()))
    Py_RETURN_TRUE;
  else
    Py_RETURN_FALSE;
}

bool ChannelWrap::on_joining(ChannelPtr channel, ClientPtr client)
{
  PyObject *args, *chan_ptr, *client_ptr;
  bool ret;

  chan_ptr = wrap(&channel);
  client_ptr = ClientWrap::wrap(&client);

  args = Py_BuildValue("(OO)", chan_ptr, client_ptr);

  ret = handle_event(joining, args);

  Py_DECREF(args);
  Py_DECREF(chan_ptr);
  Py_DECREF(client_ptr);

  return ret;
}

bool ChannelWrap::on_joined(ChannelPtr channel, ClientPtr client)
{
  PyObject *args, *chan_ptr, *client_ptr;
  bool ret;

  chan_ptr = wrap(&channel);
  client_ptr = ClientWrap::wrap(&client);

  args = Py_BuildValue("(OO)", chan_ptr, client_ptr);

  ret = handle_event(joined, args);

  Py_DECREF(args);
  Py_DECREF(chan_ptr);
  Py_DECREF(client_ptr);

  return ret;
}
#endif
