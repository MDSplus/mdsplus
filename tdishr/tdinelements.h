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
/*	tdinelements.h
	Internal routine for number of elements.

	Ken Klare, LANL P-4	(c)1989,1991
*/
#define N_ELEMENTS(dsc_ptr, count) \
	if (dsc_ptr->dtype == DTYPE_MISSING) {\
		count = 0;\
	} \
	else {\
		switch (dsc_ptr->class) {\
		default : count = 0; status = TdiINVCLADSC; break;\
		case CLASS_A : {				  \
		  array_coeff *aptr = (array_coeff *)dsc_ptr; \
		  count = 0; \
		  if (aptr->length != 0) { \
		    count = aptr->arsize / aptr->length; \
	          } else if ( aptr->dtype == DTYPE_T && aptr->aflags.coeff == 1 ) { \
		    int i; \
		    for (i=0;i<aptr->dimct;i++) { \
		      if (count == 0) count=aptr->m[i]; else count=count*aptr->m[i]; \
		    }\
		  }\
		  break;\
 	        }\
		case CLASS_S : case CLASS_D : count = 1; break;\
		} \
	}
