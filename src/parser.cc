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

#include "stdinc.h"
#include <algorithm>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include "parser.h"

using std::string;
using std::stringstream;
using std::vector;
using std::transform;
using std::map;

Parser Parser::default_parser;

void
Parser::register_command(Command command)
{
  commands[command.get_name()] = command;
}

void
Parser::parse(const string& line)
{
  stringstream stream(line);
  vector<string> args;
  string command, arg;

  if(line[0] == ':')
  {
    string prefix;
    
    stream >> prefix;

    // lookup prefix
  }

  stream >> command;
  transform(command.begin(), command.end(), command.begin(), toupper);

  map<string, Command>::iterator it = commands.find(command);
  if(it == commands.end())
  {
    // Invalid command, go away
  }

  Command& cmd = it->second;

  while(stream >> arg)
  {
    if(arg[0] == ':')
    {
      stringstream tmpstream;

      tmpstream << arg.substr(1);

      while(stream >> arg)
        tmpstream << ' ' << arg;

      arg = tmpstream.str();
    }
    args.push_back(arg);
  }

  cmd.get_handler()(Client(), cmd, args);
}