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

#ifndef PMETHOD_H_INC
#define PMETHOD_H_INC

#include "stdinc.h"
#include "python/pobject.h"
#include "python/ptuple.h"
#include "python/pdict.h"

template<class T>
class PMethod
{
private:
  typename T::NoArgsMethod noargs;
  typename T::VarArgsMethod varargs;
  typename T::KeywordArgsMethod kwargs;

  const char *name;
  const char *doc;
  unsigned int flags;
  PyCFunction callback;
public:
  PMethod()
  {
  }

  PMethod(const char *meth_name, PyCFunction meth_callback, const char *meth_doc, typename T::NoArgsMethod arg)
  {
    noargs = arg;
    name = meth_name;
    flags = METH_VARARGS;
    callback = meth_callback;
    doc = meth_doc;
  }

  PMethod(const char *meth_name, PyCFunction meth_callback, const char *meth_doc, typename T::VarArgsMethod arg)
  {
    varags = arg;
    name = meth_name;
    flags = METH_VARARGS;
    callback = meth_callback;
    doc = meth_doc;
  }

  PMethod(const char *meth_name, PyCFunction meth_callback, const char *meth_doc, typename T::KeywordArgsMethod arg)
  {
    kwargs = arg;
    name = meth_name;
    flags = METH_KEYWORDS| METH_VARARGS;
    callback = meth_callback;
    doc = meth_doc;
  }

  PMethod(const char *meth_name, unsigned int meth_flags, PyCFunction meth_callback, const char *meth_doc)
  {
    name = meth_name;
    flags = meth_flags;
    callback = meth_callback;
    doc = meth_doc;
  }

  void fill_pymethod(PyMethodDef& def)
  {
    def.ml_name = name;
    def.ml_doc = doc;
    def.ml_flags = flags;
    def.ml_meth = callback;
  }
};

#endif
