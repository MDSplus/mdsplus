
#include <stdlib.h>
#include "testing.h"


int main(int argc, char *argv[])
{
    BEGIN_TESTING(leak test);

    int undef;
    if(undef) {
        printf(".");
    }
    else {
        printf(",");
    }

    TEST1(1);    
    END_TESTING
}



