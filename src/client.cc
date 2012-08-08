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
#include <stdarg.h>
#include <iomanip>
#include "system.h"
#include "client.h"
#include "connection.h"
#include "numeric.h"
#include "server.h"

using std::string;
using std::setw;
using std::setfill;

Event<ClientPtr> Client::connected;
Event<ClientPtr> Client::registering;
Event<ClientPtr> Client::disconnected;
list<ClientPtr> Client::unregistered_list;
list<ClientPtr> Client::client_list;
uv_timer_t Client::ping_timer;

Client::Client() : invisible(false), last_message(time(NULL))
{
}

void Client::send(string arg, int numeric)
{
  stringstream buffer;

  buffer << ":" << Server::get_me()->str() << " ";
  buffer << setw(3) << setfill('0') << numeric;
  buffer << " ";
  if(name.empty())
    buffer << "*";
  else
    buffer << name;

  buffer << " " << arg;

  BaseClient::send(buffer.str());
}

void Client::send(int numeric, ...)
{
  va_list args;

  va_start(args, numeric);

  send(Numeric::format(numeric, args), numeric);
  
  va_end(args);
}

irc_string Client::str() const
{
  stringstream buff;

  if(name.empty())
    return "*";

  buff << name << "!" << username << "@" << host;

  return irc_string(buff.str().c_str());
}

bool Client::is_invisible() const
{
  return invisible;
}

string Client::get_username() const
{
  return username;
}

string Client::get_realname() const
{
  return realname;
}

time_t Client::get_idletime() const
{
  return time(NULL) - last_message;
}

void Client::set_invisible(bool invis)
{
  invisible = invis;
}

void Client::set_username(const string user)
{
  username = user;
}

void Client::set_realname(const string real)
{
  realname = real;
}

void Client::set_last_message(time_t when)
{
  last_message = when;
}

// Statics

void Client::init()
{
  uv_timer_init(uv_default_loop(), &ping_timer);
  uv_timer_start(&ping_timer, check_pings, 0, 5);
}

void Client::add(ClientPtr ptr)
{
  shared_ptr<Client> client = dynamic_pointer_cast<Client>(ptr);

  registering(client);

  client_list.push_back(client);
  client->set_registered();

  unregistered_list.remove(client);

  connected(client);

  client->send(001, client->str().c_str());
  client->send(002, Server::get_me()->str().c_str(), "0.0.1");
  client->send(003, System::get_built_date());
}

void Client::add_unregistered(ClientPtr client)
{
  unregistered_list.push_back(client);
}

void Client::remove(ClientPtr client)
{
  if(client->is_registered())
    client_list.remove(client);
  else
    unregistered_list.remove(client);
}

void Client::check_pings(uv_timer_t *handle, int status)
{
  list<ClientPtr>::const_iterator it;

  for(it = client_list.begin(); it != client_list.end(); it++)
  {
    ClientPtr client = *it;

    if(!client->check_timeout())
    {
      client->close("Ping Timeout");
    }
  }

  for(it = unregistered_list.begin(); it != unregistered_list.end(); it++)
  {
    ClientPtr client = *it;

    if(!client->check_timeout())
    {
      client->close("Registration timed out");
    }
  }
}
