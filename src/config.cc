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
#include <json/json.h>
#include <iostream>
#include <fstream>
#include "config.h"
#include "configsection.h"

using std::ifstream;
using std::cerr;
using std::endl;

// Initialise the static member
ConfigSectionHash Config::sections;

// Statics
void Config::init(const string path)
{
  ifstream config(path.c_str());
  Json::Value root;
  Json::Reader reader;

  if(!config.is_open())
    throw config_error(string("Failed to open config: ") + path);

  for(auto it = sections.cbegin(); it != sections.cend(); it++)
  {
    it->second->set_defaults();
  }

  if(!reader.parse(config, root))
    throw config_error(reader.getFormattedErrorMessages());

  if(root.type() != Json::objectValue)
    throw config_error("Root node is not an object");

  Json::Value::Members members = root.getMemberNames();

  for(auto mit = members.cbegin(); mit != members.cend(); mit++)
  {
    const string name = *mit;
    ConfigSection *section = sections[name];

    if(section == NULL)
      cerr << "Unknown config section: " << name << endl;
    else
      section->process(root[name]);
  }
}

void Config::add_section(const string name, ConfigSection* const section)
{
  sections[name] = section;
}

bool Config::check_file_exists(const string path)
{
  ifstream file(path.c_str());

  if(!file.is_open())
    return false;

  file.close();

  return true;
}
