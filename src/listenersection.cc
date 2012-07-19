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

#include "stdinc.h"
#include <json/json.h>
#include <limits.h>
#include "listenersection.h"
#include "listener.h"

void
ListenerSection::set_defaults()
{
}

void
ListenerSection::process(const Json::Value value)
{
  if(value.type() != Json::arrayValue)
    throw runtime_error("listener section not an array as expected");

  for(Json::Value::const_iterator it = value.begin(); it != value.end(); it++)
  {
    Json::Value val = *it;
    string host(val["host"].asString());
    int port;
    int flags = 0;

    if(!val["port"])
      port = Listener::DEFAULT_PORT;
    else
      port = val["port"].asInt();

    if(!val["ssl"].isNull() && val["ssl"].asBool())
      flags |= Listener_SSL;

    if(port < 0 || port > USHRT_MAX)
    {
      Logging::warning << "Ignoring listener, port " << port << " is invalid" <<
        Logging::endl;
      continue;
    }

    if(host.length() == 0)
    {
      Listener::create("::", port, static_cast<ListenerFlag>(flags));
      Listener::create("0.0.0.0", port, static_cast<ListenerFlag>(flags));
    }
    else
      Listener::create(host, port, static_cast<ListenerFlag>(flags));
  }
}

void
ListenerSection::verify() const
{
}
