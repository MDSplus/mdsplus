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
#include <iostream>
#include <iomanip>

#include <mdsobjects.h>

#include "testing.h"
#include "testutils/unique_ptr.h"
#include "mdsplus/AutoPointer.hpp"

using namespace MDSplus;
using namespace testing;


////////////////////////////////////////////////////////////////////////////////
//  CONGLOM COMPOUND  //////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Conglom class object description of DTYPE_CONGLOM
///

//class Conglom: public Compound
//{
//public:
//    Conglom(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0);
//    Conglom(Data *image, Data *model, Data *name, Data *qualifiers, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0);

//    Data *getImage() { return getDescAt(0); }
//    Data *getModel() { return getDescAt(1); }
//    Data *getName() { return getDescAt(2); }
//    Data *getQualifiers() { return getDescAt(3); }
//    void setImage(Data *image) {assignDescAt(image, 0);}
//    void setModel(Data *model){assignDescAt(model, 1);}
//    void setName(Data *name) {assignDescAt(name, 2);}
//    void setQualifiers(Data *qualifiers) {assignDescAt(qualifiers, 3);}
//};

int main(int argc UNUSED_ARGUMENT, char **argv UNUSED_ARGUMENT) {
    BEGIN_TESTING(Conglom);

    { // CTR
        Data * raw[200];
        for(int i=0; i<200; ++i) raw[i] = new Float32(i);
        char dummy_op_code = 0;
        unique_ptr<Conglom> ctr = new Conglom(0,sizeof(dummy_op_code),&dummy_op_code,200,(char**)raw);
    }


    unique_ptr<Conglom> cong = new Conglom( new String("Image"),
                                            new String("Model"),
                                            new String("Name"),
                                            new String("Qualifiers"));

    TEST1( AutoString(unique_ptr<Data>(cong->getImage())->getString()).string == std::string("Image"));
    TEST1( AutoString(unique_ptr<Data>(cong->getModel())->getString()).string == std::string("Model"));
    TEST1( AutoString(unique_ptr<Data>(cong->getName())->getString()).string  == std::string("Name"));
    TEST1( AutoString(unique_ptr<Data>(cong->getQualifiers())->getString()).string == std::string("Qualifiers"));

    { // clone //
        unique_ptr<Conglom> clone = (Conglom*)cong->clone();
        TEST1( AutoString(unique_ptr<Data>(clone->getImage())->getString()).string == std::string("Image"));
        TEST1( AutoString(unique_ptr<Data>(clone->getModel())->getString()).string == std::string("Model"));
        TEST1( AutoString(unique_ptr<Data>(clone->getName())->getString()).string  == std::string("Name"));
        TEST1( AutoString(unique_ptr<Data>(clone->getQualifiers())->getString()).string == std::string("Qualifiers"));
    }

    { // TDI compile //
        std::stringstream ss; ss << (Conglom*)cong;
        //        std::cout << (Conglom*)cong << "\n";
        TEST1(ss.str() == std::string("Build_Conglom(0, \"Image\", \"Model\", \"Name\", \"Qualifiers\")"));
    }

    { // access //
        cong->setImage(new String("new Image"));
        cong->setModel(new String("new Model"));
        cong->setName(new String("new Name"));
        cong->setQualifiers(new String("new Qualifiers"));
        TEST1( AutoString(unique_ptr<Data>(cong->getImage())->getString()).string == std::string("new Image"));
        TEST1( AutoString(unique_ptr<Data>(cong->getModel())->getString()).string == std::string("new Model"));
        TEST1( AutoString(unique_ptr<Data>(cong->getName())->getString()).string  == std::string("new Name"));
        TEST1( AutoString(unique_ptr<Data>(cong->getQualifiers())->getString()).string == std::string("new Qualifiers"));
    }


    END_TESTING;
}


