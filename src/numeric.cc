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
#include <iostream>
#include <fstream>
#include <iomanip>
#include "numeric.h"

using std::ifstream;
using std::setw;
using std::setfill;

Json::Value Numeric::message_table;

void
Numeric::load_messages(string path)
{
  ifstream message_file(path.c_str());
  Json::Reader reader;
  Json::Value root;

  if(!message_file.is_open())
    throw runtime_error(string("Failed to open message file: ") + path);

  if(!reader.parse(message_file, root))
    throw runtime_error(reader.getFormattedErrorMessages());

  if(root.type() != Json::objectValue)
    throw runtime_error("Root node is not an object");

  message_table = *root.begin();
}

string
Numeric::format(int numeric, va_list args)
{
  char buffer[510 + 1];
  string format;

  format = raw_format(numeric);

  vsnprintf(buffer, sizeof(buffer), format.c_str(), args);

  return string(buffer);
}

string
Numeric::raw_format(int numeric)
{
  stringstream numstr;

  numstr << setw(3) << setfill('0') << numeric;
  Json::Value num = message_table.get(numstr.str(), Json::Value(Json::nullValue));

  if(num.isNull())
  {
    Logging::warning << "Request for unknown numeric: " << numeric << Logging::endl;
    return "";
  }

  return num.asString();
}
