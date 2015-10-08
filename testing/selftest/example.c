
#include <stdio.h>
#include <string.h>
#include <unistd.h>


#include <testing.h>


void test_pass_function();
void test_fail_function();
void external_test();


////////////////////////////////////////////////////////////////////////////////
//  main  //////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


int main(int argc, char *argv[])
{    
    // test blok not explicit //
    BEGIN_TESTING(example1);   
    printf(" ... child 1 ... \n");    
    
    int a = 1;
    TEST1( 1 && "this works and is not logged" );
    TEST1( a == 0 && "test on failing assertion");        
    
    END_TESTING;

    
    // test block explicit //
    BEGIN_TESTING(example2) {              
        printf(" ... child 2 ... \n");
        
        int a, *b; 
        memcpy(b,&a,10000000000);        
        printf("I should be in segfault here...\n");
    }    
    END_TESTING;
    
    
    // start testing without fork //
    TEST_FORK(0);
    
    BEGIN_TESTING(example3) {
        printf(" ... NO forked function ... \n");
        
        int c=1;        
        TEST0(c==0 && "this is true as we expect a false assetion");        
        test_pass_function();
        test_fail_function();        
    }
    END_TESTING;    
    
    
    external_test();
        
    
    TEST_FORK(1);    
    BEGIN_TESTING(timeout) {
        printf(" ... child 3 ... \n");
        sleep(30);
    }
    END_TESTING;
    
    // do not return .. here //
    // test automatically returns with the correct number of errors //
}


////////////////////////////////////////////////////////////////////////////////
//  test functions  ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


///
/// Passing function called by a test
///

void test_pass_function() {    
    TEST1(1 && "passing external function");        
}

///
/// Failing function called by a test
///

void test_fail_function() {    
    TEST1(0 && "no passing external function");    
}


///
/// Test nested into an external function .. note that this can not be called
/// inside another test as tests can not be nested.
///

void external_test() {
    BEGIN_TESTING(external_test);    
    printf(" ... child 4 ... \n");    
    int external_test = 0;
    TEST1(external_test==0);
    END_TESTING;
}
