#include "config.h"
#include "testing.h"
// Skip test in windows as fork is not supported yet //
#   ifdef _WIN32
    int main() { SKIP_TEST("current test is not performed on windows"); }
#else

#include <unistd.h>
#include <stdlib.h>

#ifdef HAVE_VALGRIND_H
# include <valgrind/valgrind.h>
#endif

int main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused)))
{
    
#if defined(__has_feature)
#  if __has_feature(address_sanitizer) || __has_feature(thread_sanitizer)
    SKIP_TEST("timeout test disabled on sanitizer");
#  endif
#endif

#if __SANITIZE_ADDRESS__ || __SANITIZE_THREAD__
    SKIP_TEST("timeout test disabled on sanitizer");
#endif

#ifdef HAVE_VALGRIND_H
   if(RUNNING_ON_VALGRIND)
       SKIP_TEST("timeout test disabled on valgrind");
#endif


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
