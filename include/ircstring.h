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

#ifndef IRCSTRING_H_INC
#define IRCSTRING_H_INC

#include <string>
#include <iostream>
#include <algorithm>
#include "murmurhash3.h"

using std::char_traits;
using std::basic_string;
using std::ostream;
using std::istream;
using std::transform;

inline static char to_upper(char left)
{
  if(left == '{')
    return '[';
  if(left == '}')
    return ']';
  if(left == '|')
    return '\\';
  if(left == '^')
    return '~';

  return toupper(left);
}

class irc_traits : public char_traits<char>
{
public:
  static bool eq(char left, char right)
  {
    return to_upper(left) == to_upper(right);
  }

  static bool lt(char left, char right)
  {
    return to_upper(left) < to_upper(right);
  }

  static bool gt(char left, char right)
  {
    return to_upper(left) > to_upper(right);
  }

  static int compare(const char *left, const char *right, size_t size)
  {
    return strncasecmp(left, right, size);
  }
};

typedef basic_string<char, irc_traits> irc_string;

namespace std
{
template<>
struct hash<irc_string> : public unary_function<irc_string, size_t>
{
public:
  size_t operator()(const irc_string& v) const
  {
    unsigned int hash_value;
    irc_string tmp = v;

    transform(v.begin(), v.end(), tmp.begin(), to_upper);

    MurmurHash3_x86_32(tmp.c_str(), tmp.length(), 0x4F9B438D, &hash_value);

    return hash_value;
  }
};
}

inline ostream& operator<<(ostream &left, const irc_string& right)
{
  return left << right.c_str();
}

#endif
