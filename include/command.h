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

#ifndef COMMAND_H_INC
#define COMMAND_H_INC

#include "stdinc.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

class Command; 
class BaseClient;

typedef vector<string> ParamList;

typedef function<void(const shared_ptr<BaseClient> client, const Command& command, const ParamList& params)> CommandHandler;

enum AccessLevel
{
  Unregistered = 0,
  Registered,
  Oper
};

class Command
{
private:
  CommandHandler handler;
  string name;
  AccessLevel min_access;
  unsigned int min_args;
  unsigned int max_args;
  int rate_control;
  void *data;
public:
  Command();
  Command(CommandHandler, string, AccessLevel, unsigned int, unsigned int, int, void *);
  ~Command();

  inline string get_name() const { return name; }
  inline CommandHandler get_handler() const { return handler; }
  inline void *get_data() const { return data; }
  inline unsigned int get_min_args() const { return min_args; }
  inline unsigned int get_max_args() const { return max_args; }
  inline AccessLevel get_min_access() const { return min_access; }
};

#endif
