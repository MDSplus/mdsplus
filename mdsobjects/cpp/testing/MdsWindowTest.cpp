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
//  WINDOW COMPOUND  ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


//class Window: public Compound
//{
//public:
//    Window(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0) :
//    Window(Data *startidx, Data *endidx, Data *value_at_idx0, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)

//    Data *getStartIdx()
//    Data *getEndIdx()
//    Data *getValueAt0()
//    void setStartIdx(Data *startidx) {assignDescAt(startidx, 0);}
//    void setEndIdx(Data *endidx){assignDescAt(endidx, 1);}
//    void setValueAt0(Data *value_at_idx0) {assignDescAt(value_at_idx0, 2);}

//};


int main() {
    BEGIN_TESTING(Window);

    { // CTR
        Data * raw[200];
        for(int i=0; i<200; ++i) raw[i] = new Float32(i);
        char dummy_op_code = 0;
        unique_ptr<Window> ctr = new Window(0,sizeof(dummy_op_code),&dummy_op_code,200,(char**)raw);
    }

    unique_ptr<Window> win = new Window(new Int32(0), new Int32(10), new Float64(0.5552368));

    { // clone //
        unique_ptr<Window> clone = (Window*)win->clone();
        clone->setStartIdx(new Int32(0));
        clone->setEndIdx(new Int32(1));
        clone->setValueAt0(new Float32(1.));
        unique_ptr<Data>(clone->getStartIdx());
        unique_ptr<Data>(clone->getEndIdx());
        unique_ptr<Data>(clone->getValueAt0());
    }

    { // TDI compile //
        std::stringstream ss; ss << (Window*)win;
        TEST1(ss.str() == std::string("Build_Window(0, 0, 10, .5552368D0)"));
    }

    { // access //
        win->setStartIdx(new Int32(0));
        win->setEndIdx(new Int32(-10));
        win->setValueAt0(new Float32(-1.0));
        unique_ptr<Data>(win->getStartIdx());
        unique_ptr<Data>(win->getEndIdx());
        unique_ptr<Data>(win->getValueAt0());
    }


    END_TESTING;
}
