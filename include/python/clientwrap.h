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

#ifndef CLIENTWRAP_H_INC
#define CLIENTWRAP_H_INC

#include "Python.h"
#include "baseclient.h"
#include "python/channelwrap.h"
#include "python/eventwrap.h"

class ChannelWrap;

class ClientWrap : public PythonWrap<ClientWrap>
{
private:
  static ClientWrap *me;
  static PyObject *connected;
  static PyObject *registering;
  static PyObject *disconnected;
  static PyObject *closing;
  static PyObject *nick_changing;
  static PyObject *nick_changed;
  
  ClientPtr client;
public:
  // Non Python methods
  static void init(PyObject *);

  // Event callbacks
  static bool on_connected(ClientPtr);
  static bool on_disconnected(ClientPtr);
  static bool on_registering(ClientPtr);
  static bool on_closing(ClientPtr, string);
  static bool on_nick_changing(ClientPtr, irc_string);
  static bool on_nick_changed(ClientPtr, string);

  // Event fires
  static PyObject *fire_connected(EventWrap *, PyObject *);
  static PyObject *fire_registering(EventWrap *, PyObject *);
  static PyObject *fire_disconnected(EventWrap *, PyObject *);
  static PyObject *fire_closing(EventWrap *, PyObject *);
  static PyObject *fire_nick_changing(EventWrap *, PyObject *);
  static PyObject *fire_nick_changed(EventWrap *, PyObject *);

  // Get/Setters
  static PyObject *get_wrap(ClientWrap *, void *);
  static int set_wrap(ClientWrap *, PyObject *, void *);
  static PyObject *get_me() { return me; }

  // Python methods
  static PyObject *add(ClientWrap *, ClientWrap *);
  static PyObject *add_name(PyObject *, ClientWrap *);
  static PyObject *close(ClientWrap *, PyObject *);
  static int compare(ClientWrap *, ClientWrap *);
  static PyObject *del_name(PyObject *, ClientWrap *);
  static PyObject *find_by_name(PyObject *, PyObject *);
  static PyObject *is_registered(ClientWrap *, PyObject *);
  static PyObject *is_ssl(ClientWrap *, PyObject *);
  static PyObject *numeric(ClientWrap *, PyObject *);
  static PyObject *remove_channel(ClientWrap *, ChannelWrap *);
  static PyObject *send(ClientWrap *, PyObject *, PyObject *);
  static PyObject *send_channels_common(ClientWrap *, PyObject *, PyObject *);
  static PyObject *str(ClientWrap *);

  // ctor/dtor
  ClientWrap(PyObject *, PyObject *);
  ~ClientWrap();

  // members
  inline ClientPtr get_client() const { return client; }
};

#endif
