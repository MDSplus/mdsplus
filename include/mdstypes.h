#ifndef MDSTYPES_H
#define MDSTYPES_H

#ifndef HAVE_VXWORKS_H
#include <config.h>
#endif

/* temporary debugging fix */
#ifdef __APPLE__
#include <sys/types.h>
typedef int64_t _int64;
typedef u_int64_t _int64u;
#else
#ifdef HAVE_VXWORKS_H
typedef long long _int64;
typedef unsigned long long _int64u;
#else

#if (SIZEOF__INT64 != 8)
#if (SIZEOF_LONG == 8)
typedef long _int64;
typedef unsigned long _int64u;
#elif (SIZEOF_LONG_LONG == 8)
typedef long long _int64;
typedef unsigned long long _int64u;
#endif
#else
typedef unsigned _int64 _int64u;
#endif
#endif /*vxWorks*/
#endif /* __APPLE__ */



#endif
