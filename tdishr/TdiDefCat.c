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
/*      TDI$$DEF_CAT.C
        Data type information table definitions.
        Used by Tdi1Same and others.

        requirements for type matching
        maximum real size minus 1
        (Some significance can be lost:
        Unsigned to signed loses 1 bit,
        L to F and Q to D lose 7 bits,
        Q to G loses 10 bits,
        O to H loses 15 bits, and
        D to G loses 3 bits.)

        Ken Klare, LANL CTR-7   (c)1989,1990
*/
#include <STATICdef.h>
#include "tdirefcat.h"

#define F_SYM   "F"
#define FS_SYM  "E"
#define FT_SYM  "D"
#define G_SYM   "G"
#define D_SYM   "V"

const struct TdiCatStruct_table TdiREF_CAT[] = {
  {"MISSING", 0, 0, 0, 0},	/*0 Z=unsecified */
  {"V", 0x7fff, 0, 0, 0},	/*1 aligned bit string (bits) */
  {"BU", 0x8100, 1, 4, 0},	/*2 byte unsigned */
  {"WU", 0x8101, 2, 8, 0},	/*3 byte unsigned */
  {"LU", 0x8103, 4, 12, 0},	/*4 long unsigned */
  {"QU", 0x8107, 8, 20, 0},	/*5 quadword unsigned */
  {"B", 0x8300, 1, 4, 0},	/*6 byte */
  {"W", 0x8301, 2, 8, 0},	/*7 word */
  {"L", 0x8303, 4, 12, 0},	/*8 long */
  {"Q", 0x8307, 8, 20, 0},	/*9 quadword */
  {"F", 0x8703, 4, 16, F_SYM},	/*10 f-real */
  {"D", 0x8707, 8, 24, D_SYM},	/*11 d-real */
  {"FC", 0x9703, 8, 32, F_SYM},	/*12 f-complex */
  {"DC", 0x9707, 16, 48, D_SYM},	/*13 d-complex */
  {"T", 0x8000, 0, 0, 0},	/*14 text */
  {"NU", 0x7fff, 0, 0, 0},	/*15 numeric unsigned */
  {"NL", 0x7fff, 0, 0, 0},	/*16 numeric left separate */
  {"NLO", 0x7fff, 0, 0, 0},	/*17 numeric left overpunched */
  {"NR", 0x7fff, 0, 0, 0},	/*18 numeric right separate */
  {"NRO", 0x7fff, 0, 0, 0},	/*19 numeric right overpunched */
  {"NZ", 0x7fff, 0, 0, 0},	/*20 numeric zoned */
  {"NP", 0x7fff, 0, 0, 0},	/*21 numeric packed (nibbles) */
  {"ZI", 0x7fff, 0, 0, 0},	/*22 sequence of instructions */
  {"ZEM", 0x7fff, 2, 0, 0},	/*23 procedure entry mask */
  {"DSC", 0x7fff, 8, 0, 0},	/*24 descriptor */
  {"OU", 0x810f, 16, 36, 0},	/*25 octaword unsigned */
  {"O", 0x830f, 16, 36, 0},	/*26 octaword */
  {"G", 0x8707, 8, 24, G_SYM},	/*27 g-real */
  {"H", 0x8f0f, 16, 40, "H"},	/*28 h-real */
  {"GC", 0x9707, 16, 48, G_SYM},	/*29 g-complex */
  {"HC", 0x9f0f, 0, 0, 0},	/*30 h-complex */
  {"CIT", 0x7fff, 10, 0, 0},	/*31 cobol intermediate temporary */
  {"BPV", 0x7fff, 8, 0, 0},	/*32 bound procedure value */
  {"BLV", 0x7fff, 8, 0, 0},	/*33 bound label value */
  {"VU", 0x7fff, 0, 0, 0},	/*34 unaligned bit string (bits) */
  {"ADT", 0x7fff, 8, 23, 0},	/*35 absolute date time */
  {"36", 0x7fff, 0, 0, 0},	/*36 */
  {"VT", 0x7fff, 0, 0, 0},	/*37 varying character string */
  {"?", 0x7fff, 0, 0, 0},	/*38 ? */
  {"?", 0x7fff, 0, 0, 0},	/*39 ? */
  {"?", 0x7fff, 0, 0, 0},	/*40 ? */
  {"?", 0x7fff, 0, 0, 0},	/*41 ? */
  {"?", 0x7fff, 0, 0, 0},	/*42 ? */
  {"?", 0x7fff, 0, 0, 0},	/*43 ? */
  {"?", 0x7fff, 0, 0, 0},	/*44 ? */
  {"?", 0x7fff, 0, 0, 0},	/*45 ? */
  {"?", 0x7fff, 0, 0, 0},	/*46 ? */
  {"?", 0x7fff, 0, 0, 0},	/*47 ? */
  {"?", 0x7fff, 0, 0, 0},	/*48 ? */
  {"?", 0x7fff, 0, 0, 0},	/*49 ? */
  {"?", 0x7fff, 0, 0, 0},	/*50 ? */
  {"P", 0x7fff, sizeof(void *), 36, 0},	/*51 POINTER */
  {"FS", 0x8703, 4, 16, FS_SYM},	/*52 ieee single floating */
  {"FT", 0x8707, 8, 32, FT_SYM},	/*53 ieee double floating */
  {"FSC", 0x9703, 8, 24, FS_SYM},	/*54 ieee single floating complex */
  {"FTC", 0x9707, 16, 48, FT_SYM}	/*55 ieee double floating complex */
};

const unsigned char TdiCAT_MAX = sizeof(TdiREF_CAT) / sizeof(struct TdiCatStruct_table);
