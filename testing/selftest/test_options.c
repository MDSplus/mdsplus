#include <stdio.h>
#include "testing.h"

int main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused)))
{
    BEGIN_TESTING(1);
    TEST1(1);
    END_TESTING;
    
    printf("pappa\n");
    
    BEGIN_TESTING(2);
    TEST1(1);
    END_TESTING;               
    
    printf("pappa\n");
    
    BEGIN_TESTING(3);
    TEST1(1);
    END_TESTING;               
}


