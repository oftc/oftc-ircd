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

template<> map<string, PMethod<PParser> > PCType<PParser, Parser>::methods;

PParser::PParser()
{
}

PParser::PParser(PTuple args, PDict kwargs)
{
}

PParser::~PParser()
{
}

PObject PParser::register_command(PTuple, PDict)
{
  Py_RETURN_NONE;
}

// Statics

void PParser::init()
{
  PyTypeObject& type = type_object();

  type.tp_name = "Parser";

  add_method("register_command", METH_VARARGS, "register a command in the parser", KeywordArgsMethod(&PParser::register_command));

  PCType::init();
}
