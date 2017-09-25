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
#ifndef UNIQUE_PTR_H
#define UNIQUE_PTR_H

#include <mdsobjects.h>

namespace testing {

////////////////////////////////////////////////////////////////////////////////
//  Unique Ptr  ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template < typename T >
class Deleter {
public:
    static void _delete(MDSplus::Data * ptr) {
        MDSplus::deleteData(ptr);
    }

    static void _delete(void * ptr) {
        delete (T*)(ptr);
    }
};

template < typename T, typename D = Deleter<T> >
class unique_ptr {
    unique_ptr(const T &ref) : ptr(new T(ref)) {}
    T *ptr;
public:

    unique_ptr() : ptr(NULL) {}

    unique_ptr(unique_ptr &other) : ptr(other.ptr)
    { other.ptr = NULL; }

    unique_ptr(const unique_ptr &other) : ptr(other.ptr)
    { const_cast<unique_ptr&>(other).ptr = NULL; }

    unique_ptr(T *ref) : ptr(ref) { }

    ~unique_ptr() { _delete(); }

    unique_ptr & operator = (T * ref) {
        _delete();
        ptr = ref;
        return *this;
    }

    unique_ptr & operator = (unique_ptr other) {
        ptr = other.ptr;
        other.ptr = NULL;
        return *this;
    }

    void _delete() { if(ptr) D::_delete(ptr); ptr=NULL; }

    operator bool() { return ptr; }

    operator T *() { return ptr; }
    operator const T *() const { return ptr; }

//    operator T *&() { return ptr; }
//    operator const T *&() const { return ptr; }

    T * operator ->() { return ptr; }
    const T * operator ->() const { return ptr; }

    T *& base() { return ptr; }
    const T *& base() const { return ptr; }
};



} // testing




#endif // UNIQUE_PTR_H

