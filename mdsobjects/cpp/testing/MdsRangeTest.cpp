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
#include <mdsobjects.h>

#include "testing.h"
#include "testutils/unique_ptr.h"
#include "mdsplus/AutoPointer.hpp"

using namespace MDSplus;
using namespace testing;

////////////////////////////////////////////////////////////////////////////////
//  Range COMPOUND  ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



int main() {
    BEGIN_TESTING(Range);
            
    { // CTR
        Data * raw[200];
        for(int i=0; i<200; ++i) raw[i] = new Float32(i);    
        char dummy_op_code = 0;
        unique_ptr<Range> ctr = new Range(0,sizeof(dummy_op_code),&dummy_op_code,200,(char**)raw);
    }
        
    unique_ptr<Range> range = new Range(new Int32(0), new Int32(5), new Float64(0.5));
    
    { // clone //
        unique_ptr<Range> clone = (Range*)range->clone();        
        clone->setBegin(new Int32(0));
        clone->setEnding(new Int32(1));
        clone->setDeltaVal(new Float32(1.));
        unique_ptr<Data>(clone->getBegin());
        unique_ptr<Data>(clone->getEnding());
        unique_ptr<Data>(clone->getDeltaVal());
    }
    
    { // TDI compile //
        std::stringstream ss; ss << (Range*)range;      
        TEST1(ss.str() == std::string("0 : 5 : .5D0"));
    }
    
    { // test TDI evaluated data //
        std::stringstream ss; ss << (Data*)unique_ptr<Data>(range->data());        
        TEST1(ss.str() == std::string("[0D0,.5D0,1D0,1.5D0,2D0,2.5D0,3D0,3.5D0,4D0,4.5D0,5D0]"));
    }
    
    { // access //
        range->setBegin(new Int32(0));
        range->setEnding(new Int32(-10));
        range->setDeltaVal(new Float32(-1.0));
        unique_ptr<Data>(range->getBegin());
        unique_ptr<Data>(range->getEnding());
        unique_ptr<Data>(range->getDeltaVal());
        std::stringstream ss; ss << (Data*)unique_ptr<Data>(range->data());
        TEST1(ss.str() == std::string("[0.,-1.,-2.,-3.,-4.,-5.,-6.,-7.,-8.,-9.,-10.]"));        
    }    
            
    END_TESTING;
}

