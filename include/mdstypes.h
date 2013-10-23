#ifndef MDSTYPES_H

#define MDSTYPES_H
#include <config.h>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#else /* HAVE_STDINT_H */
#ifdef HAVE_WINDOWS_H
typedef __int64 off_t;
typedef off_t _off_t;
#define _OFF_T_DEFINED
#include <msc_stdint.h>
#define _STDINT_H
#else /* HAVE_WINDOWS_H */
#if (SIZEOF__INT64 != 8)
#if (SIZEOF_LONG == 8)
typedef long int64_t;
typedef unsigned long uint64_t;
#elif (SIZEOF_LONG_LONG == 8)
typedef long long int64_t;
typedef unsigned long long uint64_t;
#endif /* SIZEOF_LONG */
#else /*SIZEOF_INT64 */
typedef unsigned int64_t uint64_t;
#endif /* SIZEOF_INT64 */
#endif /* HAVE_WINDOWS_H */
#endif /* HAVE_STDINT_H */
#endif /* MDSTYPES_H */
