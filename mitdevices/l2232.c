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

		Name:   L2232$ROUTINES   

		Type:   C function

     		Author:	Josh Stillerman

		Date:   26-JUL-1993

    		Purpose: Support for L2232 32 channel Differential ADC

------------------------------------------------------------------------------

	Call sequence: 

EXPORT int L2232__INIT(struct descriptor *niddsc_ptr __attribute__ ((unused)), InInitStruct *setup)
EXPORT int L2232__STORE(struct descriptor *niddsc_ptr __attribute__ ((unused)), InStoreStruct *setup)

------------------------------------------------------------------------------
   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

 	Description:

------------------------------------------------------------------------------*/
#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>
#include <treeshr.h>
#include <mdsshr.h>




#include "l2232_gen.h"
#include "devroutines.h"
/*------------------------------------------------------------------------------

 External functions or symbols referenced:                                    */

/*------------------------------------------------------------------------------

 Subroutines referenced:                                                      */

/*------------------------------------------------------------------------------

 Macros:                                                                      */

/*------------------------------------------------------------------------------

 Global variables:                                                            */

/*------------------------------------------------------------------------------

 Local variables:                                                             */

static int one = 1;

/*------------------------------------------------------------------------------

 Executable:                                                                  */

EXPORT int l2232___init(struct descriptor *niddsc_ptr __attribute__ ((unused)), InInitStruct * setup)
{

#define return_on_error(f,retstatus) if (!((status = f) & 1)) {return retstatus; }

  int status;

  return_on_error(DevCamChk(CamPiow(setup->cts_name, 0, 9, 0, 16, 0), &one, 0), status);
  return_on_error(DevCamChk(CamPiow(setup->cts_name, 0, 26, 0, 16, 0), &one, 0), status);
  return status;
}

EXPORT int l2232___store(struct descriptor *niddsc_ptr __attribute__ ((unused)), InStoreStruct * setup)
{
  unsigned short dummy;
  int nid;
  int status;
  static short buffer[32];
  static DESCRIPTOR_A(a_dsc, sizeof(buffer[0]), DTYPE_W, buffer, sizeof(buffer));
  static DESCRIPTOR(expr, "BUILD_SIGNAL($VALUE*.0024, $, 1 : 32 : 1)");
  static struct descriptor_xd sig = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };

  return_on_error(DevCamChk(CamPiow(setup->cts_name, 0, 8, 0, 16, 0), &one, 0), status);
  if ((CamXandQ(0) & 1) == 0)
    return DEV$_NOT_TRIGGERED;

  return_on_error(DevCamChk(CamPiow(setup->cts_name, 0, 2, &dummy, 16, 0), &one, 0), status);
  return_on_error(DevCamChk(CamStopw(setup->cts_name, 0, 2, 32, buffer, 16, 0), &one, 0), status);
  return_on_error(DevCamChk(CamPiow(setup->cts_name, 0, 2, &dummy, 16, 0), &one, 0), status);

  return_on_error(TdiCompile((struct descriptor *)&expr, &a_dsc, (struct descriptor *)&sig MDS_END_ARG), status);
  nid = setup->head_nid + L2232_N_INPUTS;
  return_on_error(TreePutRecord(nid, (struct descriptor *)&sig, 0), status);
  MdsFree1Dx(&sig, 0);
  return 1;
}
