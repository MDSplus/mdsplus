/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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


int main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused)))
{    
    
    BEGIN_TESTING(test 1);
    printf("test1\n");
    int a = 1;
    TEST1( 1 && "this works and is not logged" );
//    TEST1( a == 0 && "test on failing assertion");            
    END_TESTING;
    
    BEGIN_TESTING(test 1 fail);
    printf("test1\n");
    int a = 1;
    TEST1( a == 0 && "test on failing assertion");            
    END_TESTING;

    
    // test block explicit //
    BEGIN_TESTING(example2) {                      
        int a, *b; 
        memcpy(b,&a,10000000000);        
        printf("I should be in segfault here...\n");
    }    
    END_TESTING;
    
    
    // start testing without fork //
    TEST_FORK(0);
    
    BEGIN_TESTING(example3) {
        int c=1;        
        TEST0(c==0 && "false");
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
