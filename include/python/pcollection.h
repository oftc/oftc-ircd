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

#ifndef PCOLLECTION_H_INC
#define PCOLLECTION_H_INC

#include "Python.h"
#include "event.h"
#include "python/pctype.h"

class PCollection : public PCType<PCollection, NullArg>
{
public:
  PCollection()
  {
    Logging::trace << "PCollection: " << this << "[***]" << Logging::endl;
  }

  PCollection(const PTuple& args, const PDict& kwargs)
  {
    Logging::trace << "PCollection: " << this << "[***]" << Logging::endl;
  }

  virtual ~PCollection()
  {
  }

  virtual Py_ssize_t get_length()
  {
    return 0;
  }

  virtual PObject append(const PTuple& args) 
  {
    return PObject::None();
  }

  virtual PObject remove(const PTuple& args)
  {
    return PObject::None();
  }

  virtual PObject iter()
  {
    incref();
    return this;
  }

  virtual PObject next() 
  {
    return NULL;
  }

  static Py_ssize_t get_length(PyObject *self)
  {
    PCollection *collection = static_cast<PCollection *>(self);

    return collection->get_length();
  }

  static PyObject *collection_append(PyObject *self, PyObject *args)
  {
    PCollection *collection = static_cast<PCollection *>(self);

    return collection->append(args);
  }

  static PyObject *collection_remove(PyObject *self, PyObject *args)
  {
    PCollection *collection = static_cast<PCollection *>(self);

    return collection->remove(args);
  }

  static PyObject *collection_iter(PyObject *self)
  {
    PCollection *collection = static_cast<PCollection *>(self);

    return collection->iter();
  }

  static PyObject *collection_iter_next(PyObject *self)
  {
    PCollection *collection = static_cast<PCollection *>(self);

    return collection->next();
  }

  static void free(void *ptr)
  {
    PyObject *tmp = static_cast<PyObject *>(ptr);
    PCollection *obj = static_cast<PCollection *>(tmp);

    Logging::trace << "PCollection: " << obj << "[___]" << Logging::endl;

    delete obj;
  }

  static PyObject *get_item(PyObject *, PyObject *)
  {
    return PObject::None();
  }

  static int set_item(PyObject *, PyObject *, PyObject *)
  {
    return 0;
  }

  static void init(PyObject *module)
  {
    static PySequenceMethods seq_methods =
    {
      get_length
    };

    static PyMappingMethods map_methods =
    {
      get_length,
      get_item,
      set_item
    };

    add_method("append", "Add an item to the collection", VarArgsMethod(&collection_append));
    add_method("remove", "Remove an item from the collection", VarArgsMethod(&collection_remove));

    PyTypeObject& type = type_object();

    type.tp_free = free;
    type.tp_iter = reinterpret_cast<getiterfunc>(collection_iter);
    type.tp_iternext = reinterpret_cast<iternextfunc>(collection_iter_next);    
    type.tp_as_sequence = &seq_methods;
    type.tp_as_mapping = &map_methods;

    type.tp_flags |= Py_TPFLAGS_HAVE_ITER;
    type.tp_name = "Collection";
    PCType::init(module);
  }
};

#endif
