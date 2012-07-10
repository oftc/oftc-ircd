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
Connection::accept(uv_stream_t *server_handle)
{
  sockaddr_storage addr;
  sockaddr_in *addr4;
  sockaddr_in6 *addr6;
  char ipstr[INET6_ADDRSTRLEN];
  int addrlen = sizeof(addr);
  int ret;

  uv_tcp_init(uv_default_loop(), &handle);

  ret = uv_accept(server_handle, reinterpret_cast<uv_stream_t *>(&handle));
  if(ret < 0)
    throw std::runtime_error(System::uv_perror("Unable to accept connection"));

  ret = uv_tcp_getpeername(&handle, reinterpret_cast<sockaddr*>(&addr), &addrlen);
  switch(addr.ss_family)
  {
  case AF_INET:
    addr4 = reinterpret_cast<sockaddr_in *>(&addr);
    uv_ip4_name(addr4, ipstr, sizeof(ipstr));
    break;
  case AF_INET6:
    addr6 = reinterpret_cast<sockaddr_in6 *>(&addr);
    uv_ip6_name(addr6, ipstr, sizeof(ipstr));
    break;
  }

  handle.data = this;

  uv_read_start(reinterpret_cast<uv_stream_t *>(&handle), on_buf_alloc, on_read);
  Logging::debug << "Accepted connection from: " << ipstr << Logging::endl;
}

void
Connection::read(uv_stream_t *stream, ssize_t nread, uv_buf_t buf)
{
  Logging::debug << "Read " << nread << "bytes '" << buf.base << "'" << Logging::endl;

  if(nread < 0)
  {
    if(buf.base != NULL)
    {
      delete[] buf.base;
      buf.base = 0;
      buf.len = 0;
    }
  }

  delete[] buf.base;
  buf.base = 0;
  buf.len = 0;
}

// Statics
Connection &
Connection::add()
{
  connections.push_back(Connection());

  return connections.back();
}

uv_buf_t
Connection::on_buf_alloc(uv_handle_t *handle, size_t size)
{
  uv_buf_t buf;

  buf.len = size;
  buf.base = new char[size]();

  return buf;
}

void 
Connection::on_read(uv_stream_t *stream, ssize_t nread, uv_buf_t buf)
{
  Connection *connection = static_cast<Connection *>(stream->data);

  connection->read(stream, nread, buf);
}

