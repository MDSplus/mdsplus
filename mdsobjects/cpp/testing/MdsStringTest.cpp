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
#include "MdsDataTest.h"
#include "mdsplus/AutoPointer.hpp"

using namespace MDSplus;
using namespace testing;


// class  EXPORT String : public Scalar
// {
// public:
//    String(const char *val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0);
//    String(const char *val, int len, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0);
//    String(unsigned char *uval, int len, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0);
//    String(unsigned char *uval, int numDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0);
//    char *getString();
//    bool equals(Data *data);
// };

//class StringArray: public Array
//{
//public:
//    StringArray(char **data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0);
//    StringArray(char const * data, int nStrings, int stringLen, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0); //For contiuguous
//};


int main(int argc UNUSED_ARGUMENT, char *argv[] UNUSED_ARGUMENT)
{
    BEGIN_TESTING(String);

    {
        String * string1 = new String("string");
        Data * string2 = string1->clone();
        Data * string3 = new String("string3",6);

        TEST0( strcmp(AutoArray<char>(string1->getString()),"string") );
        TEST0( strcmp(AutoArray<char>(string2->getString()),"string") );
        TEST1( string1->equals(string2) );
        TEST0( strcmp(AutoArray<char>(string2->getString()),"string") );        
        
        AutoString c_str(string1->getString());
        
        deleteData(string1);
        deleteData(string2);
        deleteData(string3);
    }

    {
        char *string[] = {
            (char*)"str1",
            (char*)"str2",
            (char*)"str3",
            (char*)"str4",
            (char*)"str5",
        };

        Array * sactr1 = new StringArray(string, 5);
        TEST1( AutoString(AutoData<Data>(sactr1->getElementAt(0))->getString()).string == std::string("str1") );
        TEST1( AutoString(AutoData<Data>(sactr1->getElementAt(1))->getString()).string == std::string("str2") );
        TEST1( AutoString(AutoData<Data>(sactr1->getElementAt(2))->getString()).string == std::string("str3") );
        TEST1( AutoString(AutoData<Data>(sactr1->getElementAt(3))->getString()).string == std::string("str4") );
        TEST1( AutoString(AutoData<Data>(sactr1->getElementAt(4))->getString()).string == std::string("str5") );

        Array * sactr2 = new StringArray("str1str2str3str4str5",5,4);
        TEST1( AutoString(AutoData<Data>(sactr2->getElementAt(0))->getString()).string == std::string("str1") );
        TEST1( AutoString(AutoData<Data>(sactr2->getElementAt(1))->getString()).string == std::string("str2") );
        TEST1( AutoString(AutoData<Data>(sactr2->getElementAt(2))->getString()).string == std::string("str3") );
        TEST1( AutoString(AutoData<Data>(sactr2->getElementAt(3))->getString()).string == std::string("str4") );
        TEST1( AutoString(AutoData<Data>(sactr2->getElementAt(4))->getString()).string == std::string("str5") );

        deleteData(sactr1);
        deleteData(sactr2);
    }


    END_TESTING;
}



