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
#include <stdint.h>

#define MdsEND_ARG ((void *)(uintptr_t)0xffffffff)
#ifdef va_count
#define MDS_END_ARG
#else
#define MDS_END_ARG ,MdsEND_ARG
#endif
#endif				/* MDS_STDARG */
