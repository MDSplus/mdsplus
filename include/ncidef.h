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
/**************************************
  NCIDEF.H - definitions of constants
  used in  item list arguments to the
  routines:

    TreeGetNci
    TreeSetNci
**************************************/

#define NciEND_OF_LIST          0
#define NciSET_FLAGS            1
#define NciCLEAR_FLAGS          2
#define   NciM_STATE	           0x00000001
#define   NciM_PARENT_STATE        0x00000002
#define   NciM_ESSENTIAL           0x00000004
#define   NciM_CACHED              0x00000008
#define   NciM_VERSIONS            0x00000010
#define   NciM_SEGMENTED           0x00000020
#define   NciM_SETUP_INFORMATION   0x00000040
#define   NciM_WRITE_ONCE          0x00000080
#define   NciM_COMPRESSIBLE        0x00000100
#define   NciM_DO_NOT_COMPRESS     0x00000200
#define   NciM_COMPRESS_ON_PUT     0x00000400
#define   NciM_NO_WRITE_MODEL      0x00000800
#define   NciM_NO_WRITE_SHOT       0x00001000
#define   NciM_PATH_REFERENCE      0x00002000
#define   NciM_NID_REFERENCE       0x00004000
#define   NciM_INCLUDE_IN_PULSE    0x00008000
#define   NciM_COMPRESS_SEGMENTS   0x00010000

#define NciTIME_INSERTED          4
#define NciOWNER_ID	          5
#define NciCLASS	          6
#define NciDTYPE	          7
#define NciLENGTH	          8
#define NciSTATUS	          9
#define NciCONGLOMERATE_ELT	 10
#define NciGET_FLAGS		 12
#define NciNODE_NAME		 13
#define NciPATH		         14
#define NciDEPTH		 15
#define NciPARENT		 16
#define NciBROTHER		 17
#define NciMEMBER		 18
#define NciCHILD		 19
#define NciPARENT_RELATIONSHIP   20
#define NciK_IS_CHILD             1
#define NciK_IS_MEMBER            2
#define NciCONGLOMERATE_NIDS     21
#define NciORIGINAL_PART_NAME    22
#define NciNUMBER_OF_MEMBERS     23
#define NciNUMBER_OF_CHILDREN    24
#define NciMEMBER_NIDS           25
#define NciCHILDREN_NIDS         26
#define NciFULLPATH		 27
#define NciMINPATH		 28
#define NciUSAGE		 29
#define NciPARENT_TREE	         30
#define NciRLENGTH	 	 31
#define NciNUMBER_OF_ELTS	 32
#define NciDATA_IN_NCI	         33
#define NciERROR_ON_PUT          34
#define NciRFA		         35
#define NciIO_STATUS             36
#define NciIO_STV                37
#define NciDTYPE_STR             38
#define NciUSAGE_STR             39
#define NciCLASS_STR             40
#define NciVERSION               41
typedef struct nci_itm {
  short int buffer_length;
  short int code;
  void *pointer;
  int *return_length_address;
} NCI_ITM;
