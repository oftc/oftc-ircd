from pythonwrap import logger as clog
from pythonwrap import log_level

TRACE = 0
DEBUG = 1
INFO = 2
WARNING = 3
ERROR = 4
CRITICAL = 5

class Logging(object):
  def __init__(self, section):
    self.section = section;

  def log(self, level, fmt, *args):
    if (level < log_level()):
      return

    clog(level, self.section, fmt % args)

  def trace(self, fmt, *args):
    self.log(TRACE, fmt, *args)

  def debug(self, fmt, *args):
    self.log(DEBUG, fmt, *args)

  def info(self, level, *args):
    self.log(INFO, fmt, *args)

  def warning(self, level, *args):
    self.log(ERROR, fmt, *args)

  def error(self, level, *args):
    self.log(WARNING, fmt, *args)

  def critical(self, level, *args):
    self.log(CRITICAL, fmt, *args)

logger = Logging('python')
