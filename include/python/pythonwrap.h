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

template <class T> class PythonWrap : PyObject
{
protected:
  static PyTypeObject type_object;
  static PyMethodDef *methods;
  static PyMemberDef *members;
  static PyGetSetDef *getsetters;
  static reprfunc str;
public:
  static void init(const char *);
  static PyObject *alloc(PyTypeObject *, Py_ssize_t);
  static PyObject *create(PyTypeObject *, PyObject *, PyObject *);
  static bool handle_event(PyObject *, PyObject *);
  static void free(void *);
  static void dealloc(PyObject *);
  static inline PyTypeObject *get_type_object() { return &type_object; }
  static inline PyObject *get_type() { return reinterpret_cast<PyObject *>(&type_object); }
  static T *wrap(void *arg);
};

void python_init();

#endif
