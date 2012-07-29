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
from pythonwrap import Client
from sets import Set

@register("NICK", min_args=1, max_args=2, access=0)
def handle_nick(client, nick):
  if Client.find_by_name(nick):
    client.numeric(433, nick)
    return

  client.Name = nick
  Client.add_name(client)

  check_and_register(client)

@register("USER", min_args=4, max_args=4, access=0)
def handle_user(client, username, unused, unused2, realname):
  if client.is_registered():
    client.numeric(462)
    return 

  client.Username = username
  client.Realname = realname

  check_and_register(client)

@register("PING", min_args=1, max_args=1, access=1)
def handle_ping(client, arg):
  client.send(":{me} PONG {me} :{arg}", arg=arg)

@register("MODE", min_args=1, max_args=2, access=1)
def handle_mode(client, name, *arg):
  target = Client.find_by_name(name)

  if not target:
    client.numeric(401, name)
    return

  if target != client:
    client.numeric(502)
    return

  set_before = Set()
  if client.Invisible:
    set_before.add('i')

  if len(arg) == 0:
    mode = "+"
    for c in set_before:
      mode += c

    client.send("{client} MODE :{mode}", mode=mode)
    return
  
  set = True
  invalid = False
  set_after = Set()

  for c in arg[0]:
    if c == '+':
      set = True
    elif c == '-':
      set = False
    elif c == 'i':
      client.Invisible = set
      if set:
        set_after.add(c)
      else:
        if c in set_after:
          set_after.remove(c)
    else:
      invalid = True

  if invalid:
    client.numeric(501)

  mode = ""
  added = set_after - set_before
  removed = set_before - set_after
  if len(added) > 0:
    mode += '+'
    for c in added:
      mode += c
  if len(removed) > 0:
    mode += '-'
    for c in removed:
      mode += c

  client.send(":{client} MODE {name} :{mode}", name=client.Name, mode=mode)

def check_and_register(client):
  if client.is_registered():
    return

  if client.Name and client.Username:
    Client.add(client)

@event(Client.disconnected)
def client_disconnected(client):
  if(client.Name):
    Client.del_name(client)
