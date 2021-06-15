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
#ifndef MDS_STDARG
#define MDS_STDARG

#include <stdarg.h>
#include <stdint.h>

/* Should be save for descriptor args due to alignment.
 * !!! must match python/MDSplus/version.py !!!
 */
#define MdsEND_ARG ((void *)(intptr_t)1)
#define MDS_END_ARG , MdsEND_ARG

/* VA_LIST_TO_ARGLIST helps to unwrap the va_list into
 *
 * arglist:	target array of pointers
 * nargs:	int value returns the total number of args
 * pre:		offset in the arglist, so you can prepend your own
 * post:	added to nargs, so you can append some, if 1, last arg will be
 * <end> anchor:	last fix argument just before ... end:
 * terminating value, e.g. NULL or MdsEND_ARG
 */
#define VA_LIST_TO_ARGLIST(arglist, nargs, pre, post, anchor, end)         \
  {                                                                        \
    va_list args;                                                          \
    va_start(args, anchor);                                                \
    for (nargs = pre; nargs < (int)(sizeof(arglist) / sizeof(arglist[0])); \
         nargs++)                                                          \
      if (end == (arglist[nargs] = va_arg(args, void *)))                  \
        break;                                                             \
    va_end(args);                                                          \
    nargs += post;                                                         \
  }

// abbreviations for NULL and MdsEND_ARG terminated va_lists
#define VA_LIST_NULL(arglist, nargs, pre, post, anchor) \
  VA_LIST_TO_ARGLIST(arglist, nargs, pre, post, anchor, NULL)
#define VA_LIST_MDS_END_ARG(arglist, nargs, pre, post, anchor) \
  VA_LIST_TO_ARGLIST(arglist, nargs, pre, post, anchor, MdsEND_ARG)

#endif /* MDS_STDARG */
