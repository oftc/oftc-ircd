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
#include <stdlib.h>
#include <uv.h>
#include <iostream>
#include <stdexcept>
#include "config.h"
#include "logging.h"
#include "system.h"
#include "listener.h"

int 
main(int argc, char *argv[])
{
  uv_loop_t *uv_loop;

  uv_loop = uv_default_loop();

  try
  {
    System::parse_args(argc, argv);

    System::init();
    Logging::init();
    Listener::init();
    Config::init(CONFIG_PATH);

    Logging(INFO) << "oftc-ircd starting up";
  }
  catch(std::exception &ex)
  {
    std::cerr << "Unhandled exception: " << ex.what() << std::endl;
    return EXIT_FAILURE;
  }

  // Now that logging is setup, switch to a try catch that will log as well
  try
  {
    if(System::get_daemon())
      System::daemonize();

    Listener::start_listeners();
    uv_run(uv_loop);
  }
  catch(std::exception &ex)
  {
    Logging(CRITICAL) << "Unhandled exception: " << ex.what();
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
