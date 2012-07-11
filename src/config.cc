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

typedef map<string, ConfigSection *>::const_iterator ConfigSectionConstIt;

// Initialise the static member
map<string, ConfigSection *> Config::sections;

// Statics
void
Config::init(const string path)
{
  ifstream config(path.c_str());
  Json::Value root;
  Json::Reader reader;

  for(ConfigSectionConstIt it = sections.begin(); it != sections.end(); it++)
  {
    it->second->set_defaults();
  }

  if(!reader.parse(config, root))
    throw runtime_error(reader.getFormattedErrorMessages());

  if(root.type() != Json::objectValue)
    throw runtime_error("Root node is not an object");

  Json::Value::Members members = root.getMemberNames();
  Json::Value::Members::const_iterator mit;

  for(mit = members.begin(); mit != members.end(); mit++)
  {
    const string name = *mit;
    ConfigSection *section = sections[name];

    if(section == NULL)
      cerr << "Unknown config section: " << name << endl;
    else
      section->process(root[name]);
  }
}

void 
Config::add_section(const string name, ConfigSection* const section)
{
  sections[name] = section;
}
