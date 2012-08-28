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

#include "python/pchannel.h"
#include "python/pevent.h"
#include "stdinc.h"
#include <functional>

using std::bind;
using namespace std::placeholders;

enum ChannelProps
{
  Name,
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
  Excepts
};

PChannel::PChannel()
{
}

PChannel::PChannel(ChannelPtr ptr) : PCType(ptr)
{
}

PChannel::PChannel(PTuple args, PDict kwargs) : PCType(args, kwargs)
{
  inner = ChannelPtr(new Channel());
}

PChannel::~PChannel()
{
}

PObject PChannel::add_member(PTuple args)
{
  PyObject *arg = args[0];

  PClient *client = static_cast<PClient *>(arg);

  if(!PClient::check(client) || !BaseClient::is_client(*client))
  {
    PyErr_SetString(PyExc_TypeError, "Argument must be a Client type");
    return NULL;
  }

  inner->add_member(*client);

  return PObject::None();
}

PObject PChannel::is_member(PTuple args)
{
  PyObject *arg = args[0];
  PBool ret;

  PClient *client = static_cast<PClient *>(arg);
  
  if(!PClient::check(client) || !BaseClient::is_client(*client))
  {
    PyErr_SetString(PyExc_TypeError, "argument must be a Client type");
    return NULL;
  }

  ret = PBool(inner->is_member(*client));

  return ret;
}

PObject PChannel::remove_member(PTuple args)
{
  PyObject *arg = args[0];

  PClient *client = static_cast<PClient *>(arg);
  
  if(!PClient::check(client) || !BaseClient::is_client(*client))
  {
    PyErr_SetString(PyExc_TypeError, "argument must be a Client type");
    return NULL;
  }

  inner->remove_member(*client);

  return PObject::None();
}

PObject PChannel::send_names(PTuple args)
{
  PyObject *arg = args[0];

  PClient *client = static_cast<PClient *>(arg);
  
  if(!PClient::check(client) || !BaseClient::is_client(*client))
  {
    PyErr_SetString(PyExc_TypeError, "argument must be a Client type");
    return NULL;
  }
  
  inner->send_names(*client);

  return PObject::None();
}

PObject PChannel::send(PTuple args, PDict kwargs)
{
  PClient *client;
  PString result = PythonUtil::send_format(this, args, kwargs);
  PyObject *obj;

  obj = kwargs["client"];

  client = static_cast<PClient *>(obj);

  if(!PClient::check(client) || !BaseClient::is_client(*client))
  {
    PyErr_SetString(PyExc_TypeError, "client argument must be a Client type");
    return NULL;
  }

  if(!result)
    return NULL;

  inner->send(*client, result);

  return PObject::None();
}

PObject PChannel::set_mode_char(PTuple args)
{
  char c;
  PObject set, ch;

  ch = args[0];
  c = ch.str().c_str()[0];
  set = args[1];

  inner->set_mode_char(c, set.is_true());

  return PObject::None();
}

PString PChannel::str()
{
  return inner->str();
}

PObject PChannel::get(Property prop)
{ 
  switch(prop.number)
  {
  case Name:
    return PString(inner->get_name());
  case InviteOnly:
    return PBool(inner->is_invite_only()).incref();
  case Moderated:
    return PBool(inner->is_moderated()).incref();
  case NoExternal:
    return PBool(inner->is_no_external_msgs()).incref();
  case Private:
    return PBool(inner->is_private()).incref();
  case Secret:
    return PBool(inner->is_secret()).incref();
  case TopicOpsOnly:
    return PBool(inner->is_topic_op_only()).incref();
  case Secure:
    return PBool(inner->is_secure()).incref();
  case Members:
    return new PChannelMemberList(inner->get_members());
  case Bans:
    return new PNuhMaskList(inner->get_bans());
  }

  return PObject::None();
}

int PChannel::set(Property prop, PObject value)
{

  switch(prop.number)
  {
  case Name:
    inner->set_name(value.str().c_str());
    break;
  }

  return 0;
}

// Statics

void PChannel::init(const PObject& module)
{
  PyTypeObject& type = type_object();

  type.tp_name = "Channel";

  add_method("add_member", "add a client to the channel", VarArgsMethod(&PChannel::add_member));
  add_method("is_member", "test if the client is on this channel", VarArgsMethod(&PChannel::is_member));
  add_method("remove_member", "remove a client from the channel", VarArgsMethod(&PChannel::remove_member));
  add_method("send_names", "send a names reply about this channel", VarArgsMethod(&PChannel::send_names));
  add_method("send", "send to all clients on this channel", KeywordArgsMethod(&PChannel::send));
  add_method("set_mode_char", "set a non args mode on this channel", VarArgsMethod(&PChannel::set_mode_char));

  add_method("add", "add a channel", &PChannel::add);
  add_method("del", "remove a channel", &PChannel::del);
  add_method("find", "find a channel", &PChannel::find);
  add_method("supported_modes", "find out what channel modes we support", &PChannel::supported_modes);

  add_property("Name", "channel name", Name, StringArg);
  add_property("InviteOnly", "channel members", InviteOnly, BoolArg);
  add_property("Moderated", "if the channel is moderated", Moderated, BoolArg);
  add_property("NoExternal", "if the channel disallowed external messages", NoExternal, BoolArg);
  add_property("Private", "if the channel is private", Private, BoolArg);
  add_property("Secret", "if the channel is secret", Secret, BoolArg);
  add_property("TopicOpsOnly", "if the channel topic can be set by ops only", TopicOpsOnly, BoolArg);
  add_property("Secure", "if the channel is secure", Secure, BoolArg);
  add_property("Bans", "channel ban list", Bans, NoFlags);
  add_property("Invexes", "channel invex list", Invexes, NoFlags);
  add_property("Quiets", "channel quiet list", Quiets, NoFlags);
  add_property("Excepts", "channel except list", Excepts, NoFlags);
  add_property("Members", "channel member list", Members, NoFlags);

  PCType::init(module);

  PEvent::add_event(type.tp_dict, "joining", Channel::joining, &PEvent::channel_client_callback);
  PEvent::add_event(type.tp_dict, "joined", Channel::joined, &PEvent::channel_client_callback);
}

PyObject *PChannel::add(PyObject *self, PyObject *vargs)
{
  PTuple args(vargs);
  PyObject *obj = args[0];

  PChannel *channel = static_cast<PChannel *>(obj);

  if(!PChannel::check(channel))
  {
    PyErr_SetString(PyExc_TypeError, "argument must be a Channel object");
    return NULL;
  }

  Channel::add(*channel);

  return PObject::None();
}

PyObject *PChannel::del(PyObject *self, PyObject *vargs)
{
  PTuple args(vargs);
  PyObject *obj = args[0];

  PChannel *channel = static_cast<PChannel *>(obj);

  if(!PChannel::check(channel))
  {
    PyErr_SetString(PyExc_TypeError, "argument must be a Channel object");
    return NULL;
  }

  Channel::del(*channel);

  return PObject::None();
}

PyObject *PChannel::find(PyObject *self, PyObject *vargs)
{
  PTuple args(vargs);
  PString name = static_cast<PString>(args[0]);
  ChannelPtr channel;

  channel = Channel::find(name.c_str());

  if(!channel)
    return PObject::None();

  return new PChannel(channel);
}

PyObject *PChannel::supported_modes(PyObject *self, PyObject *args)
{
  PString modes(Channel::supported_modes());

  return modes;
}
