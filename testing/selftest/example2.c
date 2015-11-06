
#include <stdio.h>

#include <testing.h>




int main(int argc, char *argv[])
{            
    BEGIN_TESTING(example2 fail 1)
    {        
        printf("Hello\n");    
        int success = 0;
        TEST1(success == 1); 
        printf("After test\n");                    
    }
    END_TESTING;
    
}

