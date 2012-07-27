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
#include <uv.h>
#include "sslconnection.h"
#include "system.h"
#include "ssl.h"

SSLConnection::~SSLConnection()
{
  // This actually frees the underlying bios, so dont do it manually
  SSL_free(ssl);
}

void SSLConnection::accept(uv_stream_t *server_handle)
{
  ssl = SSL_new(Ssl::get_context());
  read_bio = BIO_new(BIO_s_mem());
  write_bio = BIO_new(BIO_s_mem());

  SSL_set_app_data(ssl, this);

  SSL_set_bio(ssl, read_bio, write_bio);
  
  SSL_set_accept_state(ssl);

  Connection::accept(server_handle);
}

void SSLConnection::handle_error(int code)
{
  int err = SSL_get_error(ssl, code);

  switch(err)
  {
  case SSL_ERROR_WANT_READ:
    int pending;

    while((pending = BIO_pending(write_bio)) > 0)
    {
      char *buf = new char[pending];
      int len = BIO_read(write_bio, buf, pending);

      Connection::send(buf, len);

      delete[] buf;
    }
    break;
  default:
    Logging::debug << "Unknown ssl error status" << Logging::endl;
  }
}

void SSLConnection::read(uv_stream_t *stream, ssize_t nread, uv_buf_t buf)
{
  if(nread <= 0)
  {
    Connection::read(stream, nread, buf);
    return;
  }

  BIO_write(read_bio, buf.base, nread);

  if(!SSL_is_init_finished(ssl))
  {
    int ret = SSL_accept(ssl);

    if(ret <= 0)
    {
      handle_error(ret);
      free_buffer(buf);
      return;
    }
  }

  int offset = 0;

  while(BIO_pending(read_bio) || BIO_pending(write_bio))
  {
    int ret = SSL_read(ssl, buf.base + offset, nread - offset);
    if(ret <= 0)
    {
      handle_error(ret);
      free_buffer(buf);
      return;
    }
    offset += ret;
  }

  Connection::read(stream, offset, buf);
}

void SSLConnection::send(string buffer)
{
  send(buffer.c_str(), buffer.length());
}

void SSLConnection::send(const char *buf, size_t len)
{
  int ret = SSL_write(ssl, buf, len);

  if(ret <= 0)
  {
    handle_error(ret);
    throw runtime_error("SSL Write error handling not yet implemented");
  }

  int bytes = BIO_pending(write_bio);
  int offset = 0;
  char *buffer = new char[bytes]();

  while(BIO_pending(write_bio))
  {
    ret = BIO_read(write_bio, buffer + offset, bytes - offset);
    if(ret < 0)
    {
      throw runtime_error("BIO Read errors not implemented yet");
    }
    offset += ret;
  }

  Connection::send(buffer, offset);

  delete[] buffer;
}

// Statics
