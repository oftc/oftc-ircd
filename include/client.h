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
#include "event.h"
#include "baseclient.h"

using std::string;
using std::vector;

class Client;
class Connection;

class Client : public BaseClient
{
private:
  static list<ClientPtr> client_list;

  string username;
  string realname;
  bool invisible;
public:
  // Events
  static Event<ClientPtr> connected;
  static Event<ClientPtr> registered;
  static Event<ClientPtr> disconnected;

  // Client list maniuplators
  static void add(const ClientPtr);
  static void remove(const ClientPtr);

  // ctor/dtor
  Client();

  // Members
  void send(const string arg, int);
  void send(int, ...);
  using BaseClient::send;
  irc_string str() const;
  
  // Tests
  bool is_invisible() const;

  // Getters
  string get_username() const;
  string get_realname() const;

  // Setters
  void set_username(const string);
  void set_realname(const string);
  void set_invisible(bool invis);
};

#endif
