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

unordered_map<string, ClientPtr> BaseClient::names;

BaseClient::BaseClient() : level(Unregistered)
{
  Logging::debug << "Created Client: " << this << Logging::endl;
}

BaseClient::~BaseClient()
{
  Logging::debug << "Destroyed Client: " << this << Logging::endl;
}

void
BaseClient::send(string message)
{
  if (message.length() >= 510)
    message.resize(510);

  message.append("\r\n");

  connection->send(message);
}

string
BaseClient::str()
{
  if(name.empty())
    return "*";
  else
    return name;
}

bool 
BaseClient::is_registered() const
{
  return level >= Registered;
}

void
BaseClient::clear_connection()
{
  connection.reset();
}

string
BaseClient::get_name() const
{
  return name;
}

void
BaseClient::set_name(const string _name)
{
  name = _name;
}

void
BaseClient::set_resgistered()
{
  level = Registered;
}

void
BaseClient::set_connection(const ConnectionPtr conn)
{
  connection = conn;
  host = connection->get_host();
}

//  Statics

void 
BaseClient::add_name(const ClientPtr client)
{
  names[client->name] = client;
}

ClientPtr
BaseClient::find_by_name(const string name)
{
  return names[name];
}

void
BaseClient::del_name(const ClientPtr client)
{
  names.erase(client->name);
}

bool 
BaseClient::is_client(const ClientPtr client)
{
  return typeid(*client) == typeid(Client);
}

bool 
BaseClient::is_server(const ClientPtr client)
{
  return typeid(*client) == typeid(Server);
}
