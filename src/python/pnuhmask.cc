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

#include <Python.h>
#include <structmember.h>
#include "stdinc.h"
#include "python/pnuhmask.h"
#include "python/pctype.h"
#include "python/pclient.h"
#include "nuhmask.h"

enum NukMaskProperties
{
  Name,
  Username,
  Host
};

PNuhMask::PNuhMask(const PTuple& args, const PDict& kwds) : PCType(args, kwds)
{
  PString mask = static_cast<PString>(args[0]);

  inner = NuhMask(mask.c_str());
}

PNuhMask::PNuhMask(NuhMask ptr) : PCType(ptr)
{
}

PNuhMask::~PNuhMask()
{
}

PObject PNuhMask::get(const Property prop)
{
  switch(prop.number)
  {
  case Name:
    return PString(inner.get_name());
  case Username:
    return PString(inner.get_username());
  case Host:
    return PString(inner.get_host());
  }
  return PObject::None();
}

int PNuhMask::set(const Property prop, const PObject& value)
{
  int ret;

  ret = PCType::set(prop, value);

  if(ret != 0)
    return -1;

  return ret;
}

PObject PNuhMask::match(const PTuple& args)
{
  PyObject *arg = args[0];
  PNuhMask *right_mask;
  PClient *right_client;
  PBool ret;

  right_mask  = static_cast<PNuhMask *>(arg);

  if(PNuhMask::check(right_mask))
    ret = PBool(inner.match(right_mask->inner));
  else
  {
    right_client = static_cast<PClient *>(arg);
    if(!PClient::check(right_client)) {
      PyErr_SetString(PyExc_TypeError, "argument must be NuhMask or Client");
      return NULL;
    }
    ret = PBool(inner.match(*right_client));
  }

  return ret;
}

// Statics

void PNuhMask::init(const PObject& module)
{
  PyTypeObject& type = type_object();

  type.tp_name = "NuhMask";

  add_method("match", "Match another nuhmask against this", VarArgsMethod(&PNuhMask::match));

  add_property("Name", "Nickname part of the mask", Name, StringArg);
  add_property("Username", "Username part of the mask", Username, StringArg);
  add_property("Host", "Host part of the mask", Host, StringArg);

  PCType::init(module);
}
