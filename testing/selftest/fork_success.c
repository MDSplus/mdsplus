
#include <testing.h>

int main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused)))
{
    BEGIN_TESTING(fork success);
#   ifdef _WIN32
    SKIP_TEST("Skipping fork tests under wine");
#   endif    
    TEST0(0);
    TEST1(1);
    assert(1);  
    END_TESTING;         
}

