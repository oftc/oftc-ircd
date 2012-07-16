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
#include "logging.h"
#include <openssl/ssl.h>
#include <openssl/err.h>

void
SSLConnection::accept(uv_stream_t *server_handle)
{
  ssl = SSL_new(System::get_ssl_context());
  read_bio = BIO_new(BIO_s_mem());
  write_bio = BIO_new(BIO_s_mem());

  SSL_set_app_data(ssl, this);

  SSL_set_bio(ssl, read_bio, write_bio);
  
  SSL_set_accept_state(ssl);

  Connection::accept(server_handle);
}

void
SSLConnection::handle_error(int code)
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

      send(buf, len);

      delete[] buf;
    }
    break;
  default:
    Logging::debug << "Unknown ssl error status" << Logging::endl;
  }
}

void
SSLConnection::read(uv_stream_t *stream, ssize_t nread, uv_buf_t buf)
{
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

  int ret = SSL_read(ssl, buf.base, nread);
  if(ret <= 0)
  {
    handle_error(ret);
    free_buffer(buf);
    return;
  }

  Connection::read(stream, ret, buf);
}

// Statics
SSLConnection *
SSLConnection::create()
{
  ConnectionPtr conn_ptr(new SSLConnection);

  Connection::connections.push_back(conn_ptr);

  return dynamic_cast<SSLConnection *>(conn_ptr.get());
}