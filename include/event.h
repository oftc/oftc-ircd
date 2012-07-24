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

#ifndef EVENT_H_INC
#define EVENT_H_INC

#include <list>

using std::list;

struct NullArg {};

template<class T1 = NullArg, class T2 = NullArg, class T3 = NullArg, class T4 = NullArg, class T5 = NullArg, class T6 = NullArg> 
class Event
{
private:
  list<function<bool(T1)> > handlers;
  list<function<bool(T1, T2)> > handlers2;
  list<function<bool(T1, T2, T3)> > handlers3;
  list<function<bool(T1, T2, T3, T4)> > handlers4;
  list<function<bool(T1, T2, T3, T4, T5)> > handlers5;
  list<function<bool(T1, T2, T3, T4, T5, T6)> > handlers6;
public:
  void attach(function<bool(T1)> func)
  {
    handlers.push_back(func);
  }
  void attach(function<bool(T1, T2)> func)
  {
    handlers2.push_back(func);
  }
  void attach(function<bool(T1, T2, T3)> func)
  {
    handlers3.push_back(func);
  }
  void attach(function<bool(T1, T2, T3, T4)> func)
  {
    handlers4.push_back(func);
  }
  void attach(function<bool(T1, T2, T3, T4, T5)> func)
  {
    handlers5.push_back(func);
  }
  void attach(function<bool(T1, T2, T3, T4, T5, T6)> func)
  {
    handlers6.push_back(func);
  }

  bool fire(T1 arg)
  {
    typename list<function<bool(T1)> >::const_iterator it;

    for(it = handlers.begin();  it != handlers.end(); it++)
    {
      function<bool(T1)> func = *it;
      func(arg);
    }

    return true;
  }
  bool fire(T1 arg, T2 arg2)
  {
    typename list<function<bool(T1, T2)> >::const_iterator it;

    for(it = handlers2.begin();  it != handlers2.end(); it++)
    {
      function<bool(T1, T2)> func = *it;
      func(arg, arg2);
    }

    return true;
  }
  bool fire(T1 arg, T2 arg2, T3 arg3)
  {
    typename list<function<bool(T1, T2, T3)> >::const_iterator it;

    for(it = handlers3.begin();  it != handlers3.end(); it++)
    {
      function<bool(T1, T2, T3)> func = *it;
      func(arg, arg2, arg3);
    }

    return true;
  }
  bool fire(T1 arg, T2 arg2, T3 arg3, T4 arg4)
  {
    typename list<function<bool(T1, T2, T3, T4)> >::const_iterator it;

    for(it = handlers4.begin();  it != handlers4.end(); it++)
    {
      function<bool(T1, T2, T3, T4)> func = *it;
      func(arg, arg2, arg3, arg4);
    }

    return true;
  }
  bool fire(T1 arg, T2 arg2, T3 arg3, T4 arg4, T5 arg5)
  {
    typename list<function<bool(T1, T2, T3, T4, T5)> >::const_iterator it;

    for(it = handlers5.begin();  it != handlers5.end(); it++)
    {
      function<bool(T1, T2, T3, T4, T5)> func = *it;
      func(arg, arg2, arg3, arg4, arg5);
    }

    return true;
  }
  bool fire(T1 arg, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6)
  {
    typename list<function<bool(T1, T2, T3, T4, T5, T6)> >::const_iterator it;

    for(it = handlers6.begin();  it != handlers6.end(); it++)
    {
      function<bool(T1, T2, T3, T4, T5, T6)> func = *it;
      func(arg, arg2, arg3, arg4, arg5, arg6);
    }

    return true;
  }
};

#endif
