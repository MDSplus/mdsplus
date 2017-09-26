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

		Name:   MdsGetCurrentShotId

		Type:   C function

		Author:	Thomas W. Fredian

		Date:   11-OCT-1989

		Purpose: Get current shot number

------------------------------------------------------------------------------

	Call sequence:

int MdsGetCurrentShotId(experiment,shot)

------------------------------------------------------------------------------
   Copyright (c) 1989
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <STATICdef.h>

int MdsGetCurrentShotId()
{
  printf("MdsGetCurrentShotId is obsolete, use TreeGetCurrentShotId in TreeShr\n");
  return 0;
}

int MdsSetCurrentShotId()
{
  printf("MdsSetCurrentShotId is obsolete, use TreeSetCurrentShotId in TreeShr\n");
  return 0;
}
