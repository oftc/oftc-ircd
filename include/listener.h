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
typedef vector<ListenerPtr> ListenerList;

enum ListenerFlag
{
  Listener_SSL = 0x1
};

class Listener 
{
private:
  static ListenerSection config;
  static ListenerList listeners;

  uv_tcp_t listener;
  string host;
  int port;
  ListenerFlag flags;

  static void on_connected(uv_stream_t *, int);

  void connected(uv_stream_t *, int);
  void set_flag(ListenerFlag);
public:
  static const int DEFAULT_PORT = 6667;

  // static methods
  static void init();
  static void create(const string, int, ListenerFlag);
  static void start_listeners();

  // ctor/dtor
  Listener();
  Listener(const string, int, ListenerFlag);
  ~Listener();

  // methods
  void start();

  // testers
  bool is_ssl() const;

  // setters
  void set_ssl();
};

#endif
