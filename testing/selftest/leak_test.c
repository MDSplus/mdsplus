
#include <stdlib.h>

#include "config.h"
#include "testing.h"

int main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused)))
{
    if(!getenv("VALGRIND_TOOL") || strcmp("memcheck",getenv("VALGRIND_TOOL")))
        SKIP_TEST("This test is supposed to run with valgrind memcheck");

    BEGIN_TESTING(leak test);
    volatile int undef;
    if(undef) {
        printf(".");
    }
    else {
        printf(",");
    }
    TEST1(1);    
    END_TESTING
}



