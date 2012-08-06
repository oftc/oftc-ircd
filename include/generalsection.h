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

#ifndef GENERALSECTION_H_INC
#define GENERALSECTION_H_INC

#include <string>
#include <json/json.h>
#include "configsection.h"
#include "ircstring.h"

using std::string;

class GeneralSection : public ConfigSection
{
private:
  bool daemon;
  string ssl_certificate;
  string ssl_privatekey;
  string messages_file;
  irc_string server_name;
  string server_info;
  unsigned int nicklen;
  string motd;
public:
  void set_defaults();
  void process(const Json::Value);
  void verify() const;

  inline bool get_daemon() const { return daemon; }
  inline string get_ssl_certificate() const { return ssl_certificate; }
  inline string get_ssl_privatekey() const { return ssl_privatekey; }
  inline string get_messages_file() const { return messages_file; }
  inline unsigned int get_nicklen() const { return nicklen; }
  inline irc_string get_server_name() const { return server_name; }
  inline string get_server_info() const { return server_info; }
  inline string get_motd() const { return motd; }
};

#endif
