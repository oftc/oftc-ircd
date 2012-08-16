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

#ifndef PYTHONWRAP_H_INC
#define PYTHONWRAP_H_INC

#include "Python.h"
#include "python/pythonbase.h"
#include "python/pythonutil.h"

template <class Outer, class Inner> class PythonWrap : public PythonBase
{
protected:
  Inner wrapped;
public:
  PythonWrap(PyObject *args, PyObject *kwds)
  {
    PyObject *obj;
    Inner ptr;

    if(PyTuple_Size(args) == 0)
      return;

    if(!PyArg_ParseTuple(args, "O", &obj))
    {
      PythonUtil::log_error();
      return;
    }

    ptr = *(reinterpret_cast<Inner *>(PyCObject_AsVoidPtr(obj)));
    wrapped = ptr;

    Logging::trace << "Created PythonWrap: " << this << Logging::endl;
  }

  const Inner& get_wrapped() const
  {
    return wrapped;
  }

  static PyObject *create(PyTypeObject *type, PyObject *args, PyObject *kwds)
  {
    PyObject *ptr = type->tp_alloc(type, 0);
    Outer *obj = new(ptr) Outer(args, kwds);

    return reinterpret_cast<PyObject*>(obj);
  }

  static void dealloc(PyObject *obj)
  {
    Outer *ptr = reinterpret_cast<Outer *>(obj);
    ptr->~Outer();
    obj->ob_type->tp_free(obj);
  }

  static void init(const char *name)
  {
    PyTypeObject& type = type_object();

    type.tp_name = name;
    type.tp_new = create;
    type.tp_dealloc = dealloc;

    PythonBase::init();
  }

/*  static Wrap *wrap(void *arg)
  {
    PyObject *obj, *args;
    Wrap *wrapped;

    obj = PyCObject_FromVoidPtr(arg, NULL);

    args = Py_BuildValue("(O)", obj);

    wrapped = call(args);
    Py_DECREF(obj);
    if(wrapped == NULL)
      return NULL;

    return wrapped;
  }*/
};

#endif
