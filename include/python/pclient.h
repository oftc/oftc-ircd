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

#ifndef PCLIENT_H_INC
#define PCLIENT_H_INC

#include "Python.h"
#include "python/pctype.h"
#include "python/pbool.h"
#include "python/pcmap.h"
#include "python/pmembership.h"
#include "python/pchannel.h"
#include "client.h"

class PChannel;
typedef PCMap<ClientMemberList, PChannel, PMembership> PClientMemberList;

class PClient : public PCType<PClient, ClientPtr>
{
private:
  static PClient *me;
public:
  PClient();
  PClient(PTuple, PDict);
  PClient(ClientPtr);
  ~PClient();

  PObject get(Property);
  int set(Property, PObject);

  PString str()
  {
    return inner->str();
  }

  inline PBool is_registered() const { return inner->is_registered(); }
  inline PBool is_ssl() const { return inner->is_ssl(); }

  PObject close(PTuple);
  PObject numeric(PTuple);
  PObject send(PTuple, PDict);
  PObject send_channels_common(PTuple, PDict);
  PObject remove_channel(PTuple);

  static PyObject *find_by_name(PyObject *, PyObject *);
  static PyObject *del_name(PyObject *, PyObject *);
  static PyObject *add_name(PyObject *, PyObject *);
  static PyObject *add(PyObject *, PyObject *);
  static void init(const PObject&);

  inline static PClient *get_me() { return me; }
};

#endif
