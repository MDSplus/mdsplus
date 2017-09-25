/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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
