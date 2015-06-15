
#include <string>
#include <cstring>

#include <mdsobjects.h>

#include "testing.h"
#include "testutils/unique_ptr.h"
#include "mdsplus/AutoPointer.hpp"

using namespace MDSplus;
using namespace testing;



// class EXPORT Compound: public Data
// public:
//    Compound();
//    Compound(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0);
//    virtual void propagateDeletion();
//    void * convertToDsc();
//    virtual ~Compound();




int main(int argc, char *argv[])
{
    BEGIN_TESTING(Compound);

    unique_ptr<Compound> com1 = new Compound();
    unique_ptr<Compound> com2 = new Compound();


    END_TESTING;
}
