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

#ifndef POBJECT_H_INC
#define POBJECT_H_INC

#include "Python.h"

class PString;

class PObject
{
protected:
  PyObject *object;
public:
  PObject();
  PObject(PyObject *);
  PObject(int, int, int) : object(NULL) { }; // empty constructor only for use in derived classes
  PObject(const PObject &);
  virtual ~PObject();

  inline virtual operator PyObject *() const { return object; }

  virtual PObject& operator=(const PObject&);
  virtual PObject& operator=(const PyObject *);

  virtual PObject operator()(const PObject& );
  virtual PObject operator()(const PObject&, const PObject&);

  template<typename T>
  T As() const
  {
    if(!T::check(*this))
      return static_cast<T>(None());

    return static_cast<T>(*this);
  }

  template<typename T>
  T *AsPtr() const
  {
    PyObject *tmp = *this;

    if(!T::check(*this))
    {
      Py_INCREF(Py_None);
      return static_cast<T *>(Py_None);
    }

    return static_cast<T *>(tmp);
  }
  
  inline virtual PObject getattr(const char *attr) { return PyObject_GetAttrString(object, attr); }
  inline virtual bool is_true() const { return PyObject_IsTrue(object) != 0; }
  virtual const PObject& incref();
  virtual void decref();
  virtual PString str() const;

  static const PObject None();
  inline static bool check(const PObject& item) { return Py_TYPE(item.object) == &PyBaseObject_Type; }
};

#endif
