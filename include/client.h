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

#ifndef CLIENT_H_INC
#define CLIENT_H_INC

#include <string>
#include <vector>
#include <uv.h>
#include "event.h"
#include "baseclient.h"
#include "channel.h"

using std::string;
using std::vector;

class Client;
class Connection;

class Client : public BaseClient
{
private:
  static map<BaseClient *, ClientPtr> client_list;
  static list<ClientPtr> unregistered_list;
  static uv_timer_t ping_timer;

  string username;
  string realname;
  bool invisible;
  time_t last_message;
  list<ChannelPtr> channels;
public:
  // Events
  static Event<ClientPtr> connected;
  static Event<ClientPtr> registering;
  static Event<ClientPtr> disconnected;
  static Event<ClientPtr, irc_string> nick_changing;
  static Event<ClientPtr, string> nick_changed;

  // Client list maniuplators
  static void add(const ClientPtr);
  static void add_unregistered(const ClientPtr);
  static void remove(const ClientPtr);

  // Static members
  static void init();
  static void check_pings(uv_timer_t *, int);

  // ctor/dtor
  Client();

  // Members
  void add_channel(const ChannelPtr);
  void close(const string);
  void remove_channel(const ChannelPtr);
  void send(const string arg, int);
  void send(int, ...);
  using BaseClient::send;
  void send_channels_common(const string);
  irc_string str() const;
  
  // Tests
  bool is_invisible() const;

  // Getters
  string get_username() const;
  string get_realname() const;
  time_t get_idletime() const;


  // Setters
  void set_username(const string);
  void set_realname(const string);
  void set_invisible(bool invis);
  void set_last_message(time_t);
};

#endif
