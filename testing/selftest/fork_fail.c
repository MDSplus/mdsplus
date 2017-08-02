#include <testing.h>

int main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused)))
{
    BEGIN_TESTING(fork fail);
#   ifdef _WIN32
    SKIP_TEST("Skipping fork tests under wine");
#   endif
    assert( 0&& "Assertion failed" );
    END_TESTING;
}

