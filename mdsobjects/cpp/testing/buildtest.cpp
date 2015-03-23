
#include <stdlib.h>
#include "testing.h"


int main(int argc, char *argv[])
{
    BEGIN_TESTING(build);

    TEST1(1);
    TEST0(0);

    END_TESTING
}


