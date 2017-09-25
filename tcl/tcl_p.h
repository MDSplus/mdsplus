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
#ifndef TCL_P
#define TCL_P


typedef enum { on_off, rename_node, delete, new, tree, set_def } NodeTouchType;

extern void TclSetCallbacks(		/* Returns: void                        */
		      void (*error_out) ()	/* <r> addr of error output routine */
		      , void (*text_out) ()	/* <r> addr of normal text output routine */
		      , void (*node_touched) ()	/* <r> addro of "node touched" routine      */
    );
extern void TclNodeTouched(		/* Returns: void                        */
		     int nid	/* <r> node id                          */
		     , NodeTouchType type	/* <r> type of "touch"                  */
    );

extern char tclUsageToNumber(const char *usage, char **error);
extern int tclStringToShot(char *str, int *shot, char **error);
extern void tclAppend(char **output, char *string);

#endif
