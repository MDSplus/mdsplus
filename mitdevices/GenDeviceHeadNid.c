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
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element GEN_DEVICE$HEAD_NID.C */
/*  *1     7-OCT-1993 10:37:18 TWF "Return head nid of conglomerate" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element GEN_DEVICE$HEAD_NID.C */
#include <mdsdescrip.h>
#include <ncidef.h>
#include <treeshr.h>

EXPORT int GenDeviceHeadNid(struct descriptor *niddsc)
{
  int nid = *(int *)niddsc->pointer;
  static int idx;
  static NCI_ITM itmlst[] = { {sizeof(int), NciCONGLOMERATE_ELT, (unsigned char *)&idx, 0},
  {0, NciEND_OF_LIST, 0, 0}
  };
  TreeGetNci(nid, itmlst);
  return nid - idx + 1;
}
