
#include "testing.h"

int main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused)))
{
    SKIP_TEST( "test skip by user" );
    BEGIN_TESTING(skip test);
    END_TESTING;
}
