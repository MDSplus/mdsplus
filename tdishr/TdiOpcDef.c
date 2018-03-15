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
/*  CMS REPLACEMENT HISTORY, Element TDI$$OPC_DEF.C */
/*  *6    27-JUN-1996 09:10:25 TWF "Port to Unix/Windows" */
/*  *5    25-JUN-1996 08:44:12 TWF "Port to Unix/Windows" */
/*  *4    18-OCT-1995 11:58:53 TWF "make all includes the same case" */
/*  *3     2-NOV-1994 08:51:05 TWF "Change extern model" */
/*  *2    28-OCT-1994 16:23:53 TWF "split out opc$_xx and opc$xx" */
/*  *1    25-OCT-1994 08:22:42 TWF "Define Opc_function and Opcfunction symbols" */
/*  CMS REPLACEMENT HISTORY, Element TDI$$OPC_DEF.C */
#include <mdsplus/mdsconfig.h>
#define COM
#define OPC(name,builtin,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11) EXPORT const unsigned short Opc##name = __LINE__ - 25;
#include "opcbuiltins.h"
#undef OPC
