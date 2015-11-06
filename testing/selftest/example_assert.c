#include <assert.h>
#include "testing.h"
#include "mdstestdummy.h"

// extern void generate_false_assert();

int main(int argc, char *argv[])
{
    BEGIN_TESTING(ok);    
    int a=1;
    TEST1(a==1);
    END_TESTING;    
    
    BEGIN_TESTING(not ok);    
    int a=1;
    TEST1(a==0);
    END_TESTING;    
    
//    BEGIN_TESTING(test direct assert);    
//    int a=1;
//    TEST1(a==1);
//    assert( "test direct assert"&&0 );
//    END_TESTING;    
    
//    BEGIN_TESTING(test lib assert);
//    generate_false_assert();
//    END_TESTING;    
}

