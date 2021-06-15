#ifndef MDSTESTUTILS_VECTOR_TEST_H
#define MDSTESTUTILS_VECTOR_TEST_H

#include <vector>

////////////////////////////////////////////////////////////////////////////////
//  Vector Utils ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace std
{
  template <typename T>
  inline ostream &operator<<(ostream &o, const vector<T> &v)
  {
    typedef typename vector<T>::const_iterator iterator;
    for (iterator it = v.begin(); it < v.end(); ++it)
    {
      o << *it << " ";
    }
    return o;
  }

  template <typename T>
  inline istream &operator>>(istream &is, vector<T> &v)
  {
    T value;
    v.clear();
    while (!(is >> value).fail())
      v.push_back(value);
    return is;
  }

  // std implementation only for the same type //
  // template<typename _Tp, typename _Alloc>
  //  inline bool
  //  operator==(const vector<_Tp, _Alloc>& __x, const vector<_Tp, _Alloc>& __y)
  //  { return (__x.size() == __y.size()
  //        && std::equal(__x.begin(), __x.end(), __y.begin())); }

  // std implementation for different types //
  template <typename _T1, typename _T2, typename _Alloc1, typename _Alloc2>
  inline bool operator==(const std::vector<_T1, _Alloc1> &v1,
                         const std::vector<_T2, _Alloc2> &v2)
  {
    if (v1.size() != v2.size())
      return false;
    for (size_t i = 0; i < v1.size(); ++i)
      if (!(v1[i] == static_cast<_T1>(v2[i])))
        return false;
    return true;
  }
} // namespace std

#endif // VECTOR_TEST_H
