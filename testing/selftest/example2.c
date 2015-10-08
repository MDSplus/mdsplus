
#include <stdio.h>

#include <testing.h>
#include <check.h>


int main(int argc, char *argv[])
{
    BEGIN_TESTING(example2_success) 
    {           
        printf("Hello successfull test\n");    
        int success = 1;
        TEST1(success == 1);
    }
    END_TESTING;
    
    BEGIN_TESTING(example2_fail)
    {
        printf("Hello failing test\n");    
        int success = 0;
        TEST1(success == 1);        
    }
    END_TESTING;
}
