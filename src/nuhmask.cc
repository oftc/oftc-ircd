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
#include "nuhmask.h"

NuhMask::NuhMask() 
{
  Logging::trace << "Created NuhMask: " << this << Logging::endl;
}

NuhMask::NuhMask(const string& mask)
{
  parse_mask(mask);
}

NuhMask::~NuhMask()
{
  Logging::trace << "Destroyed NuhMask: " << this << Logging::endl;
}

void NuhMask::parse_mask(const string& mask)
{
  full_mask = mask;
  size_t pos, last;

  last = pos = mask.find_first_of('!');
  if(pos == string::npos || pos == 0)
    name = "*";
  else
    name = mask.substr(0, pos).c_str();

  if(last == string::npos)
    last = 0;
  else
    last++;

  pos = mask.find_first_of('@', last);
  if(pos == string::npos || pos == last)
    username = "*";
  else
    username = mask.substr(last, pos - last).c_str();

  last = pos;
  if(last == string::npos)
    last = 0;
  else
    last++;

  host = mask.substr(last, pos - last);
  if(host.empty())
    host = "*";
}

string NuhMask::str() const
{
  stringstream ss;

  ss << name << "!" << username << "@" << host;

  return ss.str();
}
