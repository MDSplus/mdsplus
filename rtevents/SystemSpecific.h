#ifndef _SYSTEM_SPECIFIC_H
#define SYSTEM_SPECIFIC_H

#ifdef HAVE_WINDOWS_H
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
typedef long long _int64;
typedef unsigned long long _int64u;
#endif

#endif