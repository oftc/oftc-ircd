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

#ifndef LISTENER_H_INC
#define LISTENER_H_INC

#include <string>
#include <vector>
#include <uv.h>
#include "listenersection.h"

using std::vector;
using std::string;

class Listener;

typedef shared_ptr<Listener> ListenerPtr;

enum ListenerFlag
{
  Listener_SSL = 0x1
};

class Listener 
{
private:
  static ListenerSection config;
  static vector<ListenerPtr> listeners;
  static void on_connected(uv_stream_t *, int);

  uv_tcp_t listener;
  string host;
  int port;
  ListenerFlag flags;

  void connected(uv_stream_t *, int);

  inline void set_flag(ListenerFlag flag) 
  { 
    int int_flags = static_cast<int>(flags);
    int_flags |= static_cast<int>(flag); 

    flags = static_cast<ListenerFlag>(int_flags);
  }

public:
  static const int DEFAULT_PORT = 6667;

  static void init();
  static Listener *create(string, int, ListenerFlag);
  static void start_listeners();

  Listener();
  Listener(string, int, ListenerFlag);

  void start();

  inline bool is_ssl() const { return flags & Listener_SSL; }
  inline void set_ssl() { set_flag(Listener_SSL); }
};

#endif
