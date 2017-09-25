/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef MDSTESTUTILS_TYPE_TRAITS_H
#define MDSTESTUTILS_TYPE_TRAITS_H


namespace testing {


////////////////////////////////////////////////////////////////////////////////
//  REMOVE PTR   ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace detail {

template < typename T >
struct remove_pointer { typedef T type; };

#define _MDS_TESTING_TYPE_TRAIT_IMPL_PARTIAL1(param,trait,spec,result) \
template<param> struct trait<spec> \
{ typedef result type; };

_MDS_TESTING_TYPE_TRAIT_IMPL_PARTIAL1(typename T,remove_pointer,T*,T)
_MDS_TESTING_TYPE_TRAIT_IMPL_PARTIAL1(typename T,remove_pointer,T* const,T)
_MDS_TESTING_TYPE_TRAIT_IMPL_PARTIAL1(typename T,remove_pointer,T* volatile,T)
_MDS_TESTING_TYPE_TRAIT_IMPL_PARTIAL1(typename T,remove_pointer,T* const volatile,T)

#undef _MDS_TESTING_TYPE_TRAIT_IMPL_PARTIAL1

} // detail


////////////////////////////////////////////////////////////////////////////////
//  MDS to CPP TYPE TRAIT MAP  /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace detail {

#define _MDS_TESTING_TYPE_TRAIT_IMPL_SP1(trait,spec,result) \
    template<> struct trait<spec> \
        { typedef result type; }; \
    template<> struct trait<spec##Array>       \
        { typedef std::vector<result> type; }; \
    /**/

template < typename T >
struct mds2cpp_typemap { typedef T type; };

_MDS_TESTING_TYPE_TRAIT_IMPL_SP1(mds2cpp_typemap, MDSplus::Int8,  char )
_MDS_TESTING_TYPE_TRAIT_IMPL_SP1(mds2cpp_typemap, MDSplus::Int16, short)
_MDS_TESTING_TYPE_TRAIT_IMPL_SP1(mds2cpp_typemap, MDSplus::Int32, int  )
_MDS_TESTING_TYPE_TRAIT_IMPL_SP1(mds2cpp_typemap, MDSplus::Int64, int64_t)

_MDS_TESTING_TYPE_TRAIT_IMPL_SP1(mds2cpp_typemap, MDSplus::Uint8,  unsigned char )
_MDS_TESTING_TYPE_TRAIT_IMPL_SP1(mds2cpp_typemap, MDSplus::Uint16, unsigned short)
_MDS_TESTING_TYPE_TRAIT_IMPL_SP1(mds2cpp_typemap, MDSplus::Uint32, unsigned int  )
_MDS_TESTING_TYPE_TRAIT_IMPL_SP1(mds2cpp_typemap, MDSplus::Uint64, uint64_t)

_MDS_TESTING_TYPE_TRAIT_IMPL_SP1(mds2cpp_typemap, MDSplus::Float32, float)
_MDS_TESTING_TYPE_TRAIT_IMPL_SP1(mds2cpp_typemap, MDSplus::Float64, double)

//_MDS_TESTING_TYPE_TRAIT_IMPL_SP1(mds2cpp_typemap, MDSplus::Complex32, std::complex<float>)
//_MDS_TESTING_TYPE_TRAIT_IMPL_SP1(mds2cpp_typemap, MDSplus::Complex64, std::complex<double>)


#undef _MDS_TESTING_TYPE_TRAIT_IMPL_SP1

} // detail
} // testing

#endif // TYPE_TRAITS_H

