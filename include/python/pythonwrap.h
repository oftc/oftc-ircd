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

#ifndef PYTHONWRAP_H_INC
#define PYTHONWRAP_H_INC

#include "Python.h"

template <class T> class PythonWrap : public PyObject
{
protected:
  static PyTypeObject type_object;
public:
  // Non Python methods
  static void init(const char *);

  // Python methods
  static PyObject *alloc(PyTypeObject *, Py_ssize_t);
  static PyObject *create(PyTypeObject *, PyObject *, PyObject *);
  static void dealloc(PyObject *);
  static void free(void *);
  static bool handle_event(PyObject *, PyObject *);
  static T *wrap(void *arg);

  // Getters
  static PyTypeObject *get_type_object();
  static PyObject *get_type();
};

#endif
