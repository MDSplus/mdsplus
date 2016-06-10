
#include <stdlib.h>
#include <mdsobjects.h>
#include "testing.h"

#include <limits>

int main(int argc, char *argv[])
{
    BEGIN_TESTING(build);
    
    TEST1(1);
    TEST0(0);    
        
    END_TESTING
}


