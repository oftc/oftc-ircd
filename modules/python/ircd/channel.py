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

from pythonwrap import Channel, NuhMask
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
  ret_args = []
  curr_arg = 1
  largs = list(args)

  for c in mode:
    if c == '+':
      plus = True
      continue
    elif c == '-':
      plus = False
      continue
    elif c in 'bqeI':
      ret = process_list(client, channel, plus, c, largs)
      if ret == 0:
        continue

      for i in range(ret):
        ret_args.append(largs[curr_arg])
        curr_arg += 1
    elif c in modes:
      channel.set_mode_char(c, plus)
    else:
      client.numeric(numerics.ERR_UNKNOWNMODE, c, channel.Name)
      continue
    if plus and not c in set_after:
      set_after.add(c)
    elif not plus and c in set_after:
      set_after.remove(c)

  mode = ""
  added = set_after - set_before
  removed = set_before - set_after

  if len(added) > 0:
    mode = '+' + ''.join(added)
  if len(removed) > 0:
    mode += '-' + ''.join(removed)

  argstr = ''
  for s in ret_args:
    argstr += ' ' + s

  if len(mode) > 0:
    client.send(":{client} MODE {channel} :{mode}{args}", channel=channel, mode=mode, args=argstr)
    channel.send(":{client} MODE {channel} :{mode}{args}", client=client, channel=channel, mode=mode, args=argstr)

def process_list(client, channel, plus, mode, args):
  if mode == 'b':
    list = channel.Bans
    end = numerics.RPL_ENDOFBANLIST
    numeric = numerics.RPL_BANLIST
  elif mode == 'e':
    list = channel.Excepts
    end = numerics.RPL_ENDOFEXCEPTLIST
  elif mode == 'I':
    list = channel.Invexes
    end = numerics.RPL_ENDOFINVEXLIST
  elif mode == 'q':
    list = channel.Quiets
    end = numerics.RPL_ENDOFQUIETLIST

  if len(args) == 1:
    for element in list:
      client.numeric(numeric, channel.Name, str(element))
    client.numeric(end, channel.Name)
    return 0
  else:
    mask = NuhMask(args[1])
    list.append(mask)
    args[1] = str(mask)
    return 1
