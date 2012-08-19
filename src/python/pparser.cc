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

#include "stdinc.h"
#include "python/pparser.h"
#include "python/pint.h"

template<> map<string, PMethod<PParser> > PCType<PParser, Parser>::methods = {};

PParser::PParser()
{
}

PParser::PParser(PTuple args, PDict kwargs)
{
}

PParser::~PParser()
{
}

PyObject *PParser::register_command(PyObject *self, PyObject *arg, PyObject *kwargs)
{
  PTuple args(arg);
  PDict kwds(kwargs);
  PString name;
  PObject function;
  PInt min_args, max_args, access, rate_control;

  if(!(name = kwds["name"]))
    name = args[0];

  if(!(function = kwds["function"]))
    function = args[1];

  if(!(min_args = kwds["min_args"]))
    min_args = args[2];

  if(!(max_args = kwds["max_args"]))
    max_args = args[3];

  if(!(access = kwds["access"]))
    access = args[4];

  if(!(rate_control = kwds["rate_control"]))
    rate_control = args[5];

  Command command(handle_command, name, access, min_args, max_args, rate_control, function);

  Parser::get_default().register_command(command);

  Py_RETURN_NONE;
}

// Statics

void PParser::init()
{
  PyTypeObject& type = type_object();

  type.tp_name = "Parser";

  add_method("Register", "register a command in the parser", reinterpret_cast<PyCFunctionWithKeywords>(PParser::register_command));

  PCType::init();
}

void PParser::handle_command(const ClientPtr client, const Command& command, const ParamList& params)
{
}
