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
#include <unordered_map>
#include "command.h"
#include "event.h"
#include "baseclient.h"

using std::string;
using std::vector;
using std::unordered_map;

class Client;

class Connection;

class Client : public BaseClient
{
private:
  static ClientPtr me;
  static list<ClientPtr> client_list;

  string username;
  string realname;
public:
  static Event<ClientPtr> connected;
  static Event<ClientPtr> registered;
  static Event<ClientPtr> disconnected;

  void send(string arg, int);
  void send(int, ...);
  using BaseClient::send;
  
  static void init();
  static void add(ClientPtr);
  static void remove(ClientPtr);
  static inline ClientPtr get_me() { return me; }

  inline string get_username() const { return username; }
  inline string get_realname() const { return realname; }

  inline void set_username(string _username) { username = _username; }
  inline void set_realname(string _realname) { realname = _realname; }

  string str();
};

#endif
