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
/*      Tdi1ShowVm.C
        Show virtual memory zone
                SHOW_VM([print-level], [selection-mask])

        Ken Klare, LANL CTR-7   (c)1989,1990
*/

#include <STATICdef.h>
#include "tdirefstandard.h"
#include <libroutines.h>
#include <mdsshr_messages.h>



extern int TdiGetLong();

int Tdi1ShowVm()
{
  /*
  INIT_STATUS;
  int code, contex = 0, j = 1, mask, zone_id = 0;

  if (narg > 0 && list[0])
    status = TdiGetLong(list[0], &code);
  else
    code = 3;
  if (narg > 1 && list[1] && STATUS_OK)
    status = TdiGetLong(list[1], &mask);
  else
    mask = -1;
  while STATUS_OK {
    status = LibFindVmZone(&contex, &zone_id);
    if (STATUS_OK && zone_id && j & mask)
      status = LibShowVmZone(&zone_id, &code);
    j <<= 1;
  }
  if (status == LibNOTFOU)
    status = MDSplusSUCCESS;
  return status;
  */
  return 0;
}
