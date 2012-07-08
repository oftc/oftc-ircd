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

#include <string>
#include <iostream>
#include <fstream>
#include "loggingsection.h"

#define LOG_PATH "/home/stu/oircd/var/log/ircd.log"

class Logging;

typedef Logging& (*manip)(Logging&);

class Logging 
{
private:
  static LoggingSection config;
  static const int MAX_DATE_LEN = 30;
  static std::ofstream log_stream;
  static bool flush;
  static bool dostamp;
  static std::stringstream stream;

  LogLevel log_level;
public:  
  static Logging debug;
  static Logging info;
  static Logging notice;
  static Logging warning;
  static Logging error;
  static Logging critical;

  static void init();
  static void start();
  static LogLevel string_to_level(const std::string);
  inline static LogLevel get_min_loglevel() { return config.get_min_loglevel(); }

  Logging& operator <<(const std::string);
  Logging& operator <<(manip);

  Logging(LogLevel);

  static Logging& endl(Logging&);
};

#endif
