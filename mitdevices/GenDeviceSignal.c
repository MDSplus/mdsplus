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
/*------------------------------------------------------------------------------

		Name:   GEN_DEVICE$SIGNAL   

		Type:   C function

     		Author:	Giulio Fregonese

		Date:    6-OCT-90

    		Purpose: Perform device specific error handling 

------------------------------------------------------------------------------
   
   Work done by 1999 Informatica Ricerca Sviluppo, s.c.r.l.,
   under contract from Istituto Gas Ionizzati del CNR - Padova (Italy)
   
--------------------------------------------------------------------------- */
#include <treeshr.h>
#include <stdio.h>

extern int GenDeviceCallData();

EXPORT int GenDeviceSignal(int head_nid, unsigned int status_id_1 __attribute__ ((unused)), unsigned int status_id_2)
{
  char *pathname = 0;
  pathname = TreeGetPath(head_nid);
  if (pathname != 0) {
    printf("Error processing device %s\n", pathname);
    TreeFree(pathname);
  }
  return (status_id_2);
}
