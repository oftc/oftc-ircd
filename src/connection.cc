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
#include <string>
#include <uv.h>
#include <utility>
#include "connection.h"
#include "system.h"
#include "client.h"

using std::getline;
using std::pair;

unordered_map<Connection *, ConnectionPtr> Connection::connections;

Connection::Connection() : parser(Parser::get_default())
{
  Logging::debug << "Created Connection: " << this << Logging::endl;
}

Connection::~Connection()
{
  Client::disconnected(client);
  Client::remove(client);
  Logging::debug << "Destroyed Connection: " << this << Logging::endl;
}

void
Connection::accept(uv_stream_t *server_handle)
{
  sockaddr_storage addr;
  sockaddr_in *addr4;
  sockaddr_in6 *addr6;
  char ipstr[INET6_ADDRSTRLEN];
  int addrlen = sizeof(addr);
  int ret;

  handle = shared_ptr<uv_tcp_t>(new uv_tcp_t);

  uv_tcp_init(uv_default_loop(), handle.get());

  ret = uv_accept(server_handle, reinterpret_cast<uv_stream_t *>(handle.get()));
  if(ret < 0)
    throw runtime_error(System::uv_perror("Unable to accept connection"));

  ret = uv_tcp_getpeername(handle.get(), reinterpret_cast<sockaddr*>(&addr), &addrlen);
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

  host = ipstr;

  handle->data = this;

  uv_read_start(reinterpret_cast<uv_stream_t *>(handle.get()), on_buf_alloc, on_read);
  Logging::debug << "Accepted connection from: " << ipstr << Logging::endl;
}

void
Connection::send(const char *buffer, size_t len)
{
  uv_buf_t buf;
  uv_write_t *req = new uv_write_t;

  buf.base = const_cast<char*>(buffer);
  buf.len = len;
  uv_write(req, reinterpret_cast<uv_stream_t*>(handle.get()), &buf, 1, on_write);
}

void
Connection::send(string buffer)
{
  send(buffer.c_str(), buffer.length());
}

void
Connection::read(uv_stream_t *stream, ssize_t nread, uv_buf_t buf)
{
  string debug_str;

  if(nread < 0)
  {
    if(buf.base != NULL)
      free_buffer(buf);

    uv_close(reinterpret_cast<uv_handle_t *>(handle.get()), on_close);
    return;
  }

  if(nread == 0)
  {
    free_buffer(buf);
    return;
  }

  debug_str.insert(0, buf.base, nread);

  read_buffer.write(buf.base, nread);
  string line;

  while(getline(read_buffer, line))
  {
    if(read_buffer.eof())
    {
      read_buffer.clear();
      read_buffer << line;
      break;
    }

    if(line[line.length() - 1] == '\r')
      line.erase(line.length() - 1, 1);

    Logging::debug << "Complete command found '" << line << "'" << Logging::endl;

    parser.parse(client, line);
  }

  if(read_buffer.fail())
    read_buffer.clear();

  free_buffer(buf);
}

// Statics
void
Connection::add(ConnectionPtr connection)
{
  connections.insert(pair<Connection *, ConnectionPtr>(connection.get(), connection));
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

void 
Connection::on_close(uv_handle_t *handle)
{
  Connection *connection = static_cast<Connection *>(handle->data);

  connections.erase(connection);
  connection->client->clear_connection();
}

void
Connection::on_write(uv_write_t *req, int status)
{
  delete req;
}

void
Connection::free_buffer(uv_buf_t &buf)
{
  delete[] buf.base;
  buf.base = 0;
  buf.len = 0;
}
