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
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include <json/json.h>
#include "config.h"
#include "logging.h"

const char *log_levels[] =
{
  "DEBUG",
  "INFO",
  "NOTICE",
  "WARNING",
  "ERROR",
  "CRITICAL",
  NULL
};

LoggingSection Logging::config;

int
LoggingSection::string_to_level(const std::string& name)
{
  const char **p = log_levels;

  while(p != NULL)
  {
    if(strcasecmp(*p, name.c_str()) == 0)
      return (int)(p - log_levels);
    p++;
  }

  return -1;
}

void
LoggingSection::set_defaults()
{
  min_log_level = Logging::info;
  log_path = LOG_PATH;
}

void
LoggingSection::process(const Json::Value& value)
{
  log_path = value.get("log_path", LOG_PATH).asString(); 
  min_log_level = LoggingSection::string_to_level(value.get("log_level", "info").asString());
}

void
LoggingSection::verify()
{
}

Logging::Logging(int level) : log_level(level)
{
  log_stream.open(LOG_PATH, std::ios::out | std::ios::app);
}

Logging::~Logging()
{
  log_stream.close();
}

Logging& 
Logging::operator <<(const std::string param)
{
  char datestr[Logging::MAX_DATE_LEN];
  time_t current_time;

  if(log_level < Logging::config.get_min_log_level())
    return *this;

  current_time = time(NULL);

  strftime(datestr, sizeof(datestr), "%Y-%m-%d %H:%M:%S%z", 
    localtime(&current_time));
  log_stream << datestr << " - (core) [" << log_levels[log_level] << 
    "] - " << param << std::endl;
  return *this;
}

void
Logging::init()
{
  Config::add_section("logging", &Logging::config);
}
