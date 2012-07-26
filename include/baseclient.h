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

#ifndef BASECLIENT_H_INC
#define BASECLIENT_H_INC

#include "stdinc.h"
#include <string>
#include <list>
#include "command.h"

using std::string;
using std::list;

class Connection;
class BaseClient;

typedef shared_ptr<BaseClient> ClientPtr;

class BaseClient
{
protected:
  static list<ClientPtr> client_list;
  static unordered_map<string, ClientPtr> names;

  string name;
  string host;
  shared_ptr<Connection> connection;
  AccessLevel level;
public:
  static inline void add_name(ClientPtr client) { names[client->name] = client; }
  static inline ClientPtr find_by_name(string name) { return names[name]; }
  static inline void del_name(ClientPtr client) { names.erase(client->name); }
  
  BaseClient();
  virtual ~BaseClient() = 0;

  virtual void send(string);
  
  inline bool is_registered() const { return level >= Registered; }
  inline void set_resgistered() { level = Registered; }
  inline string get_name() const { return name; }
  inline void set_name(string _name) { name = _name; }
  void set_connection(shared_ptr<Connection> _connection);
 
  virtual string str();
};

#endif
