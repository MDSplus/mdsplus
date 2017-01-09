
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



