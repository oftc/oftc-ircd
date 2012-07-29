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

template class PythonWrap<EventWrap>;

static PyMethodDef event_methods[] =
{
  { NULL, NULL, 0, NULL }
};

static PyMemberDef event_members[] =
{
  { NULL, 0, 0, 0, NULL }
};

static PyGetSetDef event_getsetters[] = 
{
  { const_cast<char*>("listeners"), reinterpret_cast<getter>(EventWrap::get_wrap), 
    reinterpret_cast<setter>(EventWrap::set_wrap), const_cast<char*>("listeners"), 
    reinterpret_cast<void *>(const_cast<char *>("listeners")) },
  { const_cast<char*>("handler"), reinterpret_cast<getter>(EventWrap::get_wrap), 
    reinterpret_cast<setter>(EventWrap::set_wrap), const_cast<char*>("handler"), 
    reinterpret_cast<void *>(const_cast<char *>("handler")) },
  { NULL, NULL, NULL, NULL, NULL }
};

EventWrap::EventWrap(PyObject *args, PyObject *kwds)
{
  Logging::debug << "Created EventWrap: " << this << Logging::endl;
}

EventWrap::~EventWrap()
{
  Logging::debug << "Destroyed EventWrap: " << this << Logging::endl;
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

void EventWrap::init()
{
  PythonWrap<EventWrap>::type_object.tp_methods = event_methods;
  PythonWrap<EventWrap>::type_object.tp_members = event_members;
  PythonWrap<EventWrap>::type_object.tp_getset = event_getsetters;
  PythonWrap<EventWrap>::init("Event");
}

PyObject *EventWrap::get_wrap(EventWrap *event, void *closure)
{
  string prop = string(static_cast<char *>(closure));

  if(prop == "listeners")
    return event->get_listeners();
  else if(prop == "handler")
    return event->get_handler();

  return NULL;
}

int EventWrap::set_wrap(EventWrap *event, PyObject *value, void *closure)
{
  string prop = string(static_cast<char *>(closure));

  if(prop == "listeners")
    event->set_listeners(value);
  else if(prop == "handler")
    event->set_handler(value);

  return 0;
}
