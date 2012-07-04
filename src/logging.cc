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
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "logging.h"
#include "config_logging.h"

#define MAX_DATE_LEN 30

static FILE *logptr;

const char *log_level_lookup[] =
{
  "DEBUG",
  "INFO",
  "NOTICE",
  "WARNING",
  "ERROR",
  "CRITICAL",
  NULL
};
/* internal functions */

/* external functions */
log_levels
logging_string_to_level(const char *name)
{
  const char **p = log_level_lookup;

  while(p != NULL)
  {
    if(strcasecmp(*p, name) == 0)
      return (log_levels)(p - log_level_lookup);
    p++;
  }

  return (log_levels)-1;
}

void
logging_log(log_levels level, const char *format, ...)
{
  char *buffer;
  char datestr[MAX_DATE_LEN];
  va_list args;
  time_t current_time;

  if(logptr == NULL)
    return;

  if(level < logging_config.min_loglevel)
    return;

  va_start(args, format);
  
  if(vasprintf(&buffer, format, args) < 0)
  {
    va_end(args);
    return;
  }

  current_time = time(NULL);

  strftime(datestr, sizeof(datestr), "%Y-%m-%d %H:%M:%S%z", localtime(&current_time));
  fprintf(logptr, "%s - (core) [%s] - %s\r\n", datestr, log_level_lookup[level], buffer);

  free(buffer);
  va_end(args);
}

int
logging_init()
{
  logptr = fopen(logging_config.log_path, "a+");

  if(logptr == NULL)
  {
    perror("logging_init: Unable to open log file for appending");
    return FALSE;
  }

  return TRUE;
}
