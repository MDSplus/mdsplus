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
#include <unistd.h>

#include "testing.h"

#include <stdlib.h>
#include <pthread.h>

#if defined(__GNUC__ ) || defined(__clang__)
#  define UNUSED_ARGUMENT __attribute__((__unused__))
#else
#  define UNUSED_ARGUMENT
#endif


void __assert_fail (const char *__assertion, const char *__file,
	            ASSERT_LINE_TYPE __line, const char *__function)
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


void __test_setfork(int value UNUSED_ARGUMENT) {}

void __test_init(const char *test_name UNUSED_ARGUMENT, const char *file UNUSED_ARGUMENT,
		 const int line UNUSED_ARGUMENT) {}

void __test_end() {}

int  __setup_parent() {return 0;}

int  __setup_child() {return 0;}

void __test_assert_fail(const char *file UNUSED_ARGUMENT, int line UNUSED_ARGUMENT,
			const char *expr UNUSED_ARGUMENT, ...) {}

void __mark_point(const char *__assertion UNUSED_ARGUMENT,
		  const char *__file UNUSED_ARGUMENT,
		  ASSERT_LINE_TYPE __line UNUSED_ARGUMENT,
		  const char *__function UNUSED_ARGUMENT) {}

void __test_exit() { exit(0); }

void __test_timeout(double seconds) { (void)seconds; }








