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
#include "pobject.h"
#include "pclient.h"
#include "pchannel.h"
#include "event.h"

using namespace std::placeholders;

typedef function<PObject(PTuple)> EventCallback;

enum EventProperties
{
  Listeners,
  Handler
};

class PEvent : public PCType<PEvent, EventCallback>
{
private:
  PObject listeners;
  PObject handler;
  EventCallback callback;
public:
  PEvent() : listeners(Py_None), handler(Py_None)
  {
  }

  PEvent(PTuple, PDict) : listeners(Py_None), handler(Py_None)
  {
  }

  ~PEvent()
  {
  }

  PObject get(const Property prop)
  {
    PObject value;

    switch(prop.number)
    {
    case Listeners:
      value = listeners;
      break;
    case Handler:
      value = handler;
      break;
    }

    return value;
  }

  int set(const Property prop, const PObject& value)
  {
    switch(prop.number)
    {
    case Listeners:
      listeners = value;
      break;
    case Handler:
      handler = value;
      break;
    }

    return 0;
  }

  void set_func(EventCallback func)
  {
    callback = func;
  }

  PObject fire(PTuple args)
  {
    return callback(args);
  }

  PObject channel_client_callback(Event<ChannelPtr, ClientPtr> event, PTuple args)
  {
    PyObject *arg0, *arg1;

    arg0 = args[0];
    arg1 = args[1];

    PChannel *channel = static_cast<PChannel *>(arg0);
    PClient *client = static_cast<PClient *>(arg1);

    PBool ret(event(*channel, *client));
    
    return ret.incref();
  }

  PObject client_string_callback(Event<ClientPtr, string> event, PTuple args)
  {
    PyObject *obj = args[0];
    PClient *client = static_cast<PClient *>(obj);
    PString str = static_cast<PString>(args[1]);
    PBool ret(event(*client, str.c_str()));

    return ret.incref();
  }

  PObject client_ircstring_callback(Event<ClientPtr, irc_string> event, PTuple args)
  {
    PyObject *obj = args[0];
    PClient *client = static_cast<PClient *>(obj);
    PString str = static_cast<PString>(args[1]);
    PBool ret(event(*client, str.c_str()));

    return ret.incref();
  }

#ifdef _MSC_VER
  template<class T1, class T2, class T3, class T4, class T5>
  static void add_event(PyObject *dict, const char *name, Event<T1, T2, T3, T4, T5> event, PObject(PEvent::*callback)(Event<T1, T2, T3, T4, T5>, PTuple))
#else
  template<typename... T>
  static void add_event(PyObject *dict, const char *name, Event<T...> event, PObject(PEvent::*callback)(Event<T...>, PTuple))
#endif
  {
    PEvent *e = new PEvent();

    auto func = std::bind(callback, e, event, _1);
    e->set_func(func);

    PyDict_SetItemString(dict, name, e);
  }

  static void init(const PObject& module)
  {
    PyTypeObject& type = type_object();

    type.tp_name = "Event";

    add_method("fire", "Fire the event", VarArgsMethod(&PEvent::fire));
    add_property("listeners", "Functions listening to this event", Listeners, static_cast<PropertyFlag>(0));
    add_property("handler", "handler for the event", Handler, static_cast<PropertyFlag>(0));

    PCType::init(module);
  }
};

#endif
