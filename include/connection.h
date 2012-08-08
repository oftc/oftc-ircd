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

#ifndef CONNECTION_H_INC
#define CONNECTION_H_INC

#include <vector>
#include <sstream>

#include <uv.h>
#include "parser.h"

using std::vector;
using std::stringstream;

class Connection;
typedef shared_ptr<Connection> ConnectionPtr;

enum DnsState
{
  Reverse = 0,
  Forward,
  Done
};

struct DnsCallbackState
{
  Connection *connection;
  const char *name;
  void *addr;
  int addrlen;
  int af_type;
};

class Connection 
{
protected:
  static unordered_map<Connection *, ConnectionPtr> connections;

  shared_ptr<uv_tcp_t> handle;
  DnsState dns_state;
  bool closing;

  static void free_buffer(uv_buf_t&);

  virtual void read(uv_stream_t *, ssize_t, uv_buf_t);
private:
  stringstream read_buffer;
  Parser& parser;
  ClientPtr client;
  string host;

public:
  // Events
  static Event<ConnectionPtr> ip_connecting;
  static Event<ConnectionPtr> dns_finished;

  // static methods
  static void add(ConnectionPtr);
  static void del(ConnectionPtr);

  // callbacks
  static uv_buf_t on_buf_alloc(uv_handle_t *, size_t);
  static void on_read(uv_stream_t *, ssize_t, uv_buf_t);
  static void on_close(uv_handle_t *);
  static void on_write(uv_write_t *, int );
  static void on_dns(void *, int, int, hostent *);

  // ctor/dtor
  Connection();
  ~Connection();

  // methods
  virtual void accept(uv_stream_t *);
  virtual void send(string);
  virtual void send(const char *, size_t);
  void dns_done(int, hostent *, DnsCallbackState *);
  void close();
  inline bool is_closing() { return closing; }

  // getters
  string get_host() const;

  // setters
  void set_client(const ClientPtr);
};

#endif
