
#include <stdlib.h>
#include "testing.h"

#include <limits>

int main(int argc, char *argv[])
{
    BEGIN_TESTING(build);

    TEST1(1);
    TEST0(0);

    char * p = (char *)malloc(1234);
    strcpy(p,"ciao beo");
    printf("%s",p);
    
    
    END_TESTING
}


