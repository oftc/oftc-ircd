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

class EventWrap : public PythonWrap<EventWrap>
{
private:
  PyObject *listeners;
  PyObject *handler;
public:
  EventWrap(PyObject *, PyObject *);
  ~EventWrap();

  inline PyObject *get_listeners() const 
  { 
    if(listeners == NULL)
      Py_RETURN_NONE;

    Py_INCREF(listeners);
    return listeners; 
  }

  inline PyObject *get_handler() const 
  { 
    if(handler == NULL)
      Py_RETURN_NONE;

    Py_INCREF(handler);
    return handler; 
  }

  inline void set_listeners(PyObject *value) 
  {
    Py_XDECREF(listeners);
    Py_INCREF(value);

    listeners = value; 
  }
  inline void set_handler(PyObject *value) 
  { 
    Py_XDECREF(handler);
    Py_INCREF(value);

    handler = value; 
  }

  static void init();
  static PyObject *get_wrap(EventWrap *, void *);
  static int set_wrap(EventWrap *, PyObject *, void *);
};

#endif