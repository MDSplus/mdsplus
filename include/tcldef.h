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
#ifdef __vms
#ifndef vms
#define vms    1
#endif
#endif

#ifndef __TCLDEF_H
#define __TCLDEF_H    1

#include        <stdio.h>
#include        <stdlib.h>


/***********************************************************************
* TCLDEF.H --
*
************************************************************************/

#define TCL_STS(N)     (TCL_FACILITY<<16)+N
#define TCL_STS_NORMAL            TCL_STS(1)
#define TCL_STS_FAILED_ESSENTIAL  TCL_STS(2)

#ifdef CREATE_STS_TEXT
struct stsText tcl_stsText[] = {
  STS_TEXT(TCL_STS_NORMAL, "Normal successful completion")
      , STS_TEXT(TCL_STS_FAILED_ESSENTIAL, "Essential action failed")
};
#endif

	/*****************************************************************
	 * Function prototypes:  user functions
	 *****************************************************************/
int TclSetTree();

#endif				/* __TCLDEF_H   */
