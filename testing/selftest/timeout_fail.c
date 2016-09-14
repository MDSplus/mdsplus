#include "testing.h"
// Skip test in windows as fork is not supported yet //
#   ifdef _WIN32
    int main() { SKIP_TEST; }
#else

#include <unistd.h>
#include <stdlib.h>

int main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused)))
{
    
    TEST_TIMEOUT(0.009);
    BEGIN_TESTING(timeout fail 10ms );
    usleep(1E4);
    END_TESTING;
    
    TEST_TIMEOUT(0.019);
    BEGIN_TESTING(timeout fail 20ms);
    usleep(2E4);
    END_TESTING;
     
}

#   endif    

