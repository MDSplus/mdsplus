#ifndef MDSTESTUTILS_TYPE_TRAITS_H
#define MDSTESTUTILS_TYPE_TRAITS_H

namespace testing
{

  ////////////////////////////////////////////////////////////////////////////////
  //  REMOVE PTR   ///////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////

  namespace detail
  {

    template <typename T>
    struct remove_pointer
    {
      typedef T type;
    };

#define _MDS_TESTING_TYPE_TRAIT_IMPL_PARTIAL1(param, trait, spec, result) \
  template <param>                                                        \
  struct trait<spec>                                                      \
  {                                                                       \
    typedef result type;                                                  \
  };

    _MDS_TESTING_TYPE_TRAIT_IMPL_PARTIAL1(typename T, remove_pointer, T *, T)
    _MDS_TESTING_TYPE_TRAIT_IMPL_PARTIAL1(typename T, remove_pointer, T *const, T)
    _MDS_TESTING_TYPE_TRAIT_IMPL_PARTIAL1(typename T, remove_pointer, T *volatile,
                                          T)
    _MDS_TESTING_TYPE_TRAIT_IMPL_PARTIAL1(typename T, remove_pointer,
                                          T *const volatile, T)

#undef _MDS_TESTING_TYPE_TRAIT_IMPL_PARTIAL1

  } // namespace detail

  ////////////////////////////////////////////////////////////////////////////////
  //  MDS to CPP TYPE TRAIT MAP  /////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////

  namespace detail
  {

#define _MDS_TESTING_TYPE_TRAIT_IMPL_SP1(trait, spec, result) \
  template <>                                                 \
  struct trait<spec>                                          \
  {                                                           \
    typedef result type;                                      \
  };                                                          \
  template <>                                                 \
  struct trait<spec##Array>                                   \
  {                                                           \
    typedef std::vector<result> type;                         \
  };                                                          \
  /**/

    template <typename T>
    struct mds2cpp_typemap
    {
      typedef T type;
    };

    _MDS_TESTING_TYPE_TRAIT_IMPL_SP1(mds2cpp_typemap, MDSplus::Int8, char)
    _MDS_TESTING_TYPE_TRAIT_IMPL_SP1(mds2cpp_typemap, MDSplus::Int16, short)
    _MDS_TESTING_TYPE_TRAIT_IMPL_SP1(mds2cpp_typemap, MDSplus::Int32, int)
    _MDS_TESTING_TYPE_TRAIT_IMPL_SP1(mds2cpp_typemap, MDSplus::Int64, int64_t)

    _MDS_TESTING_TYPE_TRAIT_IMPL_SP1(mds2cpp_typemap, MDSplus::Uint8, unsigned char)
    _MDS_TESTING_TYPE_TRAIT_IMPL_SP1(mds2cpp_typemap, MDSplus::Uint16,
                                     unsigned short)
    _MDS_TESTING_TYPE_TRAIT_IMPL_SP1(mds2cpp_typemap, MDSplus::Uint32, unsigned int)
    _MDS_TESTING_TYPE_TRAIT_IMPL_SP1(mds2cpp_typemap, MDSplus::Uint64, uint64_t)

    _MDS_TESTING_TYPE_TRAIT_IMPL_SP1(mds2cpp_typemap, MDSplus::Float32, float)
    _MDS_TESTING_TYPE_TRAIT_IMPL_SP1(mds2cpp_typemap, MDSplus::Float64, double)

    //_MDS_TESTING_TYPE_TRAIT_IMPL_SP1(mds2cpp_typemap, MDSplus::Complex32,
    //std::complex<float>) _MDS_TESTING_TYPE_TRAIT_IMPL_SP1(mds2cpp_typemap,
    //MDSplus::Complex64, std::complex<double>)

#undef _MDS_TESTING_TYPE_TRAIT_IMPL_SP1

  } // namespace detail
} // namespace testing

#endif // TYPE_TRAITS_H
