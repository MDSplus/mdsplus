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
#pragma once
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element DBIDEF.H */
/*  *3     1-APR-1991 17:29:45 TWF "Add readonly open" */
/*  *2    16-FEB-1990 10:28:18 TWF "Add itmlst struct" */
/*  *1    28-FEB-1989 14:55:59 TWF "Item codes for TreeGetDbi" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element DBIDEF.H */
/**********************************
 Item list codes for TreeGetDbi
**********************************/

#define DbiEND_OF_LIST	0	/* End of list */
#define DbiNAME		1	/* Experiment name used for open - text string */
#define DbiSHOTID		2	/* Shot identification - longword */
#define DbiMODIFIED		3	/* True if tree has been modified during edit - boolean */
#define DbiOPEN_FOR_EDIT      4	/* True if tree is open for edit - boolean */
#define DbiINDEX		5	/* Index of tree to use for subsequent information requests */
#define DbiNUMBER_OPENED	6	/* Number of trees currently open */
#define DbiMAX_OPEN		7	/* Maximum number of tree allowed open at one time */
#define DbiDEFAULT	        8	/* NID of default node */
#define DbiOPEN_READONLY      9	/* True if tree has been opened readonly */
#define DbiVERSIONS_IN_MODEL 10	/* True if using versioning in model */
#define DbiVERSIONS_IN_PULSE 11	/* True if using versioning in pulse files */
#define DbiREADONLY          12 /* True if making tree readonly */

typedef struct dbi_itm {
  short int buffer_length;
  short int code;
  void *pointer;
  int *return_length_address;
} DBI_ITM;
