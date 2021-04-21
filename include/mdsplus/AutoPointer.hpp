/*
 * AutoPointer.hpp
 *
 *  Created on: May 5, 2014
 *      Author: kerickso
 */

#ifndef MDS_AUTOPOINTER_HPP_
#define MDS_AUTOPOINTER_HPP_

#include <vector>
#include <string>

namespace MDSplus
{

  template <class T>
  struct AutoPointerBase
  {
    AutoPointerBase(T *t) : ptr(t)
    {
    }

    virtual ~AutoPointerBase() = 0;

    T *get()
    {
      return ptr;
    }

    T *operator->()
    {
      return ptr;
    }

    T &operator*()
    {
      return *ptr;
    }

    operator T *() { return ptr; }
    operator const T *() const { return ptr; }

    T *ptr;
  };

  template <class T>
  AutoPointerBase<T>::~AutoPointerBase() {}

  template <class T>
  struct AutoPointer : public AutoPointerBase<T>
  {
    AutoPointer(T *t) : AutoPointerBase<T>(t)
    {
    }

    virtual ~AutoPointer()
    {
      delete AutoPointerBase<T>::ptr;
    }
  };

  // FIXME: AutoData can't be a specialization of AutoPointer
  // FIXME: until we use a more recent version of VS that supports
  // FIXME: the TR1 type traits.
  template <class T>
  struct AutoData : public AutoPointerBase<T>
  {
    AutoData(T *t) : AutoPointerBase<T>(t)
    {
    }

    virtual ~AutoData()
    {
      deleteData(AutoPointerBase<T>::ptr);
    }
  };

  template <class T>
  struct AutoArray : public AutoPointerBase<T>
  {
    AutoArray(T *t) : AutoPointerBase<T>(t)
    {
    }

    virtual ~AutoArray()
    {
      delete[] AutoPointerBase<T>::ptr;
    }
  };

  template <class T>
  struct AutoVector : public AutoArray<T>
  {
    AutoVector(T *vBeg, T *vEnd) : AutoArray<T>(vBeg), vector(vBeg, vEnd) {}
    std::vector<T> vector;
  };

  struct AutoString : public AutoArray<char>
  {
    AutoString(char *cstr) : AutoArray<char>(cstr), string(cstr) {}
    std::string string;
  };
}

#endif /* MDS_AUTOPOINTER_HPP_ */
