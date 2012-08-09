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

from ircd import register, event, have_target
from ircd.user import *
from pythonwrap import Client, Channel
import numerics
import time

@register("NICK", min_args=1, max_args=1, access=0)
def handle_nick(client, nick):
  target = Client.find_by_name(nick)
  if target and target != client:
    client.numeric(numerics.ERR_NICKNAMEINUSE, nick)
    return

  old_str = str(client)

  Client.del_name(client)
  client.Name = nick
  Client.add_name(client)

  if not client.is_registered():
    check_and_register(client)
  else:
    client.send(":{oldstr} NICK :{nick}", oldstr=old_str, nick=client.Name)

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
  if name[:1] == '#':
    pass
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

@register("WHOIS", min_args=1, max_args=2)
@have_target(epilog=numerics.RPL_ENDOFWHOIS)
def handle_whois(client, target, *arg):
  client.numeric(numerics.RPL_WHOISUSER, target.Name, target.Username, target.Host, target.Realname)
  client.numeric(numerics.RPL_WHOISIDLE, target.Name, target.Idletime);
  client.numeric(numerics.RPL_WHOISSERVER, target.Name, str(Client.Me), Client.Me.Info)
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
  target.send(":{source} PRIVMSG {name} :{message}", source=str(client), client=client, name=target.Name, message=message)
  client.LastMessage = int(time.time())

@register("NOTICE", min_args=2, max_args=2)
@have_target()
def handle_notice(client, target, message):
  target.send(":{source} NOTICE {name} :{message}", source=str(client), client=client, name=target.Name, message=message)

@register("MOTD", min_args=0, max_args=1)
def handle_motd(client, *args):
  send_motd(client)

@register("JOIN", min_args=1, max_args=2)
def handle_join(client, name, *args):
  channel = Channel.find(name)
  if not channel:
    channel = Channel()
    channel.Name = name
    Channel.add(channel)
    
  channel.add_member(client)
  client.send(":{client} JOIN :{channel}", channel=str(channel));
  channel.send_names(client)

@register("NAMES", min_args=1, max_args=1)
#@have_target(numeric=numerics.ERR_NOSUCHCHANNEL)
def handle_names(client, target):
  chan = Channel.find(target)
  if not chan:
    client.numeric(numerics.ERR_NOSUCHCHANNEL, target)
    return
  chan.send_names(client)

@event(Client.disconnected)
def client_disconnected(client):
  if(client.Name):
    Client.del_name(client)

@event(Client.closing)
def client_closing(client, reason):
  client.send_channels_common(":{client} QUIT :{reason}", reason=reason)
