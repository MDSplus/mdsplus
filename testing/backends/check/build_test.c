#include <stdio.h>

#include <check.h>
#include "check_msg.h"


void eprintf(const char *fmt, const char *file, int line,
             ...) CK_ATTRIBUTE_NORETURN;

int main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused)))
{

    setup_messaging();
    
    eprintf("build",__FILE__,__LINE__);
    
    return 0;
}
