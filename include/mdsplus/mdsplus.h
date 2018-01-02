#ifndef MDSPLUS_MDSPLUS_H
#define MDSPLUS_MDSPLUS_H

#if defined (_WIN32)
#define MDS_WINDOWS

// Workarounds for the broken windows compilers
#ifndef NOMINMAX
# define NOMINMAX
#endif
#define __func__ __FUNCTION__

#elif defined (__APPLE__)
#define MDS_MAC
#define MDS_PTHREAD

#elif defined (__linux__) && defined(__GNUC__)
#define MDS_LINUX
#define MDS_PTHREAD

#endif

#if defined(_WIN32)
#define MDS_API_IMPORT __declspec(dllimport)
#define MDS_API_EXPORT __declspec(dllexport)
#define MDS_API_HIDDEN
#ifdef __GNUC__
#if __GNUC__ >= 7
#define MDS_ATTR_FALLTHROUGH __attribute__((fallthrough));
#else
#define MDS_ATTR_FALLTHROUGH
#endif
#endif
#else
#if __GNUC__ >= 4
#define MDS_API_IMPORT __attribute__ ((visibility ("default")))
#define MDS_API_EXPORT __attribute__ ((visibility ("default")))
#define MDS_API_HIDDEN __attribute__ ((visibility ("hidden")))
#else
#define MDS_API_IMPORT
#define MDS_API_EXPORT
#define MDS_API_HIDDEN
#endif
#if __GNUC__ >= 7
#define MDS_ATTR_FALLTHROUGH __attribute__((fallthrough));
#else
#define MDS_ATTR_FALLTHROUGH
#endif
#endif

#ifdef MDS_SHARED		// defined if MDS is compiled as a shared library
#define MDS_API_LOCAL MDS_API_HIDDEN
#ifdef MDS_API_BUILD		// defined if we are building the MDS library instead of using it
#define MDS_API MDS_API_EXPORT
#else
#define MDS_API MDS_API_IMPORT
#endif
#else				// static library
#define MDS_API
#define MDS_API_LOCAL
#endif

#ifndef MDS_DEBUG_ACCESS
#define MDS_DEBUG_ACCESS
#endif

#endif
