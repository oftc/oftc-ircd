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
#include "python/pythonbase.h"
#include "python/pythonutil.h"

PyObject *PythonBase::alloc(PyTypeObject *type, Py_ssize_t items)
{
  char *ret;
  PyObject *obj;

  ret = new char[_PyObject_VAR_SIZE(type, items)]();

  obj = reinterpret_cast<PyObject *>(ret);
  static_cast<void>(PyObject_INIT(obj, type));

  Logging::trace << "Created Python object: " << type->tp_name << " at: " << static_cast<void *>(obj) << Logging::endl;

  return obj;
}

void PythonBase::free(void *ptr)
{
  char *obj = static_cast<char *>(ptr);

  Logging::trace << "Destroyed python object: " << ptr << Logging::endl;

  delete[] obj;
}

void PythonBase::init()
{
  PyTypeObject& type = type_object();

  type.ob_refcnt = 1;
  type.tp_alloc = alloc;
  type.tp_free = free;
  type.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;

  if(!PyType_Ready(&type))
  {
    PythonUtil::log_error();
    return;
  }
}

PyTypeObject& PythonBase::type_object()
{
  static PyTypeObject type;

  return type;
}
