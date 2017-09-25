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
/*
 This header was generated using mdsshr/gen_messages.py
 To add new status messages modify:
     mdsshr_messages.xml
 and then in mdsshr do:
     python gen_messages.py
*/
#pragma once
#include <status.h>

#define LibINSVIRMEM             0x158214
#define LibINVARG                0x158234
#define LibINVSTRDES             0x158224
#define LibKEYNOTFOU             0x1582fc
#define LibNOTFOU                0x158274
#define LibQUEWASEMP             0x1582ec
#define LibSTRTRU                0x158011
#define StrMATCH                 0x248419
#define StrNOMATCH               0x248208
#define StrNOELEM                0x248218
#define StrINVDELIM              0x248210
#define StrSTRTOOLON             0x248074
#define MDSplusWARNING           0x10000
#define MDSplusSUCCESS           0x10009
#define MDSplusERROR             0x10012
#define MDSplusFATAL             0x10024
#define SsSUCCESS                0x1
#define SsINTOVF                 0x47c
#define SsINTERNAL               -0x1
