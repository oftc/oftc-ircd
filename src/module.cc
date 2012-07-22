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
#include <algorithm>
#include <cctype>
#include "module.h"
#include "config.h"
#include "python/pythonloader.h"

using std::transform;

typedef vector<Module>::const_iterator ModuleConstIt;

ModuleSection Module::config;
vector<Module> Module::modules;

Module::Module()
{
}

Module::Module(string _name, string _filename) : name(_name), filename(_filename)
{
}

// Statics

void
Module::init()
{
  Config::add_section("module", &config);
}

Module
Module::create(string name, string filename)
{
  Module module = Module(name, filename);

  modules.push_back(module);

  return module;
}

void
Module::load_all()
{
  for(ModuleConstIt it = modules.begin(); it != modules.end(); it++)
  {
    Module module = *it;
    string filename = module.get_filename();
    string ext;
    size_t index = filename.find_last_of('.');

    if(index == string::npos)
      ext = "";
    else
      ext = filename.substr(index + 1);

    transform(ext.begin(), ext.end(), ext.begin(), toupper);

    if(ext == "PY")
    {
      PythonLoader::load(module.get_name());
    }
  }
}
