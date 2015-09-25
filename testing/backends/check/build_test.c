#include <stdio.h>

#include <check.h>
#include "check_msg.h"


void eprintf(const char *fmt, const char *file, int line,
             ...) CK_ATTRIBUTE_NORETURN;

int main(int argc, char *argv[])
{

    setup_messaging();
    
    eprintf("build",__FILE__,__LINE__);
    
    return 0;
}
