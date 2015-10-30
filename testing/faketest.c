
#include <string.h>
#include <unistd.h>
#include <testing.h>




int main(int argc, char *argv[])
{
    if(argc == 1) {
        BEGIN_TESTING(Dummy test);
        END_TESTING;
        _exit(0);
    }            

    if(!strcmp(argv[1],"PASS")) {
        BEGIN_TESTING(Dummy test);
        TEST1("# PASS this test was forced to exit with no errors. " || 1);
        END_TESTING;
        _exit(0);
    }            
    
    if(!strcmp(argv[1],"XPASS")) {
        BEGIN_TESTING(Dummy test);
        TEST0("# XPASS this test was forced to exit with errors. " || 1);
        END_TESTING;
        _exit(1);
    }            
    
    if(!strcmp(argv[1],"FAIL")) {
        BEGIN_TESTING(Dummy test);
        TEST0("# FAIL this test was forced to exit with errors. " || 1);
        END_TESTING;
        _exit(1);
    }            
    
    if(!strcmp(argv[1],"XFAIL")) {
        BEGIN_TESTING(Dummy test);
        TEST1("# FAIL this test was forced to exit with no errors. " || 1);
        END_TESTING;
        _exit(0);
    }            
        
    if(!strcmp(argv[1],"SKIP") ) {
        BEGIN_TESTING(Dummy test);                
        TEST1("# SKIP this test was intentionally skipped. " || 1);        
        END_TESTING;
        _exit(77);
    }
    
    if(!strcmp(argv[1],"ERROR") ) {
        BEGIN_TESTING(Dummy test);                
        TEST0("# ERROR this test was intentionally exit in error state. " || 1);        
        END_TESTING;
        _exit(99);
    }
}


