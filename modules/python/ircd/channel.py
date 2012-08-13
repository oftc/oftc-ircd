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

from pythonwrap import Channel
import numerics

def set_channel_mode(client, channel, args):
  set_before = set()
  
  if channel.InviteOnly:
    set_before.add('i')

  if channel.Moderated:
    set_before.add('m')

  if channel.NoExternal:
    set_before.add('n')

  if channel.Private:
    set_before.add('p')

  if channel.Secret:
    set_before.add('s')

  if channel.TopicOpsOnly:
    set_before.add('t')

  if channel.Secure:
    set_before.add('S')

  if len(args) == 0:
    mode = "+" + ''.join(set_before)

    client.numeric(numerics.RPL_CHANNELMODEIS, channel.Name, mode, '')
    return

  plus = True
  set_after = set_before.copy()

  modes = set(Channel.supported_modes())

  mode = args[0]

  for c in mode:
    if c == '+':
      plus = True
    elif c == '-':
      plus = False
    elif c == 'b':
      if len(args) == 1:
        client.numeric(numerics.RPL_ENDOFBANLIST, channel.Name)
    elif c in modes:
      channel.set_mode_char(c, plus)
      if plus and not c in set_after:
        set_after.add(c)
      elif not plus and c in set_after:
        set_after.remove(c)
    else:
      client.numeric(numerics.ERR_UNKNOWNMODE, c, channel.Name)

  mode = ""
  added = set_after - set_before
  removed = set_before - set_after

  if len(added) > 0:
    mode = '+' + ''.join(added)
  if len(removed) > 0:
    mode += '-' + ''.join(removed)

  if len(mode) > 0:
    client.send(":{client} MODE {channel} :{mode}", channel=channel, mode=mode)
    channel.send(":{client} MODE {channel} :{mode}", client=client, channel=channel, mode=mode)
