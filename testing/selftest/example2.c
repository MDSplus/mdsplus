
#include <stdio.h>

#include <testing.h>


int main(int argc, char *argv[])
{
    BEGIN_TESTING(example2_success 0) 
    {           
        printf("Hello successfull test\n");    
        int success = 1;
        TEST1(success == 1);
    }
    END_TESTING;
    
    TEST_FORK(0);
    
    BEGIN_TESTING(example2_success 1) 
    {           
        printf("Hello successfull test\n");    
        int success = 1;
        TEST1(success == 1);
    }
    END_TESTING;
       
    BEGIN_TESTING(example2_success 2) 
    {           
        printf("Hello successfull test\n");    
        int success = 1;
        TEST1(success == 1);
    }
    END_TESTING;
    
    TEST_FORK(1);
    
    BEGIN_TESTING(example2 fail 0)
    {        
        printf("Hello failing test\n");    
        int success = 0;
        TEST1(success == 1);        
    }
    END_TESTING;
    
    TEST_FORK(0);
    
    BEGIN_TESTING(example2 fail 1)
    {        
        printf("Hello failing test\n");    
        int success = 0;
        TEST1(success == 1);        
    }
    END_TESTING;
    
    BEGIN_TESTING(example2 fail 2)
    {        
        printf("Hello failing test\n");    
        int success = 0;
        TEST1(success == 1);        
    }
    END_TESTING;
    
    TEST_FORK(1);
    
    BEGIN_TESTING(example2 fail 3)
    {        
        printf("Hello failing test\n");    
        int success = 0;
        TEST1(success == 1);        
    }
    END_TESTING;
}
