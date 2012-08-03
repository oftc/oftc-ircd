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
#include "generalsection.h"
#include "config.h"

void GeneralSection::set_defaults()
{
  daemon = true;
  server_info = "IRC Server";
}

void GeneralSection::process(const Json::Value value)
{
  if(!value["daemon"].isNull())
    daemon = value["daemon"].asBool();
  if(!value["ssl_certificate"].isNull())
    ssl_certificate = value["ssl_certificate"].asString();
  if(!value["ssl_privatekey"].isNull())
    ssl_privatekey = value["ssl_privatekey"].asString();
  if(!value["messages_file"].isNull())
    messages_file = value["messages_file"].asString();
  if(!value["server_name"].isNull())
    server_name = value["server_name"].asString().c_str();
  if(!value["server_info"].isNull())
    server_info = value["server_info"].asString();
}

void GeneralSection::verify() const
{
  if(ssl_certificate.empty() || !Config::check_file_exists(ssl_certificate))
    Logging::warning << "SSL Certificate file is missing or incorrect" << Logging::endl;
  if(ssl_privatekey.empty() || !Config::check_file_exists(ssl_privatekey))
    Logging::warning << "SSL Private key file is missing or incorrect" << Logging::endl;

  if(messages_file.empty() || !Config::check_file_exists(messages_file))
    throw config_error("Messages file is not configured or the file is missing");

  if(server_name.empty())
    throw config_error("Server name must be configured");
}
