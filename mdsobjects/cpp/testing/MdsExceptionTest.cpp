
#include <mdsobjects.h>
#include "testing.h"

namespace mds = MDSplus;


void throw_exception_msg() {
    throw mds::MdsException("test message");
}

void throw_exception_status() {
    throw mds::MdsException(5552368);
}


int main()
{
    BEGIN_TESTING(MdsException);

    try{ throw_exception_msg(); }
    catch (mds::MdsException e) {
        TEST1( std::string(e.what()) == "test message" );
    }

    try{ throw_exception_status(); }
    catch (mds::MdsException e) {
        TEST1( std::string(e.what()) == std::string(MdsGetMsg(5552368)) );
    }

    END_TESTING;
}

