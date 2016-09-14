
#include <stdlib.h>
#include <mdsobjects.h>
#include "testing.h"

#include <limits>

int main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused)))
{
    BEGIN_TESTING(build);
    
    TEST1(1);
    TEST0(0);    
        
    END_TESTING
}


