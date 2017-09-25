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
#ifndef MDSTESTUTILS_FUNCTIONTYPES_H
#define MDSTESTUTILS_FUNCTIONTYPES_H

namespace testing {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// type synthesize ( please read: boost implementation  )                     //
////////////////////////////////////////////////////////////////////////////////

template <typename FuncT>
struct FunctionTypes {};

template <typename R, class O>
struct FunctionTypes< R(O::*)() > {
    typedef R  ref();
    typedef R (ptr)();
    typedef O  obj;
    typedef R ReturnType;
};

template <typename R, class O, typename T0>
struct FunctionTypes< R(O::*)(T0) > {
    typedef R  ref(T0);
    typedef R (ptr)(T0);
    typedef O  obj;
    typedef R ReturnType;
};

template <typename R, class O, typename T0, typename T1>
struct FunctionTypes< R(O::*)(T0,T1) > {
    typedef R  ref(T0,T1);
    typedef R (ptr)(T0,T1);
    typedef O  obj;
    typedef R ReturnType;
};

template <typename R, class O, typename T0, typename T1, typename T2>
struct FunctionTypes< R(O::*)(T0,T1,T2) > {
    typedef R  ref(T0,T1,T2);
    typedef R (ptr)(T0,T1,T2);
    typedef O  obj;
    typedef R ReturnType;
};

} // detail
} // testing

#endif // FUNCTIONTYPES_H

