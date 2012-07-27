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
  static unordered_map<string, ClientPtr> names;

  string name;
  string host;
  shared_ptr<Connection> connection;
  AccessLevel level;
public:
  static void init();

  // list managers
  static void add_name(const ClientPtr);
  static ClientPtr find_by_name(const string);
  static void del_name(const ClientPtr);

  // static tests
  static bool is_client(const ClientPtr);
  static bool is_server(const ClientPtr);
  
  // ctor/dtor
  BaseClient();
  virtual ~BaseClient() = 0;

  // members
  virtual void send(string);
  virtual string str() const;
 
  // tests
  bool is_registered() const;

  // clears
  void clear_connection();

  // getters
  string get_name() const;

  // setters
  void set_registered();
  void set_name(const string);
  void set_connection(const shared_ptr<Connection>);
 };

#endif
