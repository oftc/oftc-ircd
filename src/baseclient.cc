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
#include "baseclient.h"
#include "connection.h"
#include "server.h"
#include "system.h"

unordered_map<irc_string, ClientPtr> BaseClient::names;
Event<ClientPtr, string> BaseClient::closing;

BaseClient::BaseClient() : level(Unregistered), last_data(0)
{
  Logging::trace << "Created Client: " << this << Logging::endl;
}

BaseClient::~BaseClient()
{
  Logging::trace << "Destroyed Client: " << this << Logging::endl;
}

void BaseClient::close(string message)
{
  if(connection->is_closing())
    return;

  stringstream ss;

  BaseClient::closing(names[name], message);

  ss << "ERROR :Closing link: " << host << " (" << message << ")";

  send(ss.str());
  connection->close();
}

void BaseClient::send(string message)
{
  if (message.length() >= 510)
    message.resize(510);

  message.append("\r\n");

  connection->send(message);
}

irc_string BaseClient::str() const
{
  if(name.empty())
    return "*";
  else
    return name;
}

bool BaseClient::is_registered() const
{
  return level >= Registered;
}

void BaseClient::clear_connection()
{
  connection.reset();
}

irc_string BaseClient::get_name() const
{
  return name;
}

string BaseClient::get_host() const
{
  return host;
}

void BaseClient::set_name(const irc_string _name)
{
  name = _name;
  if(name.length() > System::get_nicklen())
    name.resize(System::get_nicklen());
}

void BaseClient::set_host(const string _host)
{
  host = _host;
}

bool BaseClient::check_timeout()
{
  time_t curr_time = time(NULL);

  if(!is_registered())
  {
    if(curr_time - first_seen > 30)
      return false;
  }
  else
  {
    if(ping_sent != 0 && curr_time - ping_sent > 30)
    {
      ping_sent = 0;
      return false;
    }

    if(ping_sent == 0 && curr_time - last_data > 15)
    {
      stringstream ss;

      ss << "PING :" << Server::get_me()->str();
      send(ss.str());
      ping_sent = curr_time;
    }
  }
  return true;
}

void BaseClient::set_registered()
{
  level = Registered;
}

void BaseClient::set_connection(const ConnectionPtr conn)
{
  connection = conn;
  host = connection->get_host();
}

void BaseClient::set_last_data(time_t when)
{
  last_data = when;
  ping_sent = 0;
}

void BaseClient::set_first_seen(time_t when)
{
  first_seen = when;
}

//  Statics

void BaseClient::init()
{
  ClientPtr me = ClientPtr(new Server);
  shared_ptr<Server> me_ptr = dynamic_pointer_cast<Server>(me);

  me->set_name(System::get_server_name());
  me_ptr->set_info(System::get_server_info());

  Server::set_me(me);
}

void BaseClient::add_name(const ClientPtr client)
{
  names[client->name] = client;
}

ClientPtr BaseClient::find_by_name(const irc_string name)
{
  return names[name];
}

void BaseClient::del_name(const ClientPtr client)
{
  if(!client->name.empty())
    names.erase(client->name);
}

bool BaseClient::is_client(const ClientPtr client)
{
  return typeid(*client) == typeid(Client);
}

bool BaseClient::is_server(const ClientPtr client)
{
  return typeid(*client) == typeid(Server);
}
