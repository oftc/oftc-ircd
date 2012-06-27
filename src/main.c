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

#include <unistd.h>
#include "stdinc.h"
#include <getopt.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

static int 
parse_args(int argc, char* const argv[])
{
  char *cvalue = NULL;
  int index;
  int c;

  while((c = getopt (argc, argv, "")) != -1)
  {
    switch(c)
    {
      case '?':
        if(isprint(optopt))
          fprintf(stderr, "Unknown option '-%c'.\n", optopt);
        break;
      default:
        perror("Error processing command line arguments");
        return FALSE;
    }
  }

  return TRUE;
}

static int
daemonize()
{
  pid_t pid, sid;

  if(getppid() == 1)
    return TRUE;

  pid = fork();

  if(pid < 0)
    return FALSE;

  if(pid > 0)
    exit(EXIT_SUCCESS);

  umask(0);

  sid = setsid();
  if(sid < 0)
    return FALSE;

  if(chdir("/") < 0)
    return FALSE;

  freopen("/dev/null", "r", stdin);
  freopen("/dev/null", "w", stdout);
  freopen("/dev/null", "w", stderr);

  return TRUE;
}

int 
main(int argc, char *argv[])
{
  if(!parse_args(argc, argv))
  {
    perror("Parsing command line arguments failed");
    return EXIT_FAILURE;
  }

  if(!daemonize())
  {
    perror("Error making the process a daemon");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
