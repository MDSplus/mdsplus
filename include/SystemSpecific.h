#ifndef MDSPLUS_SYSTEM_SPECIFIC_H
#define MDSPLUS_SYSTEM_SPECIFIC_H

#include <mdstypes.h>

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#endif
