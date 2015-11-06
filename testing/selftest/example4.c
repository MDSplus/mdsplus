
#include <stdio.h>
#include <testing.h>


int main(int argc, char *argv[])
{
    TEST_FORK(0);
    BEGIN_TESTING(example4) 
    {           
        printf("Hello successfull test\n");    
        int success = 1;
        TEST1(success == 1);
    }
    END_TESTING;    
}


