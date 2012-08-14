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
#include "python/pythonloader.h"
#include "python/pythonwrap.h"
#include "python/parserwrap.h"
#include "python/clientwrap.h"
#include "python/eventwrap.h"
#include "python/channelwrap.h"
#include "python/pythonutil.h"
#include "python/collectionwrap.h"
#include "python/membershipwrap.h"
#include "python/channelmaskwrap.h"
#include "channel.h"
#include "module.h"
#include "system.h"
#include "client.h"

using std::stringstream;
using std::string;
using std::cerr;
using std::endl;

static PyMethodDef module_methods[] =
{
  { "get_motd", PythonUtil::get_motd, 0, "Return the MOTD for the server" },
  PY_METHOD_END
};

PyObjectList PythonLoader::loaded_modules;
template<> PyTypeObject PythonWrap<CollectionWrap<ChannelMemberList, MembershipWrap>, ChannelMemberList>::type_object = {};
template<> PyTypeObject PythonWrap<CollectionWrap<ClientMemberList, MembershipWrap>, ClientMemberList>::type_object = {};
template<> PyTypeObject PythonWrap<MaskListWrap, ChannelMask>::type_object = {};

void PythonLoader::init()
{
  PyObject *m;

  Py_InitializeEx(0);

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

  m = Py_InitModule3("pythonwrap", module_methods, 
    "Wrapper module for oftc-ircd C(++) interface");

  if(m == NULL)
  {
    PythonUtil::log_error();
    throw runtime_error("Error initialising python");
  }

  ParserWrap::init(m);
  EventWrap::init(m);
  ClientWrap::init(m);
  ChannelWrap::init(m);
  MembershipWrap::init(m);
  CollectionWrap<ChannelMemberList, MembershipWrap>::init(m);
  CollectionWrap<ClientMemberList, MembershipWrap>::init(m);
  MaskListWrap::init(m);
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
