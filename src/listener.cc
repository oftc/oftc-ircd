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
#include <sys/types.h>
#ifndef _WIN32
#include <sys/socket.h>
#endif
#include <errno.h>
#include "config.h"
#include "sslconnection.h"
#include "system.h"
#include "listener.h"
#include "listenersection.h"
#include "connection.h"
#include "ssl.h"
#include "client.h"
#include "server.h"

ListenerSection Listener::config;
ListenerList Listener::listeners;

Listener::Listener() : host(""), port(6667), flags(static_cast<ListenerFlag>(0))
{
  Logging::trace << "Created Listener: " << this << Logging::endl;
}

Listener::Listener(string host, int port, ListenerFlag flags) : 
  host(host), port(port), flags(flags)
{
  Logging::trace << "Created Listener: " << this << Logging::endl;
}

Listener::~Listener()
{
  Logging::trace << "Destroyed Listener: " << this << Logging::endl;
}

void Listener::set_flag(ListenerFlag flag)
{
  int int_flags = static_cast<int>(flags);

  int_flags |= static_cast<int>(flag); 
  flags = static_cast<ListenerFlag>(int_flags);
}

bool Listener::is_ssl() const
{ 
  return flags & Listener_SSL; 
}

void Listener::set_ssl()
{
  set_flag(Listener_SSL);
}

void Listener::connected(uv_stream_t *stream, int status)
{
  ConnectionPtr connection;

  if(Ssl::is_enabled() && is_ssl())
    connection = ConnectionPtr(new SSLConnection);
  else
    connection = ConnectionPtr(new Connection);

  Connection::add(connection);
  BaseClientPtr client = BaseClientPtr(new Client());
  connection->set_client(client);
  client->set_connection(connection);

  client->set_first_seen(time(NULL));
  
  ClientPtr ptr = dynamic_pointer_cast<Client>(client);
  ptr->set_server(Server::get_me());

  Client::add_unregistered(client);

  connection->accept(stream);
  Connection::ip_connecting(connection);

  Client::connected(client);
}

void Listener::start()
{
  int ret;

  ret = uv_tcp_init(uv_default_loop(), &listener);
  listener.data = this;

  if(ret < 0)
    throw runtime_error(System::uv_perror("Failed to start listener"));

  if(host.find('.') != string::npos)
  {
    struct sockaddr_in addr = uv_ip4_addr(host.c_str(), port);

    ret = uv_tcp_bind(&listener, addr);
  }
  else if(host.find(':') != string::npos)
  {
    struct sockaddr_in6 addr = uv_ip6_addr(host.c_str(), port);
     
    uv_tcp_v6only(&listener, 1);
    ret = uv_tcp_bind6(&listener, addr);
  }
  else
    throw runtime_error("Invalid host specified for listener");

  if(ret < 0)
    throw runtime_error(System::uv_perror("Failed to start listener"));

  ret = uv_listen(reinterpret_cast<uv_stream_t *>(&listener), 128, Listener::on_connected);

  if(ret < 0)
    throw runtime_error(System::uv_perror("Failed to start listener"));

  Logging::info << "Started listener on [" << host << "]:" << port << Logging::endl;
}


// Statics
void Listener::init()
{
  Config::add_section("listeners", &config);
}

void Listener::create(string host, int port, ListenerFlag flags)
{
  ListenerPtr new_listener(new Listener(host, port, flags));

  listeners.push_back(new_listener);
}

void Listener::start_listeners()
{
  for(auto it = listeners.cbegin(); it != listeners.cend(); it++)
  {
    (*it)->start();
  }
}

void Listener::on_connected(uv_stream_t *stream, int status)
{
  Listener *listener = static_cast<Listener *>(stream->data);

  listener->connected(stream, status);
}
