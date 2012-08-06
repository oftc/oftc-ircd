/*
  Copyright (c) 2012 Stuart Walsh

  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation
  files (the "Software"), to deal in the Software without
  restriction, including without limitation the rights to use,
  copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following
  conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
  OTHER DEALINGS IN THE SOFTWARE.
*/

#include "stdinc.h"
#include <string.h>
#include "config.h"
#include "client.h"

using std::ios;

const char *log_levels[] =
{
  "TRACE",
  "DEBUG",
  "INFO",
  "NOTICE",
  "WARNING",
  "ERROR",
  "CRITICAL",
  NULL
};

LoggingSection Logging::config;
ofstream Logging::log_stream;
Logging Logging::trace(TRACE);
Logging Logging::debug(DEBUGL);
Logging Logging::info(INFO);
Logging Logging::warning(WARNING);
Logging Logging::error(ERRORL);
Logging Logging::critical(CRITICAL);
bool Logging::flush(false);
bool Logging::dostamp(true);
stringstream Logging::stream;

Logging::Logging(LogLevel level) : log_level(level)
{
}

Logging& Logging::operator <<(manip fp)
{
  return fp(*this);
}

// Statics
LogLevel Logging::string_to_level(const string name)
{
  const char **p = log_levels;

  while(p != NULL)
  {
    if(strcasecmp(*p, name.c_str()) == 0)
      return static_cast<LogLevel>(p - log_levels);
    p++;
  }

  return static_cast<LogLevel>(-1);
}

string Logging::level_to_string(const LogLevel level)
{
  return log_levels[level];
}

void Logging::init()
{
  Config::add_section("logging", &config);
}

void Logging::start()
{
  log_stream.open(config.get_log_path(), ios::out | ios::app);
}

Logging& Logging::endl(Logging &log)
{
  Logging& tmp = log << "\n";

  log_stream << stream.str();
  log_stream.flush();
  flush = false;
  dostamp = true;
  stream.str(string());
  stream.clear();

  return tmp;
}
