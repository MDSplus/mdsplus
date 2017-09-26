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

#include <string>
#include <cstring>


#include <mdsobjects.h>

#include "testing.h"
#include "testutils/unique_ptr.h"
#include "mdsplus/AutoPointer.hpp"

using namespace MDSplus;
using namespace testing;



////////////////////////////////////////////////////////////////////////////////
//  COMPOUND DATA   ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief Test Compound class, object description of DTYPE_R
///
/// Compound is the common supreclass for all CLASS_R types. Its fields contain
/// all the required information (Descriptor array, keyword). Its getter/setter
/// methods allow to read/replace descriptors, based on their index. Derived
/// classes will only define methods with appropriate names for reading/writing
/// descriptors (i.e. Data objects). This should perform Constructors both with
/// no arguments and with created Data to test if the propagate of the
/// destructor correctly free the allocated memeory. Please run tests using
/// --enable-valgrind option to check for memory leaks.
///

// class EXPORT Compound: public Data
// public:
//    Compound();
//    Compound(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0);
//    virtual void propagateDeletion();
//    void * convertToDsc();
//    virtual ~Compound();

int main(int argc UNUSED_ARGUMENT, char *argv[] UNUSED_ARGUMENT)
{
    BEGIN_TESTING(Compound);

    Compound * com1 = new Compound();

    Data * descs[4];
    descs[0] = new Int32(123);
    descs[1] = new String("descriptor 1");
    descs[2] = new Float32(1.234);
    const char * array = "test";
    descs[3] = new Int8Array(array,4);

    short dummy_opcode = 0;

    // propagate deletion should correctly perform deletion of descriptors.
    Compound  * com2 = new Compound(0,sizeof(short),(char *)&dummy_opcode,4,(char **)descs);

    deleteData(com1);

    // test for copy constructor ... commented as it should not compile //
    //    com1 = new Compound(*com2);

    deleteData(com2);

    END_TESTING;
}
