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
#include "channel.h"

list<ChannelPtr> Channel::channels;
unordered_map<irc_string, ChannelPtr> Channel::names;

Channel::Channel() 
{
  Logging::trace << "Created Channel: " << this << Logging::endl;
}

Channel::~Channel()
{
  Logging::trace << "Destroyed Channel: " << this << Logging::endl;
}

irc_string Channel::get_name() const
{
  return name;
}

void Channel::set_name(const irc_string _name)
{
  name = _name;
}

irc_string Channel::str() const
{
  return name;
}

// Statics
void Channel::add(const ChannelPtr channel)
{
  names[channel->name] = channel;
  channels.push_back(channel);
}

ChannelPtr Channel::find(const irc_string name)
{
  return names[name];
}

void Channel::del(const ChannelPtr channel)
{
  channels.remove(channel);
  names.erase(channel->name);
}