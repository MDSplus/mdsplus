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
/*  CMS REPLACEMENT HISTORY, Element MDS_STDARG.H */
/*  *8    19-OCT-1995 09:29:16 TWF "support platforms without va_count" */
/*  *7    19-OCT-1995 09:02:40 TWF "Add end argument macros" */
/*  *6    12-JUL-1995 16:14:57 TWF "va_count now included in stdarg" */
/*  *5    18-OCT-1994 16:30:00 TWF "No longer support VAXC" */
/*  *4    18-OCT-1994 16:28:44 TWF "No longer support VAXC" */
/*  *3    10-MAR-1993 09:30:53 JAS "use mds_stdarg.h" */
/*  *2    10-MAR-1993 09:21:20 JAS "add va_count for alpha" */
/*  *1     2-MAR-1993 15:12:46 TWF "Same as stdarg.h with va_count defined" */
/*  CMS REPLACEMENT HISTORY, Element MDS_STDARG.H */
#ifndef MDS_STDARG
#define MDS_STDARG

#include <stdarg.h>
#define MdsEND_ARG ((void *)0xffffffff)
#ifdef va_count
#define MDS_END_ARG
#else
#define MDS_END_ARG ,MdsEND_ARG
#endif
#endif				/* MDS_STDARG */
