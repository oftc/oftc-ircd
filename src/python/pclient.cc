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

#include "python/pclient.h"
#include "python/pint.h"
#include "stdinc.h"
#include "python/pevent.h"
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

PClient::PClient(ClientPtr ptr) : PCType(ptr)
{
}

PClient::PClient(PTuple args, PDict kwargs)
{
}

PClient::~PClient()
{
}

PObject PClient::close(PTuple args)
{
  PString reason = static_cast<PString>(args[0]);
  inner->close(reason);

  Py_RETURN_NONE;
}

PObject PClient::numeric(PTuple args)
{
  PObject item;
  PInt num;
  string format;
  stringstream output;
  int index = 1;
  shared_ptr<Client> ptr;

  if(!Client::is_client(inner))
  {
    PyErr_SetString(PyExc_RuntimeError, "Cannot send a numeric to a non-client");
    return static_cast<PyObject *>(NULL);
  }

  ptr = dynamic_pointer_cast<Client>(inner);

  num = args[0];
  format = Numeric::raw_format(num);
  for(auto i = format.begin(); i != format.end(); i++)
  {
    char c = *i;

    if(c == '%')
    {
      i++;
      item = args[index++];
      if(!item)
      {
        PyErr_SetString(PyExc_TypeError, "Not enough arguments for numeric format");
        return static_cast<PyObject *>(NULL);
      }
      switch(*i)
      {
      case 's':
      case 'c':
     /*   if(!PyString_Check(item))
        {
          stringstream ss;
          ss << "expected string argument for argument " << index;
          PyErr_SetString(PyExc_TypeError, ss.str().c_str());
          return NULL;
        }*/
        if(*i == 's')
          output << PyString_AsString(item);
        else
          output << PyString_AsString(item)[0];
        break;
      case 'd':
        /*if(!PyInt_Check(item))
        {
          stringstream ss;
          ss << "expected int argument for argument " << index;
          PyErr_SetString(PyExc_TypeError, ss.str().c_str());
          return NULL;
        }*/

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

  Py_RETURN_NONE;
}

PObject PClient::send(PTuple args, PDict kwargs)
{
  PString str = PythonUtil::send_format(this, args, kwargs);

  if(!str)
    return static_cast<PyObject *>(NULL);

  inner->send(str);

  Py_RETURN_NONE;
}

PObject PClient::send_channels_common(PTuple args, PDict kwargs)
{
  PString str = PythonUtil::send_format(this, args, kwargs);

  if(!str)
    return static_cast<PyObject *>(NULL);

  shared_ptr<Client> ptr = dynamic_pointer_cast<Client>(inner);
  ptr->send_channels_common(str);

  Py_RETURN_NONE;
}

PObject PClient::remove_channel(PTuple args)
{
  PyObject *arg = args[0];
  PChannel *channel = static_cast<PChannel *>(arg);

  shared_ptr<Client> ptr = dynamic_pointer_cast<Client>(inner);

  if(!PChannel::check(channel))
  {
    PyErr_SetString(PyExc_TypeError, "argument must be a Channel");
    return static_cast<PyObject *>(NULL);
  }

  ptr->remove_channel(*channel);

  Py_RETURN_NONE;
}

PObject PClient::get(Property prop)
{
  shared_ptr<Client> client = dynamic_pointer_cast<Client>(inner);
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
    return client->is_invisible();
  case Idletime:
    return PInt(client->get_idletime());
  case ServerProp:
    return new PClient(client->get_server());
  case Info:
    return PString(server->get_info());
  }

  Py_RETURN_NONE;
}

int PClient::set(Property prop, PObject value)
{
  shared_ptr<Client> client = dynamic_pointer_cast<Client>(inner);

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
    client->set_invisible(value);
    break;
  case LastMessage:
    client->set_last_message(value);
    break;
  }

  return 0;
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

  PEvent::add_event(type.tp_dict, "closing", Client::closing, &PEvent::client_string_callback);
  PEvent::add_event(type.tp_dict, "nick_changing", Client::nick_changing, &PEvent::client_ircstring_callback);
  PEvent::add_event(type.tp_dict, "nick_changed", Client::nick_changed, &PEvent::client_string_callback);

  PyDict_SetItemString(type.tp_dict, "Me", me);
}

PyObject *PClient::find_by_name(PyObject *self, PyObject *varargs)
{
  PTuple args(varargs);
  PString name = static_cast<PString>(args[0]);
  ClientPtr ptr = Client::find_by_name(name.c_str());

  if(ptr)
    return new PClient(ptr);
  else
    Py_RETURN_NONE;
}

PyObject *PClient::del_name(PyObject *self, PyObject *varargs)
{
  PTuple args(varargs);
  PyObject *arg = args[0];
  PClient *client = static_cast<PClient *>(arg);
  
  Client::del_name(*client);

  Py_RETURN_NONE;
}

PyObject *PClient::add_name(PyObject *self, PyObject *varargs)
{
  PTuple args(varargs);
  PyObject *arg = args[0];
  PClient *client = static_cast<PClient *>(arg);

  Client::add_name(*client);
  Py_RETURN_NONE;
}

PyObject *PClient::add(PyObject *self, PyObject *varargs)
{
  PTuple args(varargs);
  PyObject *arg = args[0];
  PClient *client = static_cast<PClient *>(arg);

  Client::add(*client);
  Py_RETURN_NONE;
}
