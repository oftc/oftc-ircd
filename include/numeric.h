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

#ifndef NUMERIC_H_INC
#define NUMERIC_H_INC

#include "json/json.h"
#include <stdarg.h>
#include <string>

using std::string;

class Numeric
{
private:
  static Json::Value message_table;
public:
  static const int Rpl_Welcome = 001;
  static const int Rpl_YourHost = 002;
  static const int Rpl_Created = 003;
  static const int Rpl_MyInfo = 004;

  // Error replies
  static const int Err_UnknownCommand = 421;
  static const int Err_NeedMoreParams = 461;
  static const int Err_AlreadyRegistered = 462;

  static string format(int, va_list);
  static void load_messages(string);
  static string raw_format(int);
};

#endif
