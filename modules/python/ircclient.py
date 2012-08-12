#  Copyright (c) 2012 Stuart Walsh
#
#  Permission is hereby granted, free of charge, to any person
#  obtaining a copy of this software and associated documentation
#  files (the "Software"), to deal in the Software without
#  restriction, including without limitation the rights to use,
#  copy, modify, merge, publish, distribute, sublicense, and/or sell
#  copies of the Software, and to permit persons to whom the
#  Software is furnished to do so, subject to the following
#  conditions:
#
#  The above copyright notice and this permission notice shall be
#  included in all copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
#  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
#  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
#  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
#  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
#  OTHER DEALINGS IN THE SOFTWARE.

from ircd import register, event, Target, have_target, numerics, msflags
from ircd.user import *
from pythonwrap import Client, Channel
import time

@register("NICK", min_args=1, max_args=1, access=0)
def handle_nick(client, nick):
  target = Client.find_by_name(nick)
  if target and target != client:
    client.numeric(numerics.ERR_NICKNAMEINUSE, nick)
    return

  old_str = str(client)

  if not Client.nick_changing.fire(client, nick):
    return

  Client.del_name(client)
  client.Name = nick
  Client.add_name(client)

  if not client.is_registered():
    check_and_register(client)
  else:
    client.send(":{oldstr} NICK :{nick}", oldstr=old_str, nick=client.Name)
    Client.nick_changed.fire(client, old_str)

@register("USER", min_args=4, max_args=4, access=0)
def handle_user(client, username, unused, unused2, realname):
  if client.is_registered():
    client.numeric(numerics.ERR_ALREADYREGISTERED)
    return 

  client.Username = username
  client.Realname = realname

  check_and_register(client)

@register("PING", min_args=1, max_args=1)
def handle_ping(client, arg):
  client.send(":{me} PONG {me} :{arg}", arg=arg)

@register("PONG", min_args=1, max_args=2, access=0)
def handle_pong(client, *args):
  pass

@register("MODE", min_args=1, max_args=2)
def handle_mode(client, name, *arg):
  if name[0] == '#':
    client.numeric(numerics.RPL_CHANNELMODEIS, name, '+', '')
  else:
    target = Client.find_by_name(name)

    if not target:
      client.numeric(numerics.ERR_NOSUCHNICK, name)
      return

    if target != client:
      client.numeric(numerics.ERR_USERSDONTMATCH)
      return

    if len(arg) > 0:
      set_user_mode(client, arg[0])
    else:
      set_user_mode(client, None)

@register("WHO", min_args=1, max_args=2)
@have_target(epilog=numerics.RPL_ENDOFWHO)
def handle_who(client, target, *arg):
  if isinstance(target, Channel):
    for ms in target.Members:
      client.numeric(numerics.RPL_WHOREPLY, target.Name, ms.Client.Username, ms.Client.Host, ms.Client.Server.Name, ms.Client.Name, "H", 0, ms.Client.Realname)
  else:
    client.numeric(numerics.RPL_WHOREPLY, '*', target.Username, target.Host, target.Server.Name, target.Name, "H", 0, target.Realname)
  
  client.numeric(numerics.RPL_ENDOFWHO, target.Name)

@register("WHOIS", min_args=1, max_args=2)
@have_target(epilog=numerics.RPL_ENDOFWHOIS)
def handle_whois(client, target, *arg):
  client.numeric(numerics.RPL_WHOISUSER, target.Name, target.Username, target.Host, target.Realname)
  client.numeric(numerics.RPL_WHOISIDLE, target.Name, target.Idletime);
  client.numeric(numerics.RPL_WHOISSERVER, target.Name, str(Client.Me), Client.Me.Info)
  min_len = 1 + len(str(Client.Me)) + 1 + 3 + 1 + len(target.Name) # ":foo.server.com 000 client :"
  channels = ""
  for membership in target.Channels:
    if min_len + len(channels) + len(membership.Channel.Name) >= 510:
      client.numeric(numerics.RPL_WHOISCHANNELS, target.Name, channels)
      channels = ""
    
    if membership.Flags & msflags.CHANOP:
      channels += '@'
    elif membership.Flags & msflags.VOICE:
      channels += '+'

    channels += membership.Channel.Name + " "

  if len(target.Channels) > 0:
    client.numeric(numerics.RPL_WHOISCHANNELS, target.Name, channels)

  if target.is_ssl():
    client.numeric(numerics.RPL_WHOISSSL, target.Name)
  client.numeric(numerics.RPL_ENDOFWHOIS, target.Name)

@register("WHOWAS", min_args=1, max_args=2)
@have_target(numeric=numerics.ERR_WASNOSUCHNICK, epilog=numerics.RPL_ENDOFWHOWAS)
def handle_whowas(client, name, *arg):
  pass

@register("QUIT", min_args=0, max_args=1, access=0)
def handle_quit(client, *arg):
  if len(arg) > 0:
    reason = arg[0]
  else:
    reason = ""

  client.close(reason)

@register("PRIVMSG", min_args=2, max_args=2)
@have_target()
def handle_privmsg(client, target, message):
  target.send(":{client} PRIVMSG {name} :{message}", client=client, name=target.Name, message=message)
  client.LastMessage = int(time.time())

@register("NOTICE", min_args=2, max_args=2)
@have_target()
def handle_notice(client, target, message):
  target.send(":{client} NOTICE {name} :{message}", client=client, name=target.Name, message=message)

@register("MOTD", min_args=0, max_args=1)
def handle_motd(client, *args):
  send_motd(client)

@register("JOIN", min_args=1, max_args=2)
def handle_join(client, cname, *args):
  for name in cname.split(','):
    channel = Channel.find(name)
    if not channel:
      if name[0] != '#':
        client.numeric(numerics.ERR_BADCHANNAME, name)
        continue

      channel = Channel()
      channel.Name = name
      Channel.add(channel)

    if not channel.is_member(client) and Channel.joining.fire(channel, client):
      channel.add_member(client)
      channel.send_names(client)

@register("NAMES", min_args=1, max_args=1)
@have_target()
def handle_names(client, target):
  target.send_names(client)

@register("PART", min_args=1, max_args=2)
@have_target(Target.CHANNEL)
def handle_part(client, target, *args):
  if len(args) > 0:
    reason = args[0]
  else:
    reason = ""

  if not target.is_member(client):
    client.numeric(numerics.ERR_NOTONCHANNEL, target.Name)
    return

  target.send(":{client} PART {channel} :{reason}", client=client, channel=target.Name, reason=reason)
  client.send(":{client} PART {channel} :{reason}", channel=target.Name, reason=reason)
  client.remove_channel(target)
  target.remove_member(client)

@event(Client.closing)
def client_closing(client, reason):
  client.send_channels_common(":{client} QUIT :{reason}", reason=reason)

@event(Client.nick_changed)
def client_nick_changed(client, old_source):
  client.send_channels_common(":{old} NICK :{new}", old=old_source, new=client.Name)

@event(Channel.joined)
def channel_joined(channel, client):
  client.send(":{client} JOIN :{channel}", channel=channel)
  channel.send(":{client} JOIN :{channel}", client=client, channel=channel)
