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

#ifndef PYTHONBASE_H_INC
#define PYTHONBASE_H_INC

#include "Python.h"

#define PY_METHOD(name, func, flags, doc) { name, reinterpret_cast<PyCFunction>(func), flags, doc }
#define PY_METHOD_END { NULL, NULL, 0, NULL }

#define PY_GETSET(name, type, doc, flags) { const_cast<char *>(name), reinterpret_cast<getter>(type::get_wrap), \
  reinterpret_cast<setter>(type::set_wrap), const_cast<char *>(doc), reinterpret_cast<void *>(flags) }
#define PY_GETSET_END { NULL, NULL, NULL, NULL, NULL }

class PythonBase : public PyObject
{
protected:
  PyObject *object;
public:
  static PyObject *alloc(PyTypeObject *, Py_ssize_t);
  static void free(void *);
  static void init();
  static PyTypeObject& type_object();

};

#endif
