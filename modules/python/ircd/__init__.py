from functools import wraps

from pythonwrap import Parser

def register(command, min_args=0, max_args=0, access=0, rate_control=0):
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
