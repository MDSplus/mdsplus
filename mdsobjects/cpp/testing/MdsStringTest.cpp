
#include <string>
#include <cstring>

#include <mdsobjects.h>

#include "testing.h"
#include "MdsDataTest.h"


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

int main(int argc, char *argv[])
{
    BEGIN_TESTING(String);

    String string();


    END_TESTING;
}



