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
/*******************************************************
 This module was generated using mdsshr/gen_messages.py
 To add new status messages modify
 mdsdcl_messages.xml
 and then do:
     python mdsshr/gen_messages.py
*******************************************************/

#pragma once
#include <status.h>

#define MdsdclSUCCESS            0x8020009
#define MdsdclEXIT               0x8020011
#define MdsdclERROR              0x8020018
#define MdsdclNORMAL             0x8020329
#define MdsdclPRESENT            0x8020331
#define MdsdclIVVERB             0x802033a
#define MdsdclABSENT             0x8020340
#define MdsdclNEGATED            0x8020348
#define MdsdclNOTNEGATABLE       0x8020352
#define MdsdclIVQUAL             0x802035a
#define MdsdclPROMPT_MORE        0x8020362
#define MdsdclTOO_MANY_PRMS      0x802036a
#define MdsdclTOO_MANY_VALS      0x8020372
#define MdsdclMISSING_VALUE      0x802037a
