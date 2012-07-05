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

#ifndef CONFIG_H_INC
#define CONFIG_H_INC

#define MAX_CONFIG_SECTION_NAME 64
#define MAX_CONFIG_SECTION_ENTRY_NAME 128
#define CONFIG_PATH "/home/stu/oircd/etc/ircd.conf"

#define ssizeof(t, m) sizeof(((t *)0)->m)

#define CONFIG_SECTION(name) #name, config_##name##_section_init, config_##name##_section_process, config_##name##_section_validate, config_##name##_section_set_defaults
#define CONFIG_SECTION_END "", NULL, NULL, NULL, NULL
#define CONFIG_SECTION_ENTRY(section, name, type) #name, type, offsetof(struct config_##section, name), ssizeof(struct config_##section, name)
#define CONFIG_SECTION_ENTRY_END "", 0, 0, 0

typedef void config_process_cb(void *);
typedef void config_function_cb(void);

struct config_section
{
  char name[MAX_CONFIG_SECTION_NAME];
  config_function_cb *config_section_init;
  config_process_cb *config_section_process;
  config_function_cb *config_section_validate;
  config_function_cb *config_section_set_defaults;
};

struct config_section_entry
{
  char name[MAX_CONFIG_SECTION_ENTRY_NAME];
  int type;
  int offset;
  int length;
};

int config_init();
void config_section_process(void *, char *, struct config_section_entry *);

#endif
