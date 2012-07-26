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

list<ClientPtr> BaseClient::client_list;
unordered_map<string, ClientPtr> BaseClient::names;

BaseClient::BaseClient() : level(Unregistered)
{
  Logging::debug << "Client created: " << this << Logging::endl;
}

BaseClient::~BaseClient()
{
  Logging::debug << "Client destroyed:" << this << Logging::endl;
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

void
BaseClient::set_connection(ConnectionPtr conn)
{
  connection = conn;
  host = connection->get_host();
}
