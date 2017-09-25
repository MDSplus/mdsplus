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
     tdishr_messages.xml
 and then in mdsshr do:
     python gen_messages.py
*/
#pragma once
#include <status.h>

#define TdiBREAK                 0xfd38008
#define TdiCASE                  0xfd38010
#define TdiCONTINUE              0xfd38018
#define TdiEXTRANEOUS            0xfd38020
#define TdiRETURN                0xfd38028
#define TdiABORT                 0xfd38032
#define TdiBAD_INDEX             0xfd3803a
#define TdiBOMB                  0xfd38042
#define TdiEXTRA_ARG             0xfd3804a
#define TdiGOTO                  0xfd38052
#define TdiINVCLADSC             0xfd3805a
#define TdiINVCLADTY             0xfd38062
#define TdiINVDTYDSC             0xfd3806a
#define TdiINV_OPC               0xfd38072
#define TdiINV_SIZE              0xfd3807a
#define TdiMISMATCH              0xfd38082
#define TdiMISS_ARG              0xfd3808a
#define TdiNDIM_OVER             0xfd38092
#define TdiNO_CMPLX              0xfd3809a
#define TdiNO_OPC                0xfd380a2
#define TdiNO_OUTPTR             0xfd380aa
#define TdiNO_SELF_PTR           0xfd380b2
#define TdiNOT_NUMBER            0xfd380ba
#define TdiNULL_PTR              0xfd380c2
#define TdiRECURSIVE             0xfd380ca
#define TdiSIG_DIM               0xfd380d2
#define TdiSYNTAX                0xfd380da
#define TdiTOO_BIG               0xfd380e2
#define TdiUNBALANCE             0xfd380ea
#define TdiUNKNOWN_VAR           0xfd380f2
#define TdiSTRTOOLON             0xfd380fc
#define TdiTIMEOUT               0xfd38104
