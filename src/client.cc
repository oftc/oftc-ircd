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
#include "client.h"
#include "connection.h"
#include "numeric.h"
#include "system.h"

using std::make_shared;
using std::string;

ClientPtr Client::me;
vector<ClientPtr> Client::client_list;

Client::Client() : level(Unregistered)
{
}

Client::Client(Connection *connection) : connection(connection), level(Unregistered)
{
}

void
Client::send(string message)
{
  if (message.length() >= 510)
    message.resize(510);

  message.append("\r\n");

  connection->send(message);
}

void
Client::send(int numeric, string arg)
{
  stringstream buffer;
  buffer << ":" << me->get_name() << " " << numeric << " ";
  if(name.empty())
    buffer << "*";
  else
    buffer << name;

  buffer << " " << arg;

  send(buffer.str());
}

void
Client::send(int numeric, ...)
{
  va_list args;

  va_start(args, numeric);

  send(numeric, Numeric::format(numeric, args));
  
  va_end(args);
}

string
Client::str()
{
  stringstream buff;

  buff << name << "!" << username << "@" << "localhost";

  return buff.str();
}

// Statics
void
Client::init()
{
  me = make_shared<Client>();
  me->set_name(System::get_server_name());
  client_list.push_back(me);
}

void
Client::add(ClientPtr client)
{
  client_list.push_back(client);
  client->level = Registered;
  
  // fire new client event
}
