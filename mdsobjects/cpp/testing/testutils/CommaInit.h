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
#ifndef MDSTESTUTILS_COMMAINIT_H
#define MDSTESTUTILS_COMMAINIT_H

#include <vector>

////////////////////////////////////////////////////////////////////////////////
//  Comma Init  ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Comma Initializer template ...
// ContentT should provide operator[] and resize() methods.

namespace testing {

template < typename ContainerT, typename ContentT >
struct CommaInitializer
{
    typedef ContentT&(ContainerT::* OpType)(const size_t);

    inline explicit CommaInitializer(ContainerT *container,
                                     ContentT s,
                                     OpType op = &ContainerT::operator() )
        : container(container),
          operation(op)
    {
        this->index = 0;
        container->resize(1);
        (container->*operation)(0) = s;
    }

    inline CommaInitializer & operator, (ContentT s) {
        this->index++;
        container->resize(index + 1);
        (container->*operation)(this->index) = s;
        return *this;
    }

    ContainerT *container;
    OpType      operation;
    unsigned int index;
};

} // testing

////////////////////////////////////////////////////////////////////////////////
//  COMMA INIT FOR STD CONTAINERS  /////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace std {
template < typename _T >
inline testing::CommaInitializer< std::vector<_T>, _T > operator << (std::vector<_T> &cnt, _T scalar ) {
    return testing::CommaInitializer< std::vector<_T>, _T>(&cnt,scalar,&std::vector<_T>::operator []);
}
} // std



#endif // COMMAINIT_H

