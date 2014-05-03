#ifndef MDSPLUS_MDSPLUS_H
#define MDSPLUS_MDSPLUS_H

#if defined (_WIN32)
#define MDS_WINDOWS
#define EXPORT __declspec(dllexport)

#elif defined (__APPLE__)
#define MDS_MAC
#define EXPORT

#elif defined (__gnu_linux__)
#define EXPORT
#define MDS_LINUX

#else
#define EXPORT
#endif

#endif
