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
/*      Tdi3Expt.C
        Data base info.

        Ken Klare, LANL CTR-7   (c)1990
*/
#include <stdio.h>
#include <string.h>
#include <mdsdescrip.h>
#include <dbidef.h>
#include <libroutines.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <STATICdef.h>



/*--------------------------------------------------------------
        Default path name.
*/
int Tdi3MdsDefault(struct descriptor *in_ptr __attribute__ ((unused)), struct descriptor_xd *out_ptr)
{
  char value[4096];
  STATIC_CONSTANT unsigned char dtype = (unsigned char)DTYPE_T;
  int retlen, status;
  struct dbi_itm lst[] = { {sizeof(value), DbiDEFAULT, 0, 0}
			   , {0, DbiEND_OF_LIST, 0, 0}
  };
  unsigned short len;
  lst[0].pointer = (unsigned char *)value;
  lst[0].return_length_address = &retlen;
  status = TreeGetDbi(lst);
  if STATUS_OK {
    len = (unsigned short)retlen;
    status = MdsGet1DxS(&len, &dtype, out_ptr);
  }
  if STATUS_OK
    memcpy(out_ptr->pointer->pointer, value, len);
  return status;
}

/*--------------------------------------------------------------
        Experiment name.
*/
int Tdi3Expt(struct descriptor *in_ptr __attribute__ ((unused)), struct descriptor_xd *out_ptr)
{
  char value[39 - 7];
  int retlen, status;
  STATIC_CONSTANT unsigned char dtype = (unsigned char)DTYPE_T;
  struct dbi_itm lst[] = { {sizeof(value), DbiNAME, 0, 0}
			   , {0, DbiEND_OF_LIST, 0, 0}
  };
  unsigned short len;
  lst[0].pointer = (unsigned char *)value;
  lst[0].return_length_address = &retlen;
  status = TreeGetDbi(lst);
  if STATUS_OK {
    len = (unsigned short)retlen;
    status = MdsGet1DxS(&len, &dtype, out_ptr);
  }
  if STATUS_OK
    memcpy(out_ptr->pointer->pointer, value, len);
  return status;
}

/*--------------------------------------------------------------
        Shot number identifier.
*/
int Tdi3Shot(struct descriptor *in_ptr __attribute__ ((unused)), struct descriptor_xd *out_ptr)
{
  int value;
  int retlen, status;
  STATIC_CONSTANT unsigned char dtype = (unsigned char)DTYPE_L;
  struct dbi_itm lst[] = { {sizeof(value), DbiSHOTID, 0, 0}
			   , {0, DbiEND_OF_LIST, 0,  0}
  };
  unsigned short len;
  lst[0].pointer = (unsigned char *)&value;
  lst[0].return_length_address = &retlen;
  status = TreeGetDbi(lst);
  if STATUS_OK {
    len = (unsigned short)retlen;
    status = MdsGet1DxS(&len, &dtype, out_ptr);
  }
  if STATUS_OK
    *(int *)out_ptr->pointer->pointer = value;
  return status;
}

/*--------------------------------------------------------------
        Shot number identifier converted to string.
*/
int Tdi3Shotname(struct descriptor *in_ptr __attribute__ ((unused)), struct descriptor_xd *out_ptr)
{
  int value;
  int retlen, status;
  struct dbi_itm lst[] = { {sizeof(value), DbiSHOTID, 0, 0}
			   , {0, DbiEND_OF_LIST, 0, 0}
  };
  DESCRIPTOR(dmodel, "MODEL");
  char string[15];
  lst[0].pointer = (unsigned char *)&value;
  lst[0].return_length_address = &retlen;
  status = TreeGetDbi(lst);
  if (value != -1) {
    sprintf(string, "%d", value);
    dmodel.pointer = string;
    dmodel.length = (unsigned short)strlen(string);
  }
  if STATUS_OK
    status = MdsCopyDxXd((struct descriptor *)&dmodel, out_ptr);
  return status;
}
