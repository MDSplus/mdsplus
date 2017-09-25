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
/*	tdireffunction.h
	Internal/intrinsic function table reference.
	For insertion into TdiEvaluate TDI$$COMPILE and others.

	Ken Klare, LANL CTR-10	(c)1989
*/
#define TdiFUNCTION_DEF
#include <mdstypes.h>
#include <config.h>
struct TdiFunctionStruct {
  char *name;			/*string to compile/decompile   */
  int (*f1) ();			/*routine to check arguments    */
  int (*f2) ();			/*routine to set conversions    */
  int (*f3) ();			/*routine to do operation       */
  unsigned char i1;		/*input minimum cat             */
  unsigned char i2;		/*input maximum cat             */
  unsigned char o1;		/*output minimum cat            */
  unsigned char o2;		/*output maximum cat            */
  unsigned char m1;		/*minimum arguments             */
  unsigned char m2;		/*maximum arguments             */
  unsigned int token;		/*YACC-LEX token for this entry */
};

extern const int TdiFUNCTION_MAX;
extern const struct TdiFunctionStruct TdiRefFunction[];
