// macros.h
// Thu May 24 11:10:48 EDT 2001
// Thu May 24 13:07:23 EDT 2001 -- added KineticSystems macros
// Mon Jan 28 12:43:12 EST 2002 -- added numeric digit count
// Tue Feb  5 15:07:26 EST 2002 -- cast 'NumOfDigits()' to int
#ifndef __MACROS_H
#define __MACROS_H

// general purpose
#define ASCII(a) (a) + '0'
#define Creat(a, b) creat(get_file_name((a)), (b))
#define NumOfDigits(x) (int)(floor(log10((float)(x))) + 1)
#define Fopen(a, b) fopen(get_file_name((a)), (b))
#define NUMERIC(a) (a) - '0'
#define Open(a, b) open(get_file_name((a)), (b))
#define Remove(a) remove(get_file_name((a)))
#define Stat(a, b) stat(get_file_name((a)), (b))
#define Shorten(a) ((unsigned short)((a)&0x0ffff))

// KineticSystems specific macros
// NB! order of members in struct NAF is f,a,n !!
#define NAFhi(n, a, f) (((n) << 1) | (((a)&0x8) >> 3))
#define NAFlo(n, a, f) ((((a)&0x7) << 5) | (f))
#define MODE(m, ws) (((m) << 3) | ((((ws) == 16) ? 1 : 0) << 1))

#endif
