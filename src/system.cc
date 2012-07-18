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
#include <string.h>
#include <errno.h>
#ifndef _WIN32
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif
#include <uv.h>

#include <iostream>
#include <sstream>
#include "config.h"
#include "generalsection.h"
#include "system.h"

using std::cerr;
using std::endl;
using std::stringstream;

GeneralSection System::config;
const char *System::config_path;

void
System::init()
{
  Config::add_section("general", &config);
  config_path = CONFIG_PATH;
}

void
System::parse_args(int argc, const char * const argv[])
{
  for(int index = 1; index < argc; index++)
  {
    string str(argv[index]);

    if(str == "-c")
    {
      config_path = argv[index + 1];
      index++;
    }
    else
    {
      cerr << "Unknown option: " << str << endl;
    }
  }
}

string
System::perror(const char *error)
{
  string str = error;

  str += ": ";
  str += strerror(errno);

  return str;
}

string
System::uv_perror(const char *error)
{
  stringstream ss;

  ss << error << ": " << uv_strerror(uv_last_error(uv_default_loop()));

  return ss.str();
}

#ifndef _WIN32
void
System::daemonize()
{
  pid_t pid, sid;

  if(getppid() == 1)
    return;

  pid = fork();

  if(pid < 0)
    throw runtime_error(System::perror("Error creating daemon"));

  if(pid > 0)
    exit(0);

  umask(0);

  sid = setsid();
  if(sid < 0)
    throw runtime_error(System::perror("Error creating daemon"));

  if(chdir("/") < 0)
    throw runtime_error(System::perror("Error creating daemon"));

  freopen("/dev/null", "r", stdin);
  freopen("/dev/null", "w", stdout);
  freopen("/dev/null", "w", stderr);
}
#endif
