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
#include <stdlib.h>
#include <mdsobjects.h>

#include "testing.h"
#include "testutils/unique_ptr.h"
#include "mdsplus/AutoPointer.hpp"

using namespace MDSplus;
using namespace testing;

////////////////////////////////////////////////////////////////////////////////
//  Call COMPOUND  ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/// This test refers to three C functions compiled into a shared library that
/// should be present in ./testutils/libMdsTestDummy.so

int main(int,char **argv UNUSED_ARGUMENT) {
    BEGIN_TESTING(Call);

    SKIP_TEST("Skipping this tests for now .. call fails if a full .so path is passed");

    { // CTR
        Data * raw[200];
        for(int i=0; i<200; ++i) raw[i] = new Float32(i);
        char dummy_op_code = 0;
        unique_ptr<Call> ctr = new Call(0,sizeof(dummy_op_code),&dummy_op_code,200,(char**)raw);
    }

    {
        // Only check if the it finds libm.a from system libraries //
        unique_ptr<Call> call_std = new Call(new String("m"), new String("sin"),0,NULL );
    }

    unique_ptr<Call> call = new Call(new String("./testutils/libMdsTestDummy.so"),new String("get_ghostbusters_phone"),0,NULL);
    TEST1( unique_ptr<Data>(call->data())->getInt() == 5552368 );

    {
        // clone //
        unique_ptr<Call> clone = (Call*)call->clone();
        clone->setRoutine(new String("routine"));
        clone->setImage(new String("image"));
        TEST1( AutoString(AutoData<Data>(clone->getRoutine())->getString()).string == std::string("routine") );
        TEST1( AutoString(AutoData<Data>(clone->getImage())->getString()).string == std::string("image") );
        // This should be avoided as number of arguments are set from ctr only //
        // This throws a std out-of-range exception, but any exception is ok   //
        TEST_EXCEPTION( clone->setArgumentAt(new Int32(552368),0), std::exception );
        TEST_EXCEPTION( clone->getArgumentAt(0), std::exception );
    }

    {
        // unary //
        Data *args[1] = { new Int32(5552367) };
        unique_ptr<Call> call1 = new Call(new String("./testutils/libMdsTestDummy.so"),new String("test_nextint"),1,(Data**)args);
        TEST1( unique_ptr<Data>(call1->data())->getInt() == 5552368 );
        deleteData(args[0]);
    }

    {
        // binary //
        Data *args[2] = { new Int32(5552367), new Int32(1) };
        unique_ptr<Call> call2 = new Call(new String("./testutils/libMdsTestDummy.so"),new String("test_addint"),2,(Data**)args);
        TEST1( unique_ptr<Data>(call2->data())->getInt() == 5552368 );
        deleteData(args[0]);
        deleteData(args[1]);
    }

    {
        // TDI compile //
        std::stringstream ss; ss << (Call*)call;
        //        std::cout << ss.str();
        TEST1(ss.str() == std::string("./testutils/libMdsTestDummy.so->get_ghostbusters_phone:L()"));
    }



    END_TESTING;
}


