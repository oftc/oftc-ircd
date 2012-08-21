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

#ifndef PEVENT_H_INC
#define PEVENT_H_INC

#include "Python.h"
#include "pctype.h"
#include "event.h"

class PEventBase;

typedef function<PObject(PEventBase *, PTuple args)> EventCallback;

class PEventBase : public PCType<PEventBase, EventCallback>
{
public:
  PEventBase()
  {
  }

  PEventBase(PTuple, PDict)
  {
  }

  PEventBase(EventCallback event)
  {
    inner = event;
  }

  ~PEventBase()
  {
  }

  virtual PObject fire(PTuple args)
  {
    return inner(this, args);
  }

  virtual PObject client_string_callback(PTuple args)
  {
    // This shouldn't get called, we don't want to create events from python
    Py_RETURN_NONE;
  }

  static void init(const PObject& module)
  {
    PyTypeObject& type = type_object();

    type.tp_name = "Event";

    add_method("fire", "Fire the event", VarArgsMethod(&fire));

    PCType::init(module);
  }
};

template<class T1=NullArg, class T2=NullArg, class T3=NullArg, class T4=NullArg, class T5=NullArg>
class PEvent : public PEventBase
{
private:
  Event<T1, T2, T3, T4, T5> inner_event;
public:
  PEvent()
  {
  }

  PEvent(PTuple, PDict)
  {
  }

  PEvent(Event<T1, T2, T3, T4, T5> event, EventCallback callback)
  {
    inner_event = event;
    inner = callback;
  }

  ~PEvent()
  {
  }

  PObject client_string_callback(PTuple args)
  {
    PyObject *obj = args[0];
    PClient *client = static_cast<PClient *>(obj);
    PString str = args[1];
    PBool ret(inner_event(*client, str.c_str()));

    return ret;
  }
};

#endif
