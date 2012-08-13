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

#ifndef EVENTWRAP_H_INC
#define EVENTWRAP_H_INC

#include "Python.h"

class EventWrap;
typedef function<PyObject *(EventWrap *, PyObject *)> EventCallback;

class EventWrap : public PythonWrap<EventWrap>
{
private:
  PyObject *listeners;
  PyObject *handler;
  EventCallback fire_func;
public:
  // Non Python methods
  static void init(PyObject *);

  // Get/Set Wrappers
  static PyObject *get_wrap(EventWrap *, void *);
  static int set_wrap(EventWrap *, PyObject *, void *);
  static EventWrap *register_event(EventCallback);

  // static methods
  static PyObject *fire(EventWrap *, PyObject *);

  // Ctor/dtors
  EventWrap(PyObject *, PyObject *);
  ~EventWrap();

  // Getters
  PyObject *get_listeners() const;
  PyObject *get_handler() const;

  // Setters
  void set_listeners(PyObject *);
  void set_handler(PyObject *);
};

#endif
