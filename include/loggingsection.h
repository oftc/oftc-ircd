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

#ifndef LOGGINGSECTION_H_INC
#define LOGGINGSECTION_H_INC

#include <json/json.h>
#include <string>
#include "configsection.h"

enum LogLevel
{
  DEBUGL = 0,
  INFO,
  NOTICE,
  WARNING,
  ERRORL,
  CRITICAL
};

class LoggingSection : public ConfigSection
{
private:
  LogLevel min_loglevel;
  std::string log_path;
public:
  void set_defaults();
  void process(const Json::Value);
  void verify() const;
  inline LogLevel get_min_loglevel() const { return min_loglevel; }
  inline const char *get_log_path() const { return log_path.c_str(); }
};

#endif
