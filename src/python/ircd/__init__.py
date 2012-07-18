from pythonwrap import Parser

def register(command, min_args=0, max_args=0, access=0, rate_control=0):
  def decorator(func):
    Parser.Register(command, func,
      min_args=min_args,
      max_args=max_args,
      access=access,
      rate_control=rate_control
    )

    def wrapper(*args, **kwargs):
      return func(*args, **kwargs)

    wrapper.__name__ = func.__name__
    wrapper.__doc__  = func.__doc__
    wrapper.__dict__.update(func.__dict__)

    return wrapper
  return decorator
