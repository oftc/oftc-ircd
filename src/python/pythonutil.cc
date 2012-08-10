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
#include <iostream>
#include "python/pythonutil.h"
#include "python/pythonwrap.h"
#include "python/parserwrap.h"
#include "python/clientwrap.h"
#include "python/eventwrap.h"
#include "python/channelwrap.h"
#include "module.h"
#include "system.h"

using std::stringstream;
using std::string;
using std::cerr;
using std::endl;
  
void PythonUtil::log_error()
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
  cerr << message << endl;
}

PyObject *PythonUtil::get_motd(PyObject *self, PyObject *arg)
{
  PyObject *ret = PyString_FromString(System::get_motd().c_str());

  return ret;
}

PyObject *PythonUtil::send_format(PyObject *source, PyObject *args, PyObject *kwargs)
{
  PyObject *fmt, *result, *meth, *fargs, *fdict;

  fmt = PyTuple_GetItem(args, 0);

  if(!PyString_Check(fmt))
  {
    PyErr_SetString(PyExc_TypeError, "you must pass a string as the first parameter");
    return NULL;
  }

  fdict = PyDict_New();
  if(!ChannelWrap::check(source))
    PyDict_SetItemString(fdict, "client", source);
  else
    PyDict_SetItemString(fdict, "channel", source);

  PyDict_SetItemString(fdict, "me", ClientWrap::get_me());

  if (kwargs != NULL)
    PyDict_Update(fdict, kwargs);

  meth = PyObject_GetAttrString(fmt, "format");

  fargs = PyTuple_New(0);
  result = PyObject_Call(meth, fargs, fdict);

  Py_DECREF(meth);
  Py_DECREF(fdict);
  Py_DECREF(fargs);

  if(result == NULL)
  {
    return NULL;
  }

  return result;
}