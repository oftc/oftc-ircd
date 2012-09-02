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
#include "python/pobject.h"
#include "python/pstring.h"
#include "python/pparser.h"
#include "python/pchannel.h"
#include "python/pclient.h"
#include "python/pevent.h"
#include "python/pcollection.h"
#include "python/pmembership.h"
#include "module.h"
#include "system.h"

using std::stringstream;
using std::string;
using std::cerr;
using std::endl;

static PyMethodDef module_methods[] =
{
  { "get_motd", PythonUtil::get_motd, METH_VARARGS, "Return the MOTD for the server" },
  { NULL, NULL, 0, NULL }
};


void PythonUtil::init_python()
{
  PyObject *module;
  Py_InitializeEx(0);

  module = Py_InitModule3("pythonwrap", module_methods, "Wrapper module for oftc-ircd C(++) interface");

  if(module == NULL)
  {
    PythonUtil::log_error();
    throw runtime_error("Error initialising python");
  }

  PParser::init(module);
  PChannel::init(module);
  PClient::init(module);
  PEvent::init(module);
  PMembership::init(module);
  PNuhMask::init(module);
  PCollection::init(module);
}
  
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

PObject PythonUtil::send_format(PObject source, PTuple args, PDict kwargs)
{
  PString format = static_cast<PString>(args[0]);
  PDict fdict;
  PObject method;
  PTuple fargs(0);

  if(!PString::check(format))
    return PException(PyExc_TypeError, "you must pass a string as the first parameter");

  if(!PChannel::check(source))
    fdict.set_item("client", source);
  else
    fdict.set_item("channel", source);

  fdict.set_item("me", PClient::get_me());

  if (kwargs)
    fdict.update(kwargs);

  method = format.getattr("format");
  return PString(method(fargs, fdict));
}
