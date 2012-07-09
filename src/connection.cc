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
#include <uv.h>
#include <stdexcept>
#include "connection.h"
#include "system.h"
#include "logging.h"

std::vector<Connection> Connection::connections;

void
Connection::accept(uv_stream_t server_handle)
{
  sockaddr_in6 addr;
  sockaddr *saddr;
  char buf[INET6_ADDRSTRLEN];
  int addrlen = sizeof(struct sockaddr_in6);
  int ret;

  uv_tcp_init(uv_default_loop(), &handle);

  ret = uv_accept(&server_handle, reinterpret_cast<uv_stream_t *>(&handle));
  if(ret < 0)
    throw std::runtime_error(System::uv_perror("Unable to accept connection"));

  ret = uv_tcp_getpeername(&handle, reinterpret_cast<sockaddr*>(&addr), &addrlen);
  saddr = reinterpret_cast<sockaddr *>(&addr);
  if(saddr->sa_family == AF_INET)
    uv_ip4_name(reinterpret_cast<sockaddr_in *>(saddr), buf, sizeof(buf));
  else if(saddr->sa_family == AF_INET6)
    uv_ip6_name(&addr, buf, sizeof(buf));

  Logging::debug << "Accepted connection from: " << buf << Logging::endl;
}

// Statics
void
Connection::add(Connection connection)
{
  connections.push_back(connection);
}
