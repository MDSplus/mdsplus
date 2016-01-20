
#include "testing.h"

int main(int argc, char *argv[])
{
    SKIP_TEST( "test skip by user" );
    BEGIN_TESTING(skip test);
    END_TESTING;
}
