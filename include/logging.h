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

#ifndef LOGGING_H_INC
#define LOGGING_H_INC

#include <json/json.h>
#include <string>
#include "configsection.h"

#define LOG_PATH "/home/stu/oircd/var/log/ircd.log"

typedef enum 
{
  LOG_DEBUG = 0,
  LOG_INFO,
  LOG_NOTICE,
  LOG_WARNING,
  LOG_ERROR,
  LOG_CRITICAL
} log_levels;

class LoggingSection : public ConfigSection
{
private:
  int min_log_level;
  std::string log_path;
public:
  void set_defaults();
  void process(Json::Value);
  void verify();
  inline int get_min_log_level() { return min_log_level; }
  inline const char *get_log_path() { return log_path.c_str(); }
};

class Logging 
{
private:
  static FILE *logptr;
  static LoggingSection config;
public:
  static void init();
  static void log(log_levels, const char *, ...);
};

#define MAX_LOG_LEVEL_NAME 20

void logging_log(log_levels, const char *, ...);
log_levels logging_string_to_level(const char *);

#endif
