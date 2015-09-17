
#include <stdio.h>
#include <unistd.h>

#include "testing.h"




void __assert_fail (const char *__assertion, const char *__file,
                    unsigned int __line, const char *__function)
{    
    printf(" .-----------------------------------. \n"
           " |  Assertion failed during test     | \n"
           " '-----------------------------------' \n"
           " > file: %s ,  function: %s, line: %d \n"
           "   assertion:  (%s) \n\n", 
           __file,__function,__line,__assertion);
    _exit(1);
}




void __test_setfork(int value) {}

void __test_init(const char *test_name, const char *file, const int line) {}

void __test_end() {}

int  __setup_parent() {}

int  __setup_child() {}

void __test_assert_fail(const char *file, int line, const char *expr, ...) {}

void __mark_point(const char *__assertion, const char *__file, unsigned int __line, const char *__function) {}

void __test_exit() { _exit(0); }








