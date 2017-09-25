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
#ifndef MDSTESTUTILS_SINGLETON_H
#define MDSTESTUTILS_SINGLETON_H

namespace testing {

////////////////////////////////////////////////////////////////////////////////
//  Singleton  /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// Single instance within the same linked module
///
template < typename T >
class Singleton {
public:

    Singleton() {}
    Singleton(const T &copy) {
        this->get_instance() = copy;
    }

    inline T * operator -> () { return &get_instance(); }
    inline const T * operator -> () const { return &get_instance(); }

    static T & get_instance() {
        static T instance;
        return instance;
    }

    static const T & get_const_instance() {
        return const_cast<const T&>(get_instance());
    }

private:
//    Singleton(Singleton const&);      // Don't Implement
    void operator=(Singleton const&); // Don't implement
};

} // testing


#endif // SINGLETON_H

