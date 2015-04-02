
#include <mdsobjects.h>
#include <string>
#include <cstring>


#include "testing.h"

#include "MdsDataTest.h"


namespace mds = MDSplus;
using namespace  testing;

template < typename Target, typename Source >
Target cast_check(Source value) {
    return mds::Scalar::scalar_cast<Target>(value);
}



int main(int argc, char *argv[])
{

    // TEST IF ISNAN WORKS //

    TEST0(isnan(0.0));
    TEST0(isnan(1.0/0.0));
    TEST0(isnan(-1.0/0.0));
    TEST1(isnan(sqrt(-1.0)));








    std::cout << "\n";
    if(Singleton<TestResults>::get_instance().fails() == 0)
        std::cout << " SUCCESS !! \n";
    else
        std::cout << " SOME FAILS OCCURRED !! \n";

    return 0;
}

