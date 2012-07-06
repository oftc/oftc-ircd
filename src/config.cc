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
#include <stdexcept>
#include <map>
#include "logging.h"
#include "config.h"
#include "configsection.h"

// Initialise the static member
std::map<std::string, ConfigSection *> Config::sections;

void
Config::init(const char *path)
{
  std::ifstream config(path);
  Json::Value root;
  Json::Reader reader;

  std::map<std::string, ConfigSection *>::iterator it;
  for(it = Config::sections.begin(); it != Config::sections.end(); it++)
  {
    it->second->set_defaults();
  }

  if(!reader.parse(config, root))
  {
    throw std::runtime_error("Error processing configuration file: " + 
      reader.getFormattedErrorMessages());
  }

  if(root.type() == Json::objectValue)
  {
    Json::Value::Members members = root.getMemberNames();

    for(Json::Value::Members::iterator it = members.begin(); 
        it != members.end(); it++)
    {
      const std::string &name = *it;
      ConfigSection *section = Config::sections[name];

      if(section == NULL)
        std::cerr << "Unknown config section: " << name << std::endl;
      else
        section->process(root[name]);
    }
  }
}

void 
Config::add_section(const std::string& name, ConfigSection* const section)
{
  Config::sections.insert(std::pair<std::string, ConfigSection *>(name, section));
}
