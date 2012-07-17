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

#include "Python.h"
#include "structmember.h"
#include "stdinc.h"
#include "python/pythonwrap.h"
#include "python/parserwrap.h"

static PyMethodDef parser_methods[] =
{
  { "Register", reinterpret_cast<PyCFunction>(ParserWrap::register_command),  
    METH_STATIC | METH_KEYWORDS | METH_VARARGS, "Register a command with the parser" },
  { NULL, NULL, 0, NULL }
};

static PyMemberDef parser_members[] =
{
  { NULL, 0, 0, 0, NULL }
};

ParserWrap::ParserWrap()
{
}

ParserWrap::ParserWrap(PyObject *args, PyObject *kwds)
{
}

ParserWrap::~ParserWrap()
{
}

// Statics

void
ParserWrap::init()
{
  PythonWrap<ParserWrap>::methods = parser_methods;
  PythonWrap<ParserWrap>::members = parser_members;
  PythonWrap<ParserWrap>::init("parser");
}

PyObject *
ParserWrap::register_command(PyObject *self, PyObject *args, PyObject *kwargs)
{
  char *kwlist[] = 
  {
    "name",
    "function",
    "min_args",
    "max_args",
    "access",
    "rate_control"
  };
  char *name;
  PyObject *function;
  unsigned int min_args, max_args, access, rate_control;

  min_args = max_args = access = rate_control = 0;

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "sO|IIII", kwlist, 
    &name, &function, &min_args, &max_args, &access, &rate_control))
  {
    return NULL;
  }

  if(!PyCallable_Check(function))
  {
    PyErr_SetString(PyExc_TypeError, "argument 2 must be callable");
    return NULL;
  }

  Command command(handle_command, string(name), static_cast<AccessLevel>(access),
    min_args, max_args, rate_control, function);

  Parser::get_default().register_command(command);

  Py_INCREF(function);
  Py_INCREF(Py_None);
  return Py_None;
}

void
ParserWrap::handle_command(const Client& client, const Command& command, const vector<string>& args)
{
  PyObject *tuple;

  tuple = PyTuple_New(args.size() + 1);

  PyTuple_SetItem(tuple, 0, PyString_FromString(command.get_name().c_str()));

  for(unsigned int i = 1; i < args.size() + 1; i++)
  {
    PyTuple_SetItem(tuple, i, PyString_FromString(args[i - 1].c_str()));
  }

  PyObject * ret = PyObject_CallObject(static_cast<PyObject *>(command.get_data()), tuple);
  if(ret == NULL)
    PyErr_Print();
}