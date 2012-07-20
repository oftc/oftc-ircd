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
#include "client.h"

class ClientWrap : public PythonWrap<ClientWrap>
{
private:
  ClientPtr client;
public:
  static void init();
  static PyObject *get_wrap(PyObject *, void *);
  static int set_wrap(PyObject *, PyObject *, void *);
  static PyObject *send(PyObject *, PyObject *, PyObject *);
  static PyObject *add(PyObject *, PyObject *);

  ClientWrap(PyObject *, PyObject *);
  ~ClientWrap();

  inline string get_name() const { return client->get_name(); }
  inline string get_username() const { return client->get_username(); }
  inline string get_realname() const { return client->get_realname(); }

  inline void set_name(const char *name) { client->set_name(name); }
  inline void set_username(const char *username) { client->set_username(username); }
  inline void set_realname(const char *realname) { client->set_realname(realname); }
};

#endif
