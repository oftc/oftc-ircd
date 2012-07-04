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
#include <json.h>
#include "logging.h"
#include "config.h"
#include "config_logging.h"
#include "config_general.h"

struct config_section config_sections[] = {
  { CONFIG_SECTION(general) },
  { CONFIG_SECTION(logging) },
  { CONFIG_SECTION_END }
};

/* internal functions */

static struct config_section *
find_section(const char *name)
{
  struct config_section *section;

  section = config_sections;

  while(section->name[0] != '\0')
  {
    if(strncmp(section->name, name, sizeof(section->name)) == 0)
      return section;
    section++;
  }

  return NULL;
}

static struct config_section_entry *
find_section_entry(struct config_section_entry *list, const char *name)
{
  struct config_section_entry *entry = list;

  while(entry->name[0] != '\0')
  {
    if(strncmp(entry->name, name, sizeof(entry->name)) == 0)
        return entry;
    entry++;
  }

  return NULL;
}

/* external functions */

void
config_section_process(void *obj, char *ptr, struct config_section_entry *entry_list)
{
  json_object_object_foreach((struct json_object *)obj, key, value)
  {
    struct config_section_entry *section_entry;
    json_type obj_type = json_object_get_type(value);

    section_entry = find_section_entry(entry_list, key);

    if(section_entry == NULL)
    {
      continue;
    }

    if(obj_type != section_entry->type)
    {
      continue;
    }

    switch(obj_type)
    {
      case json_type_int:
      {
        int intval = json_object_get_int(value);

        memcpy(ptr + section_entry->offset, &intval, section_entry->length);
        break;
      }
      case json_type_boolean:
      {
        int boolval = json_object_get_boolean(value);

        memcpy(ptr + section_entry->offset, &boolval, section_entry->length);
        break;
      }
      case json_type_string:
      {
        char *strval;
        char *addr;

        strval = ptr + section_entry->offset;
        addr = (char*)(*(int*)strval);

        free(addr);

        strval = strdup(json_object_get_string(value));
        memcpy(ptr + section_entry->offset, &strval, section_entry->length);
        break;
      }
      default:
        break;
    }
  }
}

int
config_init()
{
  json_object *config_object;
  json_type conftype;
  struct config_section *section = config_sections;

  while(section->name[0] != '\0')
  {
    (section->config_section_set_defaults)();
    section++;
  }

  config_object = json_object_from_file(CONFIG_PATH);
  if(config_object == NULL)
  {
    return FALSE;
  }

  conftype = json_object_get_type(config_object);
  if(conftype != json_type_object)
  {
    return FALSE;
  }

  json_object_object_foreach(config_object, key, value)
  {
    struct config_section *section;

    section = find_section(key);

    if(section == NULL)
    {
      return FALSE;
    }

    (section->config_section_init)();

    (section->config_section_process)(value);
    (section->config_section_validate)();
  }

  return TRUE;
}


