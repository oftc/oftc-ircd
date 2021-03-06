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

#ifndef SYSTEM_H_INC
#define SYSTEM_H_INC

#include <uv.h>
#include "generalsection.h"

class System 
{
private:
  static GeneralSection config;
  static const char *config_path;
  static const char *built_date;
  static ares_channel dns_channel; 

public:
#ifndef _WIN32
  static void daemonize();
#endif
  static void parse_args(int, const char* const []);
  static void init();
  static string perror(const char *);
  static string uv_perror(const char *);
  static inline bool get_daemon() { return config.get_daemon(); }
  static inline const char *get_config_path() { return config_path; }
  static inline string get_ssl_certificate() { return config.get_ssl_certificate(); }
  static inline string get_ssl_privatekey() { return config.get_ssl_privatekey(); }
  static inline string get_messages_file() { return config.get_messages_file(); }
  static inline irc_string get_server_name() { return config.get_server_name(); }
  static inline string get_server_info() { return config.get_server_info(); }
  static inline const char *get_built_date() { return built_date; }
  static inline ares_channel get_ares_channel() { return dns_channel; }
  static inline unsigned int get_nicklen() { return config.get_nicklen(); }
  static inline string get_motd() { return config.get_motd(); }
};

#endif
