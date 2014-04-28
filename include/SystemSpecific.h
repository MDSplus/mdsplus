#ifndef MDSPLUS_SYSTEM_SPECIFIC_H
#define MDSPLUS_SYSTEM_SPECIFIC_H

#include <mdstypes.h>

#ifdef HAVE_WINDOWS_H
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#endif
