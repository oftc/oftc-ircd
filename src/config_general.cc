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
#include "config.h"
#include "config_general.h"

struct config_section_entry general_section_entries[] = 
{ 
//  { CONFIG_SECTION_ENTRY(general, daemon, json_type_boolean) },
  { CONFIG_SECTION_ENTRY_END }  
};

struct config_general general_config = { 0 };

/* internal functions */

/* external functions */
void config_general_section_init()
{
}

void config_general_section_set_defaults()
{
  general_config.daemon = TRUE;
}

void config_general_section_process(void *obj)
{
/*  json_object *jobj = (json_object *)obj;

  config_section_process(jobj, (char*)&general_config, general_section_entries);
*/
}

void config_general_section_validate()
{
}

