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
#include "mdsplus/mdsconfig.h"
#include "testing.h"
// Skip test in windows as fork is not supported yet //
#   ifdef _WIN32
    int main() { SKIP_TEST("current test is not performed on windows"); }
#else

#include <unistd.h>
#include <stdlib.h>

#ifdef HAVE_VALGRIND_H
# include <valgrind/valgrind.h>
#endif

int main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused)))
{
    
#if defined(__has_feature)
#  if __has_feature(address_sanitizer) || __has_feature(thread_sanitizer)
    SKIP_TEST("timeout test disabled on sanitizer");
#  endif
#endif

#if __SANITIZE_ADDRESS__ || __SANITIZE_THREAD__
    SKIP_TEST("timeout test disabled on sanitizer");
#endif

#ifdef HAVE_VALGRIND_H
   if(RUNNING_ON_VALGRIND)
       SKIP_TEST("timeout test disabled on valgrind");
#endif

    TEST_TIMEOUT(0.009);
    BEGIN_TESTING(timeout fail 10ms );
    usleep(1E4);
    END_TESTING;
    
    TEST_TIMEOUT(0.019);
    BEGIN_TESTING(timeout fail 20ms);
    usleep(2E4);
    END_TESTING;
     
}

#   endif    

