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

from ircd import register, event
from ircd.user import *
from pythonwrap import Client
import numerics

@register("NICK", min_args=1, max_args=2, access=0)
def handle_nick(client, nick):
  if Client.find_by_name(nick):
    client.numeric(numerics.ERR_NICKNAMEINUSE, nick)
    return

  client.Name = nick
  Client.add_name(client)

  check_and_register(client)

@register("USER", min_args=4, max_args=4, access=0)
def handle_user(client, username, unused, unused2, realname):
  if client.is_registered():
    client.numeric(numerics.ERR_ALREADYREGISTERED)
    return 

  client.Username = username
  client.Realname = realname

  check_and_register(client)

@register("PING", min_args=1, max_args=1, access=1)
def handle_ping(client, arg):
  client.send(":{me} PONG {me} :{arg}", arg=arg)

@register("MODE", min_args=1, max_args=2, access=1)
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

@register("WHOIS", min_args=1, max_args=2, access=1)
def handle_whois(client, name, *arg):
  target = Client.find_by_name(name)

  if not target:
    client.numeric(numerics.ERR_NOSUCHNICK, name)
    return

  client.numeric(numerics.RPL_WHOISUSER, client.Name, client.Username, client.Host, client.Realname)
  client.numeric(numerics.RPL_WHOISSERVER, client.Name, str(Client.Me), Client.Me.Info)
  client.numeric(numerics.RPL_ENDOFWHOIS, client.Name)

@event(Client.disconnected)
def client_disconnected(client):
  if(client.Name):
    Client.del_name(client)
