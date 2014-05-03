#ifndef MDSPLUS_MDSPLUS_H
#define MDSPLUS_MDSPLUS_H

#ifdef _WIN32
#define MDS_WINDOWS
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#ifdef __APPLE__
#define MDS_MAC
#endif

#ifdef __gnu_linux__
#define MDS_LINUX
#endif

#endif
