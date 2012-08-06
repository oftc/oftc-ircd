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

from pythonwrap import Client, get_motd
import numerics

def check_and_register(client):
  if client.is_registered():
    return

  if client.Name and client.Username:
    Client.add(client)
    send_motd(client)

def set_user_mode(client, mode):
  set_before = set()
  if client.Invisible:
    set_before.add('i')
  
  if not mode:
    mode = "+"
    for c in set_before:
      mode += c

    client.send("{client} MODE :{mode}", mode=mode)
    return

  plus = True
  invalid = False
  set_after = set()

  for c in mode:
    if c == '+':
      plus = True
    elif c == '-':
      plus = False
    elif c == 'i':
      client.Invisible = plus
      if plus:
        set_after.add(c)
      else:
        if c in set_after:
          set_after.remove(c)
    else:
      invalid = True

  if invalid:
    client.numeric(numerics.ERR_UMODEUNKNOWNFLAG)

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

  if len(mode) > 0:
    client.send(":{client} MODE {name} :{mode}", name=client.Name, mode=mode)

def send_motd(client):
  motd = get_motd()

  if len(motd) == 0:
    client.numeric(numerics.ERR_NOMOTD)
    return

  client.numeric(numerics.RPL_MOTDSTART, Client.Me.Name)

  lines = motd.split("\n")

  for s in lines:
    client.numeric(numerics.RPL_MOTD, s[:-1])

  client.numeric(numerics.RPL_MOTDEND)