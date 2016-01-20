#include <stdio.h>
#include "testing.h"
#include "config.h"

int main(int argc, char *argv[])
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


