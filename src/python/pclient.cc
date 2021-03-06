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

#include "stdinc.h"
#include "python/pclient.h"
#include "python/pint.h"
#include "python/pevent.h"
#include "python/pcmap.h"
#include "python/pmembership.h"
#include "server.h"
#include "numeric.h"
#include <functional>

using std::bind;
using namespace std::placeholders;

enum ClientProperty
{
  Name,
  Username,
  Realname,
  Host,
  Info,
  Invisible,
  Idletime,
  LastMessage,
  Channels,
  ServerProp,
};

PClient *PClient::me;

PClient::PClient()
{
}

PClient::PClient(BaseClientPtr ptr) : PCType(ptr)
{
  Logging::trace << "Created PClient: " << this << Logging::endl;
}

PClient::PClient(const PTuple& args, const PDict& kwargs)
{
}

PClient::~PClient()
{
  Logging::trace << "Destroyed PClient: " << this << Logging::endl;
}

int PClient::compare(const PObject& right)
{
  if(!PClient::check(right))
    return PCType::compare(right);

  PClient *rptr = right.AsPtr<PClient>();

  if(inner == rptr->inner)
    return 0;

  if(inner->get_name() > rptr->inner->get_name())
    return 1;
  else
    return -1;
}

PObject PClient::close(const PTuple& args)
{
  PString reason = args[0].As<PString>();
  inner->close(reason);

  return PObject::None();
}

PObject PClient::numeric(const PTuple& args)
{
  PObject item;
  string format;
  stringstream output;
  int index = 1;
  ClientPtr ptr;

  if(!Client::is_client(inner))
    return PException(PyExc_RuntimeError, "Cannot send a numeric to a non-client");

  ptr = dynamic_pointer_cast<Client>(inner);

  PInt num(args[0]);

  format = Numeric::raw_format(num);
  for(auto i = format.begin(); i != format.end(); i++)
  {
    char c = *i;

    if(c == '%')
    {
      i++;
      item = args[index++];
      if(!item)
        return PException(PyExc_TypeError, "Not enough arguments for numeric format");

      switch(*i)
      {
      case 's':
      case 'c':
        if(!PString::check(item))
        {
          stringstream ss;
          ss << "expected string argument for argument " << index;
          return PException(PyExc_TypeError, ss.str().c_str());
        }
        if(*i == 's')
          output << item.str().c_str();
        else
          output << item.str().c_str()[0];
        break;
      case 'd':
        if(!PInt::check(item))
        {
          stringstream ss;
          ss << "expected int argument for argument " << index << " " << item.str().c_str();
          return PException(PyExc_TypeError, ss.str().c_str());
        }

        output << item.str().c_str();
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

  ptr->send(output.str(), num);

  return PObject::None();
}

PObject PClient::send(const PTuple& args, const PDict& kwargs)
{
  PObject str = PythonUtil::send_format(this, args, kwargs);

  if(!str)
    return NULL;

  inner->send(str.As<PString>());

  return PObject::None();
}

PObject PClient::send_channels_common(const PTuple& args, const PDict& kwargs)
{
  PObject str = PythonUtil::send_format(this, args, kwargs);

  if(!str)
    return NULL;

  ClientPtr ptr = dynamic_pointer_cast<Client>(inner);
  ptr->send_channels_common(str.As<PString>());

  return PObject::None();
}

PObject PClient::remove_channel(const PTuple& args)
{
  PChannel *channel = args[0].AsPtr<PChannel>();

  ClientPtr ptr = dynamic_pointer_cast<Client>(inner);

  if(!PChannel::check(channel))
    return PException(PyExc_TypeError, "argument must be a Channel");

  ptr->remove_channel(*channel);

  return PObject::None();
}

PObject PClient::get(Property prop)
{
  ClientPtr client = dynamic_pointer_cast<Client>(inner);
  ServerPtr server = dynamic_pointer_cast<Server>(inner);

  switch(prop.number)
  {
  case Name:
    return PString(inner->get_name());
  case Username:
    return PString(client->get_username());
  case Realname:
    return PString(client->get_realname());
  case Host:
    return PString(inner->get_host());
  case Invisible:
    return PBool(client->is_invisible());
  case Idletime:
    return PInt(static_cast<int>(client->get_idletime()));
  case ServerProp:
    return new PClient(client->get_server());
  case Info:
    return PString(server->get_info());
  case Channels:
    return new PClientMemberList(client->get_channels());
  }

  return PObject::None();
}

int PClient::set(const Property prop, const PObject& value)
{
  ClientPtr client = dynamic_pointer_cast<Client>(inner);

  int ret;

  ret = PCType::set(prop, value);

  if(ret != 0)
    return -1;

  switch(prop.number)
  {
  case Name:
    inner->set_name(value.str().c_str());
    break;
  case Username:
    client->set_username(value.str());
    break;
  case Realname:
    client->set_realname(value.str());
    break;
  case Host:
    inner->set_host(value.str());
    break;
  case Invisible:
    client->set_invisible(value.is_true());
    break;
  case LastMessage:
    client->set_last_message(static_cast<int>(PInt(value)));
    break;
  }

  return ret;
}

// Statics

void PClient::init(const PObject& module)
{
  PyTypeObject& type = type_object();

  add_method("is_registered", "Check if the client is registered or not", NoArgsMethod(&PClient::is_registered));
  add_method("is_ssl", "Check if the client is connected via ssl", NoArgsMethod(&PClient::is_ssl));
  add_method("close", "Close the connection to the client", VarArgsMethod(&PClient::close));
  add_method("numeric", "Send the client a numeric respose", VarArgsMethod(&PClient::numeric));
  add_method("send", "Send the client a message", KeywordArgsMethod(&PClient::send));
  add_method("send_channels_common", "Send a message to clients in the same channels as this client", KeywordArgsMethod(&PClient::send_channels_common));
  add_method("remove_channel", "Remove the client from a channel", VarArgsMethod(&PClient::remove_channel));
  add_method("find_by_name", "Search the client list for a named client", &find_by_name);
  add_method("del_name", "Delete a name from the names list", &del_name);
  add_method("add_name", "Add a name to the names list", &add_name);
  add_method("add", "Add a client to the registered client list", &add);

  add_property("Name", "Client name", Name, StringArg);
  add_property("Username", "Client username", Username, StringArg);
  add_property("Realname", "Client realname", Realname, StringArg);
  add_property("Host", "hostname", Host, StringArg);
  add_property("Info", "Server information", Info, StringArg);
  add_property("Invisible", "invisible (umode +i)", Invisible, BoolArg);
  add_property("Idletime", "how long the client has been idle", Idletime, IntArg);
  add_property("LastMessage", "when the client last sent a privmsg/notice", LastMessage, IntArg);
  add_property("Server", "server client belongs to", ServerProp, ServerArg);
  add_property("Channels", "Channels client is on", Channels, NoFlags);

  type.tp_name = "Client";

  PCType::init(module);

  me = new PClient(Server::get_me());

  PEvent::add_event<BaseClientPtr, string>(type.tp_dict, "closing", Client::closing, &PEvent::client_string_callback, on_closing);
  PEvent::add_event<BaseClientPtr, irc_string>(type.tp_dict, "nick_changing", Client::nick_changing, &PEvent::client_ircstring_callback, on_nick_changing);
  PEvent::add_event<BaseClientPtr, string>(type.tp_dict, "nick_changed", Client::nick_changed, &PEvent::client_string_callback, on_nick_changed);

  PyDict_SetItemString(type.tp_dict, "Me", me);
}

bool PClient::on_closing(BaseClientPtr client, string reason)
{
  PTuple args(2);

  args[0] = new PClient(client);
  args[1] = PString(reason);

  return PEvent::handle(PyDict_GetItemString(type_object().tp_dict, "closing"), args);
}

bool PClient::on_nick_changing(BaseClientPtr client, irc_string new_nick)
{
  PTuple args(2);

  args[0] = new PClient(client);
  args[1] = PString(new_nick);

  return PEvent::handle(PyDict_GetItemString(type_object().tp_dict, "nick_changing"), args);
}

bool PClient::on_nick_changed(BaseClientPtr client, string old_mask)
{
  PTuple args(2);

  args[0] = new PClient(client);
  args[1] = PString(old_mask);

  return PEvent::handle(PyDict_GetItemString(type_object().tp_dict, "nick_changed"), args);
}

PyObject *PClient::find_by_name(PyObject *self, PyObject *varargs)
{
  PTuple args(varargs);
  PString name = args[0].As<PString>();
  BaseClientPtr ptr = Client::find_by_name(name.c_str());

  if(ptr)
    return new PClient(ptr);
  else
    return PObject::None();
}

PyObject *PClient::del_name(PyObject *self, PyObject *varargs)
{
  PTuple args(varargs);
  PClient *client = args[0].AsPtr<PClient>();
  
  Client::del_name(*client);

  return PObject::None();
}

PyObject *PClient::add_name(PyObject *self, PyObject *varargs)
{
  PTuple args(varargs);
  PClient *client = args[0].AsPtr<PClient>();

  Client::add_name(*client);
  return PObject::None();
}

PyObject *PClient::add(PyObject *self, PyObject *varargs)
{
  PTuple args(varargs);
  PClient *client = args[0].AsPtr<PClient>();

  Client::add(*client);
  return PObject::None();
}
