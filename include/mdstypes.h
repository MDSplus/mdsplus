#ifndef MDSTYPES_H

#define MDSTYPES_H

#ifdef _MSC_VER
#include <stdint.h>

#ifndef _OFF_T_DEFINED
typedef __int64 off_t;
typedef off_t _off_t;

#define _OFF_T_DEFINED
#endif

#include <msc_stdint.h>

#define _STDINT_H

#else

#include <stdint.h>

#endif

#endif /* MDSTYPES_H */
