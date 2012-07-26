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
#include "client.h"
#include "connection.h"
#include "numeric.h"
#include "system.h"
#include "server.h"

using std::string;
using std::setw;
using std::setfill;

Event<ClientPtr> Client::connected;
Event<ClientPtr> Client::registered;
Event<ClientPtr> Client::disconnected;
list<ClientPtr> Client::client_list;

void
Client::send(string arg, int numeric)
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

void
Client::send(int numeric, ...)
{
  va_list args;

  va_start(args, numeric);

  send(Numeric::format(numeric, args), numeric);
  
  va_end(args);
}

string
Client::str()
{
  stringstream buff;

  if(name.empty())
    return "*";

  buff << name << "!" << username << "@" << host;

  return buff.str();
}

// Statics

void
Client::add(ClientPtr ptr)
{
  shared_ptr<Client> client = dynamic_pointer_cast<Client>(ptr);

  client_list.push_back(client);
  client->set_resgistered();

  registered(client);

  client->send(001, client->str().c_str());
  client->send(002, Server::get_me()->str().c_str(), "0.0.1");
  client->send(003, System::get_built_date());
}

void 
Client::remove(ClientPtr client)
{
  client_list.remove(client);
}
