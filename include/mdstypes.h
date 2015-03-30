#ifndef MDSTYPES_H

#define MDSTYPES_H

#include <config.h>

#ifdef _MSC_VER
#undef HAVE_STDINT_H
#endif

#ifdef HAVE_STDINT_H

#include <stdint.h>

#else				/* HAVE_STDINT_H */

#ifdef _WIN32

#ifndef _OFF_T_DEFINED
typedef __int64 off_t;
typedef off_t _off_t;

#define _OFF_T_DEFINED
#endif

#include <msc_stdint.h>

#define _STDINT_H

#else				/* _WIN32 */

#if (SIZEOF__INT64 != 8)

#if (SIZEOF_LONG == 8)

typedef long int64_t;

typedef unsigned long uint64_t;

#elif (SIZEOF_LONG_LONG == 8)

typedef long long int64_t;

typedef unsigned long long uint64_t;

#endif				/* SIZEOF_LONG */

#else				/*SIZEOF_INT64 */

typedef unsigned int64_t uint64_t;

#endif				/* SIZEOF_INT64 */

#endif				/* _WIN32 */

#endif				/* HAVE_STDINT_H */

#endif				/* MDSTYPES_H */
