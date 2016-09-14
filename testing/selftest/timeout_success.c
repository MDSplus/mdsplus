#include "testing.h"
// Skip test in windows as fork is not supported yet //
#   ifdef _WIN32
    int main() { SKIP_TEST; }
#else

#include <unistd.h>
#include <stdlib.h>

int main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused)))
{
    
    TEST_TIMEOUT(0.011);
    BEGIN_TESTING(timeout success 10ms );
    usleep(1E4);
    END_TESTING;
    
    TEST_TIMEOUT(0.021);    
    BEGIN_TESTING(timeout success 20ms);
    usleep(2E4);
    END_TESTING;
    
    // set time unit to double the seconds 
    // (suppose we are in a half expected speed machine )
    setenv("TEST_TIMEUNIT","2",1);
            
    BEGIN_TESTING(timeout success 2*20ms);
    // simulate a 20ms expected operation in a half speed machine //
    usleep(4E4);
    END_TESTING;
}

#   endif    
