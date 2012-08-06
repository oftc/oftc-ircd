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
#include <sstream>
#include <string>
#include "python/pythonloader.h"
#include "python/pythonwrap.h"
#include "python/parserwrap.h"
#include "python/clientwrap.h"
#include "python/eventwrap.h"
#include "module.h"
#include "system.h"

using std::stringstream;
using std::string;

typedef vector<string>::const_iterator VectorStringConstIt;

static PyMethodDef module_methods[] =
{
  { "get_motd", PythonLoader::get_motd, METH_NOARGS, "Return the MOTD for the server" }
};

vector<PyObject *> PythonLoader::loaded_modules;

void PythonLoader::init()
{
  PyObject *m;

  Py_InitializeEx(0);

  stringstream path;

  path << Py_GetPath();
  vector<string> paths = Module::get_module_paths();

  for(VectorStringConstIt it = paths.begin(); it != paths.end(); it++)
  {
#ifdef _WIN32
    path << ";" << *it;
#else
    path << ":" << *it;
#endif
  }

  PySys_SetPath(const_cast<char*>(path.str().c_str()));

  m = Py_InitModule3("pythonwrap", module_methods, 
    "Wrapper module for oftc-ircd C(++) interface");

  if(m == NULL)
  {
    log_error();
    throw runtime_error("Error initialising python");
  }

  ParserWrap::init(m);
  EventWrap::init(m);
  ClientWrap::init(m);
}

void PythonLoader::load(string name)
{
  PyObject *module = PyImport_ImportModule(name.c_str());

  if(module == NULL)
  {
    log_error();
    return;
  }

  loaded_modules.push_back(module);
}
  
void PythonLoader::log_error()
{
  PyObject *module, *attr, *ret;
  PyObject *type, *value, *traceback;
  PyObject *arg, *str;
  string message = "Python error: ";

  if(!PyErr_Occurred())
    return;

  PyErr_Fetch(&type, &value, &traceback);
  PyErr_NormalizeException(&type, &value, &traceback);

  module = PyImport_ImportModule("traceback");
  attr = PyObject_GetAttrString(module, "format_exception");

  if(traceback == NULL)
    arg = Py_BuildValue("(OOO)", type, value, Py_None);
  else
    arg = Py_BuildValue("(OOO)", type, value, traceback);
    

  ret = PyObject_CallObject(attr, arg);

  int size = PyList_Size(ret);

  for(int i = 0; i < size; i++)
  {
    str = PyList_GetItem(ret, i);
    message += PyString_AsString(str);
  }

  Py_DECREF(arg);
  Py_DECREF(attr);
  Py_DECREF(module);

  Logging::error << message << Logging::endl;
}

PyObject *PythonLoader::get_motd(PyObject *self, PyObject *arg)
{
  PyObject *ret = PyString_FromString(System::get_motd().c_str());

  return ret;
}
