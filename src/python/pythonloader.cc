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
#include "stdinc.h"
#include <string>
#include <sstream>
#include "python/pythonloader.h"
#include "python/pythonwrap.h"
#include "python/parserwrap.h"
#include "module.h"

using std::string;
using std::stringstream;

typedef vector<string>::const_iterator VectorStringConstIt;

static PyMethodDef module_methods[] =
{
  { NULL, NULL, 0, NULL }
};

void 
PythonLoader::init()
{
  PyObject *m;

  Py_InitializeEx(0);

  stringstream path;

  path << Py_GetPath();
  vector<string> paths = Module::get_module_paths();

  for(VectorStringConstIt it = paths.begin(); it != paths.end(); it++)
  {
    path << ":" << *it;
  }

  PySys_SetPath(const_cast<char*>(path.str().c_str()));

  ParserWrap::init();

  m = Py_InitModule3("pythonwrap", module_methods, 
    "Wrapper module for oftc-ircd C(++) interface");

  Py_INCREF(ParserWrap::get_type_object());
  PyModule_AddObject(m, "Parser", 
    reinterpret_cast<PyObject *>(ParserWrap::get_type_object()));
}

void
PythonLoader::load(string name)
{
  PyObject *foo = PyImport_ImportModule(name.c_str());

  if(foo == NULL)
    PyErr_Print();
}
