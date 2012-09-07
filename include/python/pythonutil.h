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

#ifndef PYTHONUTIL_H_INC
#define PYTHONUTIL_H_INC

#include "Python.h"
#include "python/pobject.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

class PTuple;
class PDict;

class PythonUtil
{
private:
public:
  static PyObject *get_motd(PyObject *, PyObject *);
  static PyObject *logger(PyObject *, PyObject *);
  static PyObject *log_level(PyObject *, PyObject *);
  static void init_python();
  static void log_error();
  static void register_type(PyObject *, PyTypeObject&);
  static PObject send_format(const PObject&, const PTuple&, const PDict&);
};

#endif
