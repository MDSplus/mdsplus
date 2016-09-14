
#include <stdio.h>
#include <unistd.h>

#include "testing.h"

#include <stdlib.h>
#include <pthread.h>


void __assert_fail (const char *__assertion, const char *__file,
                    unsigned int __line, const char *__function)
{    
    printf(" .-----------------------------------. \n"
           " |  Assertion failed during test     | \n"
           " '-----------------------------------' \n"
           " > file: %s ,  function: %s, line: %d \n"
           "   assertion:  (%s) \n\n", 
           __file,__function,__line,__assertion);
    
    __test_end();
    exit(1);
}


void __test_abort(int code, const char *__msg, const char *__file,
                  unsigned int __line, const char *__function) 
{ 
    printf(" .-----------------------------------. \n"
           " |  TEST ABORTED                     | \n"
           " '-----------------------------------' \n"
           "  file: %s ,  function: %s, line: %d "
           "  message:  (%s) \n", 
           __file,__function,__line,__msg);
    exit(code); 
}


void __test_setfork(int value __attribute__ ((unused))) {}

void __test_init(const char *test_name __attribute__ ((unused)), const char *file __attribute__ ((unused)),
		 const int line __attribute__ ((unused))) {}

void __test_end() {}

int  __setup_parent() {return 0;}

int  __setup_child() {return 0;}

void __test_assert_fail(const char *file __attribute__ ((unused)), int line __attribute__ ((unused)),
			const char *expr __attribute__ ((unused)), ...) {}

void __mark_point(const char *__assertion __attribute__ ((unused)),
		  const char *__file __attribute__ ((unused)),
		  unsigned int __line __attribute__ ((unused)),
		  const char *__function __attribute__ ((unused))) {}

void __test_exit() { exit(0); }

void __test_timeout(double seconds) { (void)seconds; }








