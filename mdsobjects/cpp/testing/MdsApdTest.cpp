#include <stdlib.h>
#include <mdsobjects.h>

#include "testing.h"
#include "testutils/unique_ptr.h"
#include "mdsplus/AutoPointer.hpp"

using namespace MDSplus;
using namespace testing;


int main(int argc UNUSED_ARGUMENT, char *argv[] UNUSED_ARGUMENT)
{
    BEGIN_TESTING(Apd);
    SKIP_TEST("Apd test is not implemented yet");

    // NOTE: Apd does not increment reference counting !! //
    // TO BE CONTINUED ... //

    END_TESTING;
}


