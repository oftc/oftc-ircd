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
#include <string>
#include <sstream>
#include <vector>
#include "parser.h"

using std::string;
using std::stringstream;
using std::vector;

Parser Parser::default_parser;

void
Parser::parse(const string& line) const
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

  // lookup command

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
}