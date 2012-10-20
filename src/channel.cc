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
#include "config.h"

Event<ChannelPtr, BaseClientPtr> Channel::joining;
Event<ChannelPtr, BaseClientPtr> Channel::joined;
ChannelList Channel::channels;
ChannelNameHash Channel::names;
ChannelSection Channel::config;
Membership Channel::NullMember;

Channel::Channel() : moderated(false), invite_only(false), no_external_msgs(false),
  private_chan(false), secret(false), topic_op_only(false), secure(false)
{
  Logging::trace << "Created Channel: " << this << Logging::endl;
}

Channel::~Channel()
{
  Logging::trace << "Destroyed Channel: " << this << Logging::endl;
}

void Channel::add_member(const BaseClientPtr client)
{
  Membership member;
  ClientPtr ptr = dynamic_pointer_cast<Client>(client);

  member.client = client;
  if(members.size() == 0)
    member.flags = ChanOp;
  else
    member.flags = Member;

  member.channel = channels[this];

  members[client] = member;
  ptr->add_channel(member);

  joined(member.channel, client);
}

bool Channel::is_member(const BaseClientPtr client) const
{
  return members.find(client) != members.end();
}

Membership& Channel::find_member(const BaseClientPtr client)
{
  auto member = members.find(client);

  if(member == members.end())
    return NullMember;
  else
    return member->second;
}

void Channel::remove_member(const BaseClientPtr client)
{
  members.erase(client);
}

irc_string Channel::get_name() const
{
  return name;
}

ChannelMemberList Channel::get_members() const
{
  return members;
}

NuhMaskList& Channel::get_bans()
{
  return bans;
}

NuhMaskList& Channel::get_invexes()
{
  return invexes;
}

NuhMaskList& Channel::get_quiets()
{
  return quiets;
}

NuhMaskList& Channel::get_excepts()
{
  return excepts;
}

string Channel::get_topic() const
{
  return topic;
}

void Channel::set_name(const irc_string _name)
{
  name = _name;
}

void Channel::set_mode_char(char mode, bool set)
{
  switch(mode)
  {
  case 'i':
    invite_only = set;
    break;
  case 'm':
    moderated = set;
    break;
  case 'n':
    no_external_msgs = set;
    break;
  case 'p':
    private_chan = set;
    break;
  case 's':
    secret = set;
    break;
  case 't':
    topic_op_only = set;
    break;
  case 'S':
    secure = set;
    break;
  default:
    Logging::debug << "unknown mode char: " << mode << Logging::endl;
    break;
  }
}

void Channel::set_topic(const string _topic)
{
  topic = _topic;
  if(topic.length() > Channel::get_topiclen())
    topic.resize(Channel::get_topiclen());
}

void Channel::send_names(BaseClientPtr client)
{
  ClientPtr ptr = dynamic_pointer_cast<Client>(client);
  stringstream reply;
  string reply_format;
  bool first = true;
  int min_len = 1 + Server::get_me()->get_name().length() + 1 + 3 + 1 + client->get_name().length() + 1; // ":Server 000 nick "

  reply_format = Numeric::format_str(Numeric::Rpl_NamesReply, '=', name.c_str(), "");
  reply << reply_format;

  for(auto it = members.cbegin(); it != members.cend(); it++)
  {
    Membership ms = it->second;
    if(min_len + reply.str().length() + ms.client->get_name().length() >= 510)
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

    if(ms.flags & ChanOp)
      reply << '@';
    else if(ms.flags & Voice)
      reply << '+';

    reply << ms.client->get_name();
  }
  
  ptr->send(reply.str(), Numeric::Rpl_NamesReply);

  ptr->send(Numeric::Rpl_EndOfNames, name.c_str());
}

void Channel::send(const BaseClientPtr client, const string str)
{
  for(auto it = members.cbegin(); it != members.cend(); it++)
  {
    if(it->second.client != client)
      it->second.client->send(str);
  }
}

irc_string Channel::str() const
{
  return name;
}

// Statics

void Channel::init()
{
  Config::add_section("channel", &config);
}

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
