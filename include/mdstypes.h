#ifndef MDSTYPES_H
#define MDSTYPES_H

#include <config.h>

#if (SIZEOF__INT64 != 8)
#if (SIZEOF_LONG == 8)
typedef long _int64;
#elif (SIZEOF_LONG_LONG == 8)
typedef long long _int64;
#endif
#endif

#endif
