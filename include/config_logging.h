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

#ifndef CONFIG_LOGGING_H_INC
#define CONFIG_LOGGING_H_INC

struct config_logging
{
  char *log_path;
  char *log_level;

  /* internal members */
  log_levels min_loglevel;
};

extern struct config_logging logging_config;

void config_logging_section_init();
void config_logging_section_process(void *);
void config_logging_section_validate();
void config_logging_section_set_defaults();

#define CONFIG_LOGGING_SECTION_ENTRY(name, type) #name, type, offsetof(struct config_logging, name), ssizeof(struct config_logging, name)

#define DEFAULT_LOG_PATH LOCALSTATEDIR "/log/ircd.log"
#define DEFAULT_LOG_LEVEL "debug"

#endif
