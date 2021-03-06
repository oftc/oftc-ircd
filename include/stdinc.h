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

#ifndef STDINC_H_INC
#define STDINC_H_INC

#if defined(_MSC_VER)
#if __clang__
#define HAS_VARTEMPLATES 1
#else
#undef HAS_VARTEMPLATES
#endif
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE (1)
#endif
#pragma warning(disable : 4996)
#else
#define HAS_VARTEMPLATES 1
#endif

#include <memory>
#include <unordered_map>
#include <functional>

#define _WINSOCKAPI_

using std::dynamic_pointer_cast;
using std::shared_ptr;
using std::unordered_map;
using std::function;

#include <stdexcept>
#include <string>
#include <list>
#include <map>
#include <vector>

using std::runtime_error;
using std::exception;
using std::string;
using std::map;
using std::list;
using std::vector;

#ifdef _WIN32 
#define strncasecmp strnicmp 
#define strcasecmp stricmp 
#else
#include <strings.h>
#endif 

#include "logging.h"

#endif
