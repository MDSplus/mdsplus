#include <testing.h>

int main(int argc, char *argv[])
{
    BEGIN_TESTING(fork fail);
#   ifdef _WIN32
    SKIP_TEST;
#   endif
    assert( 0&& "Assertion failed" );
    END_TESTING;
}

