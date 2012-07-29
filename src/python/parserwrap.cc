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
#include "python/clientwrap.h"
#include "command.h"
#include "parser.h"
#include "client.h"

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

ParserWrap::ParserWrap(PyObject *args, PyObject *kwds)
{
  Logging::debug << "Created ParserWrap: " << this << Logging::endl;
}

ParserWrap::~ParserWrap()
{
  Logging::debug << "Destroyed ParserWrap: " << this << Logging::endl;
}

// Statics

void ParserWrap::init(PyObject *module)
{
  PythonWrap<ParserWrap>::type_object.tp_methods = parser_methods;
  PythonWrap<ParserWrap>::type_object.tp_members = parser_members;
  PythonWrap<ParserWrap>::init(module, "Parser");
}

PyObject *ParserWrap::register_command(PyObject *self, PyObject *args, PyObject *kwargs)
{
  const char *kwlist[] = 
  {
    "name",
    "function",
    "min_args",
    "max_args",
    "access",
    "rate_control",
    NULL
  };
  char *name;
  PyObject *function;
  unsigned int min_args, max_args, access, rate_control;

  min_args = max_args = access = rate_control = 0;

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "sO|IIII", const_cast<char**>(kwlist), 
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

  Py_RETURN_NONE;
}

void ParserWrap::handle_command(const ClientPtr client, const Command& command, const vector<string>& params)
{
  PyObject *args;
  ClientWrap *wrapped_client;
  ClientPtr ptr = client;

  args = PyTuple_New(params.size() + 1);
  if(args == NULL)
  {
    PyErr_Print();
    return;
  }

  wrapped_client = ClientWrap::wrap(&ptr);

  if(wrapped_client == NULL)
  {
    PyErr_Print();
    Py_DECREF(args);
    return;
  }

  PyTuple_SetItem(args, 0, wrapped_client);

  for(unsigned int i = 1; i < params.size() + 1; i++)
  {
    PyTuple_SetItem(args, i, PyString_FromString(params[i - 1].c_str()));
  }

  PyObject *ret = PyObject_CallObject(static_cast<PyObject *>(command.get_data()), args);
  if(ret == NULL)
  {
    Py_DECREF(args);
    PyErr_Print();
    return;
  }

  Py_DECREF(ret);
  Py_DECREF(args);
}
