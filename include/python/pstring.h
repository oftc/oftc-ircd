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

#ifndef PSTRING_H_INC
#define PSTRING_H_INC

#include "Python.h"
#include "python/pobject.h"
#include "ircstring.h"

class PString : public PObject
{
public:
  PString();
  PString(PyObject *);
  PString(const char *);
  PString(string);
  PString(irc_string);
  ~PString();

  inline operator string() const { return string(c_str()); }
  inline operator const char *() const { return c_str(); }
  inline operator bool() const { return object != NULL; }

  using PObject::operator=;

  const char *c_str() const;

  inline static bool check(const PObject& item) { return PyString_Check(item) != 0; }
};

#endif
