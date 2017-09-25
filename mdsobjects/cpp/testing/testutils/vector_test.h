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
#ifndef MDSTESTUTILS_VECTOR_TEST_H
#define MDSTESTUTILS_VECTOR_TEST_H


#include <vector>

////////////////////////////////////////////////////////////////////////////////
//  Vector Utils ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


namespace std {
template < typename T >
inline ostream &
operator << (ostream &o, const vector<T> &v) {
    typedef typename vector<T>::const_iterator iterator;
    for(iterator it = v.begin(); it < v.end(); ++it) {
        o << *it << " ";
    }
    return o;
}

template < typename T >
inline istream &
operator >> (istream &is, vector<T> &v) {
    T value;
    v.clear();
    while( !(is >> value).fail() )
        v.push_back(value);
    return is;
}


// std implementation only for the same type //
//template<typename _Tp, typename _Alloc>
//  inline bool
//  operator==(const vector<_Tp, _Alloc>& __x, const vector<_Tp, _Alloc>& __y)
//  { return (__x.size() == __y.size()
//        && std::equal(__x.begin(), __x.end(), __y.begin())); }

// std implementation for different types //
template < typename _T1, typename _T2, typename _Alloc1, typename _Alloc2 >
inline bool
operator == (const std::vector<_T1,_Alloc1> &v1, const std::vector<_T2,_Alloc2> &v2) {
    if( v1.size() != v2.size() )
        return false;
    for(size_t i=0; i<v1.size(); ++i)
        if( !(v1[i]==static_cast<_T1>(v2[i])) ) return false;
    return true;
}
} // std


#endif // VECTOR_TEST_H

