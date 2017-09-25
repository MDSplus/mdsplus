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
//  DIMENSION COMPOUND  ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Dimension class object description of DTYPE_DIMENSION
///

//class Dimension: public Compound
//{
//public:
//    Dimension(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):Compound(dtype, length, ptr, nDescs, descs);
//    Dimension(Data *window, Data *axis, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0);

//    Data *getWindow();
//    Data *getAxis();
//    void setWindow(Data *window) { assignDescAt(window, 0); }
//    void setAxis(Data *axis){ assignDescAt(axis, 1); }
//};

int main () {
    BEGIN_TESTING(Dimension);
    
        
    { // CTR
        Data * raw[200];
        for(int i=0; i<200; ++i) raw[i] = new Float32(i);    
        char dummy_op_code = 0;
        unique_ptr<Dimension> ctr1 = new Dimension(0,sizeof(dummy_op_code),&dummy_op_code,200,(char**)raw);
    }
    
    Range * range = new Range(NULL, NULL, new Float32(0.1));
    Window * window = new Window(new Float32(-9), new Float32(0), new Float32(0.0));
    unique_ptr<Dimension> dim = new Dimension(window, range);
    
    { // clone //
        unique_ptr<Dimension> clone = (Dimension*)dim->clone();
        unique_ptr<Data>(clone->getWindow());
        unique_ptr<Data>(clone->getAxis());
    }
    
    { // TDI compile //
        std::stringstream ss; ss << (Dimension*)dim;
        TEST1(ss.str() == std::string("Build_Dim(0, Build_Window(0, -9., 0., 0.), * : * : .1)"));
    }
    
    { // Accessors //
        float array_data[10] = { 1,2,3,4,5,6,7,8,9,10 };
        dim->setAxis( new Float32Array(array_data,10) );
        dim->setWindow( new Window(new Float32(-9), new Float32(0), new Float32(-10.0)) );           
        unique_ptr<Data>(dim->getWindow());
        unique_ptr<Data>(dim->getAxis());
    }

    
    END_TESTING;
}
