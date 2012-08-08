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
#include "numeric.h"
#include "client.h"
#include "server.h"

map<Channel *, ChannelPtr> Channel::channels;
unordered_map<irc_string, ChannelPtr> Channel::names;

Channel::Channel() 
{
  Logging::trace << "Created Channel: " << this << Logging::endl;
}

Channel::~Channel()
{
  Logging::trace << "Destroyed Channel: " << this << Logging::endl;
}

void Channel::add_member(const ClientPtr client)
{
  Membership member;
  shared_ptr<Client> ptr = dynamic_pointer_cast<Client>(client);

  member.client = client;
  if(members.size() == 0)
    member.flags = ChanOp;
  else
    member.flags = Member;

  stringstream ss;

  ss << ":" << client->str() << " JOIN :" << name;
  send(client, ss.str());

  members.push_back(member);
  ptr->add_channel(channels[this]);
}

irc_string Channel::get_name() const
{
  return name;
}

void Channel::set_name(const irc_string _name)
{
  name = _name;
}

void Channel::send_names(ClientPtr client)
{
  list<Membership>::const_iterator it;
  shared_ptr<Client> ptr = dynamic_pointer_cast<Client>(client);
  stringstream reply;
  string reply_format;
  bool first = true;
  int min_len = 1 + Server::get_me()->get_name().length() + 1 + 3 + 1 + client->get_name().length() + 1; // ":Server 000 nick "

  reply_format = Numeric::format_str(Numeric::Rpl_NamesReply, '=', name.c_str(), "");
  reply << reply_format;

  for(it = members.begin(); it != members.end(); it++)
  {
    if(min_len + reply.str().length() + it->client->get_name().length() >= 510)
    {
      ptr->send(reply.str(), Numeric::Rpl_NamesReply);
      reply.clear();
      reply.str("");
      reply << reply_format;
    }

    if(first)
      first = false;
    else
      reply << ' ';

    if(it->flags & ChanOp)
      reply << '@';
    else if(it->flags & Voice)
      reply << '+';

    reply << it->client->get_name();
  }
  
  ptr->send(reply.str(), Numeric::Rpl_NamesReply);

  ptr->send(Numeric::Rpl_EndOfNames, name.c_str());
}

void Channel::send(const ClientPtr client, const string str)
{
  list<Membership>::const_iterator it;

  for(it = members.begin(); it != members.end(); it++)
  {
    if(it->client != client)
      it->client->send(str);
  }
}

irc_string Channel::str() const
{
  return name;
}

// Statics
void Channel::add(const ChannelPtr channel)
{
  names[channel->name] = channel;
  channels[channel.get()] = channel;
}

ChannelPtr Channel::find(const irc_string name)
{
  return names[name];
}

void Channel::del(const ChannelPtr channel)
{
  channels.erase(channel.get());
  names.erase(channel->name);
}