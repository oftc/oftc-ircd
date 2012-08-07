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

#ifndef CHANNEL_H_INC
#define CHANNEL_H_INC

#include "stdinc.h"
#include <string>
#include <list>
#include "ircstring.h"

using std::list;
using std::string;

class Channel;

typedef shared_ptr<Channel> ChannelPtr;

class Channel
{
private:
  static unordered_map<irc_string, ChannelPtr> names;
  static list<ChannelPtr> channels;

  irc_string name;
public:
  // Ctor/dtor
  Channel();
  ~Channel();

  // list managers
  static void add(const ChannelPtr);
  static ChannelPtr find(const irc_string);
  static void del(const ChannelPtr);

  // getters
  irc_string get_name() const;

  // setters
  void set_name(const irc_string);
  
  // members
  irc_string str() const;
 };

#endif
