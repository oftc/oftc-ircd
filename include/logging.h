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
#include <iostream>
#include <fstream>
#include "configsection.h"

#define LOG_PATH "/home/stu/oircd/var/log/ircd.log"

class LoggingSection : public ConfigSection
{
private:
  int min_log_level;
  std::string log_path;

  static int string_to_level(const std::string&);
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
  static LoggingSection config;

  std::ofstream log_stream;
  int log_level;
public:
  static const int debug = 0;
  static const int info = 1;
  static const int notice = 2;
  static const int warning = 3;
  static const int error = 4;
  static const int critical = 5;
  static const int MAX_DATE_LEN = 30;

  static void init();

  Logging& operator <<(const std::string);

  Logging(int);
  ~Logging();
};

#endif
