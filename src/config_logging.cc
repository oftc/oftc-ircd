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
#include <json.h>
#include <string.h>
#include "logging.h"
#include "config.h"
#include "config_logging.h"

struct config_section_entry logging_section_entries[] = 
{
  { CONFIG_SECTION_ENTRY(logging, log_path, json_type_string) },
  { CONFIG_SECTION_ENTRY(logging, log_level, json_type_string) },
  { CONFIG_SECTION_ENTRY_END }  
};

struct config_logging logging_config = { 0 };

/* internal functions */

/* external functions */
void config_logging_section_init()
{
}

void config_logging_section_set_defaults()
{
  logging_config.log_path = strdup(DEFAULT_LOG_PATH);
  logging_config.log_level = strdup(DEFAULT_LOG_LEVEL);
}

void config_logging_section_process(void *obj)
{
  json_object *jobj = (json_object *)obj;

  config_section_process(jobj, (char*)&logging_config, logging_section_entries);
}

void config_logging_section_validate()
{
  logging_config.min_loglevel = logging_string_to_level(logging_config.log_level);
}

