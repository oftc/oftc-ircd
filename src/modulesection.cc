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
#include "modulesection.h"
#include "module.h"
#include "config.h"

void ModuleSection::set_defaults()
{
}

void ModuleSection::process(const Json::Value value)
{
  if(!value.isObject())
    throw config_error("module config section wasn't an object as expected");

  Json::Value::Members members = value.getMemberNames();

  for(auto mit = members.cbegin(); mit != members.cend(); mit++)
  {
    const string name = *mit;
    if(name == "paths")
    {
      Json::Value paths = value[name];

      if(!paths.isArray())
        throw config_error("module paths section was not a list as expected");

      for(Json::Value::iterator it = paths.begin(); it != paths.end(); it++)
      {
        Json::Value path = *it;

        search_paths.push_back(path.asString());
      }
    }
    else if(name == "load")
    {
      Json::Value modules = value[name];

      if(!modules.isObject())
        throw config_error("module config section wasn't an object as expected");

      Json::Value::Members module_members = modules.getMemberNames();

      for(auto m_mit = module_members.begin(); m_mit != module_members.end(); m_mit++)
      {
        const string name = *m_mit;
        Json::Value module = modules[name];
        Module::create(name, module.asString());
      }
    }
  }
}

void ModuleSection::verify() const
{
}
