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
#include <vector>
#include <stdexcept>
#include <uv.h>
#include "config.h"
#include "system.h"
#include "listener.h"
#include "listenersection.h"
#include "logging.h"

ListenerSection Listener::config;
std::vector<Listener> Listener::listeners;

Listener::Listener() : host(""), port(6667)
{
}

Listener::Listener(std::string host, int port=6667) : host(host), port(port)
{
}

void
Listener::connected(uv_stream_t *stream, int status)
{
}

void
Listener::start()
{
  int ret;

  ret = uv_tcp_init(uv_default_loop(), &listener);
  listener.data = this;

  if(ret < 0)
    throw std::runtime_error(System::uv_perror("Failed to start listener"));

  if(host.find('.') != std::string::npos)
  {
    struct sockaddr_in addr = uv_ip4_addr(host.c_str(), port);

    ret = uv_tcp_bind(&listener, addr);
  }
  else if(host.find(':') != std::string::npos)
  {
    struct sockaddr_in6 addr = uv_ip6_addr(host.c_str(), port);
     
    ret = uv_tcp_bind6(&listener, addr);
  }
  else
    throw std::runtime_error("Invalid host specified for listener");

  if(ret < 0)
    throw std::runtime_error(System::uv_perror("Failed to start listener"));

  ret = uv_listen((uv_stream_t *)&listener, 128, Listener::connected_callback);

  if(ret < 0)
    throw std::runtime_error(System::uv_perror("Failed to start listener"));

  Logging::info << "Started listener on [" << host << "]:" << port << Logging::endl;
}


// Statics
void
Listener::init()
{
  Config::add_section("listeners", &Listener::config);
}

void
Listener::add(std::string host, int port=6667)
{
  listeners.push_back(Listener(host, port));
}

void
Listener::start_listeners()
{
  for(std::vector<Listener>::iterator it = listeners.begin(); 
      it != listeners.end(); it++)
  {
    (*it).start();
  }
}

void
Listener::connected_callback(uv_stream_t *stream, int status)
{
  Listener *listener = (Listener *)stream->data;

  listener->connected(stream, status);
}
