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
#include "config.h"

struct config_section config_sections[] = {
  { CONF_SECTION_END }
};

void
config_init()
{
}

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

static void
section_process(void *obj, char *ptr, struct config_section_entry *entry_list)
{
  json_object_object_foreach(obj, key, value)
  {
    struct config_section_entry *entry;
    json_type obj_type = json_object_get_type(value);

    entry = find_section_entry(entry_list, key);

    if(entry == NULL)
    {
      continue;
    }

    if(obj_type != entry->type)
    {
      continue;
    }

    switch(obj_type)
    {
      case json_type_int:
      {
        int intval = json_object_get_int(value);

        memcpy(ptr + entry->offset, &intval, entry->length);
        break;
      }
      case json_type_boolean:
      {
        int boolval = json_object_get_boolean(value);

        memcpy(ptr + entry->offset, &boolval, entry->length);
        break;
      }
      case json_type_string:
      {
        char *strval;
        char *addr;

        strval = ptr + entry->offset;
        addr = (char*)(*(int*)strval);

        free(addr);

        strval = strdup(json_object_get_string(value));
        memcpy(ptr + entry->offset, &strval, entry->length);
        break;
      }
      default:
        break;
    }
  }
}
