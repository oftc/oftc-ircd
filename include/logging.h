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
#include <sstream>
#include <time.h>
#include "loggingsection.h"

#ifdef _WIN32
#define LOG_PATH "ircd.log"
#else
#define LOG_PATH "/home/stu/oircd/var/log/ircd.log"
#endif

using std::ofstream;
using std::stringstream;
using std::string;

class Logging;

typedef Logging& (*manip)(Logging&);


class Logging 
{
private:
  static LoggingSection config;
  static const int MAX_DATE_LEN = 40;
  static ofstream log_stream;
  static bool flush;
  static bool dostamp;
  static stringstream stream;

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
  static LogLevel string_to_level(const string);
  static string level_to_string(const LogLevel);
  inline static LogLevel get_min_loglevel() { return config.get_min_loglevel(); }

  template<typename T>
  inline Logging& operator <<(T param)
  {
    if(log_level < Logging::get_min_loglevel())
      return *this;

    if(!log_stream.is_open())
      return *this;

    if(dostamp)
    {
      tm *tptr;
      char datestr[Logging::MAX_DATE_LEN];
      time_t current_time;

      current_time = time(NULL);

      tptr = localtime(&current_time);

      strftime(datestr, sizeof(datestr)+10, "%Y-%m-%d %H:%M:%S%z", tptr);

      stream << datestr << " - (core) [" << level_to_string(log_level) << "] - ";

      dostamp = false;
    }

    stream << param;

    if(flush)
    {
      log_stream << stream.str();
      log_stream.flush();
      flush = false;
      dostamp = true;
      stream.str(string());
      stream.clear();
    }

    return *this;
  }  
  Logging& operator <<(manip);

  Logging(LogLevel);

  static Logging& endl(Logging&);
};

#endif
