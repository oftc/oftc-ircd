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

#ifndef NUHMASK_H_INC
#define NUHMASK_H_INC

#include "stdinc.h"
#include <string>
#include <list>
#include "ircstring.h"
#include "baseclient.h"

using std::string;
using std::list;

class NuhMask;

typedef list<NuhMask> NuhMaskList;

class NuhMask
{
private:
  irc_string name;
  irc_string username;
  string host;
  string full_mask;

  void parse_mask(const string&);
public:  
  NuhMask();
  NuhMask(const string&);
  ~NuhMask();

  bool operator==(const NuhMask& right)
  {
    return full_mask == right.get_mask();
  }

  inline irc_string get_name() const { return name; }
  inline irc_string get_username() const { return username; }
  inline string get_host() const { return host; }
  inline string get_mask() const { return full_mask; }

  bool match(const NuhMask&);
  bool match(const ClientPtr);

  string str() const;
 };

#endif
