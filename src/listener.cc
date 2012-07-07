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
#include <iostream>
#include <uv.h>
#include "config.h"
#include "listener.h"
#include "listenersection.h"
#include "system.h"

ListenerSection Listener::config;
std::vector<Listener> Listener::listeners;

void
Listener::init()
{
  Config::add_section("listeners", &Listener::config);
}

Listener::Listener() : host(NULL), port(6667)
{
}

Listener::Listener(const char *host, int port=6667) : host(host), port(port)
{
}

void
Listener::connected(uv_stream_t *stream, int status)
{
}

void
Listener::start()
{
  struct sockaddr_in addr;
  int ret;

  addr = uv_ip4_addr(host.c_str(), port);

  ret = uv_tcp_init(uv_default_loop(), &listener);
  listener.data = this;
  ret = uv_tcp_bind(&listener, addr);
  ret = uv_listen((uv_stream_t *)&listener, 128, Listener::connected_callback);
}

void
Listener::add(const char *host, int port=6667)
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

