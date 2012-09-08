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

typedef function<PObject(const PTuple&)> EventCallback;

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

  PEvent(const PTuple&, const PDict&) : listeners(Py_None), handler(Py_None)
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
    int ret;

    ret = PCType::set(prop, value);

    if(ret != 0)
      return -1;
    switch(prop.number)
    {
    case Listeners:
      listeners = value;
      break;
    case Handler:
      handler = value;
      break;
    }

    return ret;
  }

  void set_func(EventCallback func)
  {
    callback = func;
  }

  PObject fire(const PTuple& args)
  {
    return callback(args);
  }

  PObject channel_client_callback(Event<ChannelPtr, ClientPtr>& event, const PTuple& args)
  {
    PChannel *channel = args[0].AsPtr<PChannel>();
    PClient *client = args[1].AsPtr<PClient>();

    PBool ret(event(*channel, *client));
    
    return ret.incref();
  }

  PObject client_string_callback(Event<ClientPtr, string>& event, const PTuple& args)
  {
    PClient *client = args[0].AsPtr<PClient>();
    PString str = args[1].As<PString>();
    PBool ret(event(*client, str.c_str()));

    return ret.incref();
  }

  PObject client_ircstring_callback(Event<ClientPtr, irc_string>& event, const PTuple& args)
  {
    PClient *client = args[0].AsPtr<PClient>();
    PString str = args[1].As<PString>();
    PBool ret(event(*client, str.c_str()));

    return ret.incref();
  }

//#ifndef HAS_VARTEMPLATES
#if 1
  template<class T1>
  static void add_event(PyObject *dict, const char *name, Event<T1>& event, PObject(PEvent::*callback)(Event<T1>, const PTuple&), function<bool(T1)> handler)
  {
    PEvent *e = new PEvent();

    auto func = std::bind(callback, e, std::ref(event), _1);
    e->set_func(func);

    PyDict_SetItemString(dict, name, e);

    event += handler;
  }

  template<class T1, class T2>
  static void add_event(PyObject *dict, const char *name, Event<T1, T2>& event, PObject(PEvent::*callback)(Event<T1, T2>&, const PTuple&), function<bool(T1, T2)> handler)
  {
    PEvent *e = new PEvent();

    auto func = std::bind(callback, e, std::ref(event), _1);
    e->set_func(func);

    PyDict_SetItemString(dict, name, e);

    event += handler;
  }
#else
  template<typename... T>
  static void add_event(PyObject *dict, const char *name, Event<T...> event, PObject(PEvent::*callback)(Event<T...>, const PTuple&), function<bool(T...)> handler)
  {
    PEvent *e = new PEvent();

    auto func = std::bind(callback, e, std::ref(event), _1);
    e->set_func(func);

    PyDict_SetItemString(dict, name, e);

    event += handler;
  }
#endif

  static void init(const PObject& module)
  {
    PyTypeObject& type = type_object();

    type.tp_name = "Event";

    add_method("fire", "Fire the event", VarArgsMethod(&PEvent::fire));
    add_property("listeners", "Functions listening to this event", Listeners, static_cast<PropertyFlag>(0));
    add_property("handler", "handler for the event", Handler, static_cast<PropertyFlag>(0));

    PCType::init(module);
  }

  static bool handle(const PObject& ev, const PTuple& args)
  {
    PEvent *event = ev.AsPtr<PEvent>();
    PObject ret;

    if(!PyCallable_Check(event->handler))
      return true;

    ret = event->handler(args);

    if(!ret)
    {
      PythonUtil::log_error();
      return true;
    }

    return ret.is_true();
  }
};

#endif
