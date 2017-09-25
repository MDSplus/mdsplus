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
// macros.h
// Thu May 24 11:10:48 EDT 2001
// Thu May 24 13:07:23 EDT 2001 -- added KineticSystems macros
// Mon Jan 28 12:43:12 EST 2002 -- added numeric digit count
// Tue Feb  5 15:07:26 EST 2002 -- cast 'NumOfDigits()' to int
#ifndef __MACROS_H
#define __MACROS_H

// general purpose
#define	ASCII(a)		(a) + '0'
#define Creat(a,b)		creat(get_file_name((a)),(b))
#define NumOfDigits(x)	(int)( floor(log10((float)(x))) + 1 )
#define	Fopen(a,b)		fopen(get_file_name((a)),(b))
#define	NUMERIC(a)		(a) - '0'
#define	Open(a,b)		open(get_file_name((a)),(b))
#define	Remove(a)		remove(get_file_name((a)))
#define	Stat(a,b)		stat(get_file_name((a)),(b))
#define Shorten(a)		((unsigned short)((a) & 0x0ffff))

// KineticSystems specific macros
// NB! order of members in struct NAF is f,a,n !!
#define	NAFhi(n,a,f)	( ( (n)        << 1) |  (((a) & 0x8)         >> 3) )
#define	NAFlo(n,a,f)	( (((a) & 0x7) << 5) |    (f)                      )
#define	MODE(m,ws)		( ( (m)        << 3) | ((((ws)==16) ? 1 : 0) << 1) )

#endif
