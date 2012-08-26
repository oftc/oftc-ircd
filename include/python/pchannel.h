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

#ifndef PCHANNEL_H_INC
#define PCHANNEL_H_INC

#include "Python.h"
#include "python/pctype.h"
#include "channel.h"
#include "python/pbool.h"

class PChannel : public PCType<PChannel, ChannelPtr>
{
public:
  PChannel();
  PChannel(PTuple, PDict);
  PChannel(ChannelPtr);
  ~PChannel();

  PObject add_member(PTuple);
  PObject is_member(PTuple);
  PObject remove_member(PTuple);
  PObject send_names(PTuple);
  PObject send(PTuple, PDict);
  PObject set_mode_char(PTuple);

  PString str();
  PObject get(Property);
  int set(Property, PObject);

  static void init(const PObject&);
  static PyObject *add(PyObject *, PyObject *);
  static PyObject *del(PyObject *, PyObject *);
  static PyObject *find(PyObject *, PyObject *);
  static PyObject *supported_modes(PyObject *, PyObject *);
};

#endif
