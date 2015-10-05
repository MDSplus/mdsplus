
#include <stdio.h>

#include <testing.h>
#include <check.h>


int main(int argc, char *argv[])
{
    BEGIN_TESTING(example2);
           
    printf("Hello tests\n");
    
    int success = 1;
    TEST1(success == 1);
    TEST1(success == 0);
    
    END_TESTING;
}
