#ifndef UNIQUE_PTR_H
#define UNIQUE_PTR_H

#include <mdsobjects.h>

namespace testing
{

  ////////////////////////////////////////////////////////////////////////////////
  //  Unique Ptr  ////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////

  template <typename T>
  class Deleter
  {
  public:
    static void _delete(MDSplus::Data *ptr) { MDSplus::deleteData(ptr); }

    static void _delete(void *ptr) { delete (T *)(ptr); }
  };

  template <typename T, typename D = Deleter<T> >
  class unique_ptr
  {
    unique_ptr(const T &ref) : ptr(new T(ref)) {}
    T *ptr;

  public:
    unique_ptr() : ptr(NULL) {}

    unique_ptr(unique_ptr &other) : ptr(other.ptr) { other.ptr = NULL; }

    unique_ptr(const unique_ptr &other) : ptr(other.ptr)
    {
      const_cast<unique_ptr &>(other).ptr = NULL;
    }

    unique_ptr(T *ref) : ptr(ref) {}

    ~unique_ptr() { _delete(); }

    unique_ptr &operator=(T *ref)
    {
      _delete();
      ptr = ref;
      return *this;
    }

    unique_ptr &operator=(unique_ptr other)
    {
      ptr = other.ptr;
      other.ptr = NULL;
      return *this;
    }

    void _delete()
    {
      if (ptr)
        D::_delete(ptr);
      ptr = NULL;
    }

    operator bool() { return ptr; }

    operator T *() { return ptr; }
    operator const T *() const { return ptr; }

    //    operator T *&() { return ptr; }
    //    operator const T *&() const { return ptr; }

    T *operator->() { return ptr; }
    const T *operator->() const { return ptr; }

    T *&base() { return ptr; }
    const T *&base() const { return ptr; }
  };

} // namespace testing

#endif // UNIQUE_PTR_H
