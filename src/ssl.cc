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
#include "ssl.h"
#include "system.h"

SSL_CTX *Ssl::context;
bool Ssl::enabled;

void Ssl::init()
{
  SSL_load_error_strings();
  SSL_library_init();

  context = SSL_CTX_new(SSLv23_method());
  if(context == NULL)
  {
    enabled = false;
    Logging::warning << "Unable to initialize SSL context: " << ERR_lib_error_string(ERR_get_error()) <<
      ". Disabling SSL." << Logging::endl;
    return;
  }

  SSL_CTX_set_options(context, SSL_OP_NO_SSLv2);
  SSL_CTX_set_options(context, SSL_OP_TLS_ROLLBACK_BUG | SSL_OP_ALL);

  string cert_file = System::get_ssl_certificate();
  if(cert_file.empty() || !SSL_CTX_use_certificate_chain_file(context, cert_file.c_str()))
  {
    enabled = false;
    Logging::warning << "Unable to load SSL certificate: " << ERR_lib_error_string(ERR_get_error()) <<
      ". Disabling SSL" << Logging::endl;
    return;
  }

  string key_file = System::get_ssl_privatekey();
  if(key_file.empty() || !SSL_CTX_use_PrivateKey_file(context, key_file.c_str(), SSL_FILETYPE_PEM))
  {
    enabled = false;
    Logging::warning << "Unable to load SSL private key: " << ERR_lib_error_string(ERR_get_error()) <<
      ". Disabling SSL" << Logging::endl;
    return;
  }

  enabled = true;
}