
#include <stdio.h>

#include <testing.h>




int main(int argc, char *argv[])
{            
    BEGIN_TESTING(example2 fail 1)
    {   
//        // generate a leak //
//        char * p = (char *)malloc(1234);
//        strcpy(p,"string lost\n");
//        printf("%s",p);
        
//        int i;
//        if(i) { printf("jumping condition undefined\n"); }
                
        int success = 1;
        TEST1(success);         
    }
    END_TESTING;
    
}

