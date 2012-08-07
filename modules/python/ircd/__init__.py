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

from functools import wraps

from pythonwrap import Parser, Client, Channel

import numerics

def register(command, min_args=0, max_args=0, access=1, rate_control=0):
  def decorator(func):
    Parser.Register(command, func,
      min_args=min_args,
      max_args=max_args,
      access=access,
      rate_control=rate_control
    )

    @wraps(func)
    def wrapper(*args, **kwargs):
      return func(*args, **kwargs)

    return wrapper
  return decorator

def event(evt):
  def decorator(func):
    if not evt.listeners:
      evt.listeners = []

    evt.listeners.append(func)

    if not evt.handler:
      def handler(*args, **kwargs):
        ret = True
        for listener in evt.listeners:
          ret = listener(*args, **kwargs)
          if not ret:
            break
        return ret
      evt.handler = handler

    @wraps(func)
    def wrapper(*args, **kwargs):
      return func(*args, **kwargs)

    return wrapper
  return decorator

def have_target(numeric=numerics.ERR_NOSUCHNICK, epilog=None):
  def wrapper(func):
    @wraps(func)
    def decorator(client, name, *args, **kwargs):
      if name[0] == '#':
        target = Channel.find(name)
        if numeric == numerics.ERR_NOSUCHNICK:
          numeric = numerics.ERR_NOSUCHCHANNEL
      else:
        target = Client.find_by_name(name)

      if not target:
        client.numeric(numeric, name)
        if epilog:
          client.numeric(epilog, name)
        return
      else:
        return func(client, target, *args, **kwargs)

    return decorator
  return wrapper
