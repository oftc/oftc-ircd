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

#include <Python.h>
#include <structmember.h>
#include "stdinc.h"
#include "python/pythonwrap.h"
#include "python/eventwrap.h"

template<> PyTypeObject PythonWrap<EventWrap, EventCallback>::type_object = {};

enum Property
{
  Listeners = 1,
  Handler
};

static PyMethodDef event_methods[] =
{
  { "fire", reinterpret_cast<PyCFunction>(EventWrap::fire), METH_VARARGS, "fire the event" },
  PY_METHOD_END
};

static PyGetSetDef event_getsetters[] = 
{
  PY_GETSET("listeners", EventWrap, "listeners", Listeners), 
  PY_GETSET("handler", EventWrap, "handler", Handler),
  PY_GETSET_END
};

/*EventWrap::EventWrap(PyObject *args, PyObject *kwds)
{
  PyObject *func;

  PyArg_ParseTuple(args, "O", &func);

  fire_func = *reinterpret_cast<EventCallback *>(PyCObject_AsVoidPtr(func));

  Logging::trace << "Created EventWrap: " << this << Logging::endl;
}*/

EventWrap::~EventWrap()
{
  Logging::trace << "Destroyed EventWrap: " << this << Logging::endl;
}

PyObject *EventWrap::get_listeners() const 
{ 
  if(listeners == NULL)
    Py_RETURN_NONE;

  Py_INCREF(listeners);

  return listeners; 
}

PyObject *EventWrap::get_handler() const 
{ 
  if(handler == NULL)
    Py_RETURN_NONE;

  Py_INCREF(handler);
  return handler; 
}

void EventWrap::set_listeners(PyObject *value) 
{
  Py_XDECREF(listeners);
  Py_INCREF(value);

  listeners = value; 
}

void EventWrap::set_handler(PyObject *value) 
{ 
  Py_XDECREF(handler);
  Py_INCREF(value);

  handler = value; 
}

// Statics

void EventWrap::init(PyObject *module)
{
  PythonWrap<EventWrap, EventCallback>::type_object.tp_methods = event_methods;
  PythonWrap<EventWrap, EventCallback>::type_object.tp_getset = event_getsetters;
  PythonWrap<EventWrap, EventCallback>::init(module, "Event");
}

EventWrap *EventWrap::register_event(EventCallback callback)
{
  return EventWrap::wrap(&callback);
}

PyObject *EventWrap::get_wrap(EventWrap *event, void *closure)
{
  int prop = *reinterpret_cast<int *>(&closure);

  switch(prop)
  {
    case Listeners:
      return event->get_listeners();
    case Handler:
      return event->get_handler();
  }

  return NULL;
}

int EventWrap::set_wrap(EventWrap *event, PyObject *value, void *closure)
{
  int prop = *reinterpret_cast<int *>(&closure);
  
  switch(prop)
  {
    case Listeners:
      event->set_listeners(value);
      break;
    case Handler:
      event->set_handler(value);
      break;
  }

  return 0;
}

PyObject *EventWrap::fire(EventWrap *event, PyObject *args)
{
  return event->fire_func(event, args);
}
