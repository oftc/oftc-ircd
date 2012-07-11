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
#include <cstddef> // for __GLIBCXX__
 
#ifdef __GLIBCXX__
#  include <tr1/memory>
#else
#  ifdef __IBMCPP__
#    define __IBMCPP_TR1__
#  endif
#  include <memory>
#endif
#include <sstream>

#include <uv.h>

using std::vector;
using std::tr1::shared_ptr;
using std::stringstream;

class Connection;

typedef shared_ptr<Connection> ConnectionPtr;

class Connection 
{
private:
  static vector<ConnectionPtr> connections;

  shared_ptr<uv_tcp_t> handle;
  stringstream read_buffer;

  void read(uv_stream_t *, ssize_t, uv_buf_t);
public:
  static Connection *create();
  static uv_buf_t on_buf_alloc(uv_handle_t *, size_t);
  static void on_read(uv_stream_t *, ssize_t, uv_buf_t);

  void accept(uv_stream_t *);
};

#endif
