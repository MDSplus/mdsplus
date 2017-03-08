
#include <stdio.h>
#include <testing.h>


int main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused)))
{
    TEST_FORK(0);
    BEGIN_TESTING(no fork success) 
    {           
        printf("Hello successfull test\n");    
        int success = 1;
        TEST1(success == 1);
    }
    END_TESTING;    
    
    TEST_FORK(1);
    BEGIN_TESTING(yes fork fail) 
    {           
        printf("Hello successfull test\n");    
        int success = 0;
        TEST1(success == 1);
    }
    END_TESTING;    
    
    TEST_FORK(0);
    BEGIN_TESTING(no fork success) 
    {           
        printf("Hello successfull test\n");    
        int success = 1;
        TEST1(success == 1);
    }
    END_TESTING;    
}


