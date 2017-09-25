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
/*  USAGEDEF.H ************************************

Node Usage definitions

**************************************************/

#ifndef USAGEDEF_H

#define TreeUSAGE_ANY       0
#define TreeUSAGE_NONE      1 /******* USAGE_NONE is now USAGE_STRUCTURE *******/
#define TreeUSAGE_STRUCTURE 1
#define TreeUSAGE_ACTION    2
#define TreeUSAGE_DEVICE    3
#define TreeUSAGE_DISPATCH  4
#define TreeUSAGE_NUMERIC   5
#define TreeUSAGE_SIGNAL    6
#define TreeUSAGE_TASK      7
#define TreeUSAGE_TEXT      8
#define TreeUSAGE_WINDOW    9
#define TreeUSAGE_AXIS     10
#define TreeUSAGE_SUBTREE  11
#define TreeUSAGE_COMPOUND_DATA 12
#define TreeUSAGE_MAXIMUM  TreeUSAGE_COMPOUND_DATA
#define TreeUSAGE_SUBTREE_REF 14 /* Runtime only special usage */
#define TreeUSAGE_SUBTREE_TOP 15 /* Runtime only special usage */

#endif
