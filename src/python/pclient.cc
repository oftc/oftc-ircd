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

#include "python/pclient.h"
#include "stdinc.h"

PClient::PClient()
{
}

PClient::PClient(ClientPtr ptr) : PCType(ptr)
{
}

PClient::PClient(PTuple args, PDict kwargs)
{
}

PClient::~PClient()
{
}

// Statics

void PClient::init()
{
  PyTypeObject& type = type_object();

  add_method("is_registered", "Check is the client is registered or not", NoArgsMethod(&PClient::is_registered));
  add_method("find_by_name", "Search the client list for a named client", &find_by_name);

  type.tp_name = "Client";

  PCType::init();
}
