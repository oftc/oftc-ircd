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
#include "python/pmembership.h"
#include "python/pythonutil.h"
#include "python/pchannel.h"
#include "python/pclient.h"
#include "python/pint.h"

enum MembershipProperty
{
  Channel,
  Client,
  Flags,
};

PMembership::PMembership(PTuple args, PDict kwds) : PCType(args, kwds)
{
}

PMembership::PMembership(Membership ptr) : PCType(ptr)
{
}

PMembership::~PMembership()
{
}

PObject PMembership::get(Property prop)
{
  switch(prop.number)
  {
  case Channel:
    return new PChannel(inner.channel);
  case Client:
    return new PClient(inner.client);
  case Flags:
    return PInt(inner.flags);
  }

  return PObject::None();
}

int PMembership::set(const Property prop, const PObject& value)
{
  return 0;
}

// Statics

void PMembership::init(PyObject *module)
{
  PyTypeObject& type = type_object();

  add_property("Channel", "Channel link", Channel, NoFlags);
  add_property("Client", "Client link", Client, NoFlags);
  add_property("Flags", "Flags for this membership link", Flags, NoFlags);

  type.tp_name = "Membership";

  PCType::init(module);
}

