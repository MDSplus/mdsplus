#ifndef MDSPLUS_MDSPLUS_H
#define MDSPLUS_MDSPLUS_H

#if defined (_WIN32)
#define MDS_WINDOWS
#define EXPORT __declspec(dllexport)

#elif defined (__APPLE__)
#define MDS_MAC
#define MDS_PTHREAD
#define EXPORT

#elif defined (__gnu_linux__)
#define MDS_LINUX
#define MDS_PTHREAD
#define EXPORT

#else
#define EXPORT
#endif

#endif
