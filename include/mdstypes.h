#ifndef MDSTYPES_H
#define MDSTYPES_H

#include <config.h>

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

#endif
