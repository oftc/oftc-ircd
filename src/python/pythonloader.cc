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
#include "python/pythonbase.h"
#include "python/pythonloader.h"
#include "python/pythonwrap.h"
#include "python/parserwrap.h"
#include "python/clientwrap.h"
#include "python/eventwrap.h"
#include "python/channelwrap.h"
#include "python/pythonutil.h"
#include "python/collectionwrap.h"
#include "python/membershipwrap.h"
#include "python/nuhmaskwrap.h"
#include "channel.h"
#include "module.h"
#include "system.h"
#include "client.h"

using std::stringstream;
using std::string;
using std::cerr;
using std::endl;

PyObjectList PythonLoader::loaded_modules;

void PythonLoader::init()
{
  stringstream path;

  path << Py_GetPath();
  StringList paths = Module::get_module_paths();

  for(auto it = paths.cbegin(); it != paths.cend(); it++)
  {
#ifdef _WIN32
    path << ";" << *it;
#else
    path << ":" << *it;
#endif
  }

  PySys_SetPath(const_cast<char*>(path.str().c_str()));
}

void PythonLoader::load(string name)
{
  PyObject *module = PyImport_ImportModule(name.c_str());

  if(module == NULL)
  {
    PythonUtil::log_error();
    return;
  }

  loaded_modules.push_back(module);
}
