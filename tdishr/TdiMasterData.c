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
/*      TdiMasterData.C
        Use first units to enclose the data and
        Find the correct signal to master the new data.
        Note, cmode is -1 if none is a signal,
        0 if signal 0 is chosen, 1 if signal 1, etc.
        With all non-signals, we get non-signal result.
        With one signal, that signal is used.
        With multiple signals, use smaller array (TdiGetShape).
        VMS*VMS => VMS, signal*VMS => signal, signal*param => signal.
        THINK, signal*signal => smaller_signal
        NEED, param*VMS?. Scalar-signal * vector-other => signal-vector?
        Used by Tdi1Build Tdi1Same Tdi1Trim Tdi1SetRange Tdi1Vector

        Ken Klare LANL P-4      (c)1989,1990,1991
*/
#include <STATICdef.h>
#include <status.h>
#include "tdirefstandard.h"
#include <mdsshr.h>
#include <stdlib.h>



STATIC_CONSTANT EMPTYXD(emptyxd);

int TdiMasterData(int nsig,
		  struct descriptor_xd sig[1],
		  struct descriptor_xd uni[1], int *cmode_ptr, struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor *pdu = uni[0].pointer;
  int cmode = -1, j;

	/******************
        Kill units if none.
        ******************/
  if (pdu && pdu->dtype == DTYPE_T && pdu->length == 1 && *pdu->pointer == ' ')
    pdu = 0;
	/*************************************************
        cmode = unsignaled, +n for signal
        *************************************************/
  for (j = 0; j < nsig; j++)
    if (sig[j].pointer) {
      if (cmode >= 0) {
	cmode = -1;
	break;
      }
      if (*cmode_ptr == j || *cmode_ptr < 0)
	cmode = j;
    }

	/***************************************
        If we have a signal, embed units or data.
        ***************************************/
  if (cmode >= 0 && cmode < nsig) {
    struct descriptor_xd tmp;
    struct descriptor_signal *sig_ptr = (struct descriptor_signal *)sig[cmode].pointer;
    struct descriptor *keep_ptr = sig_ptr->data;
    struct descriptor *raw_ptr = sig_ptr->raw;
    DESCRIPTOR_WITH_UNITS(wu, 0, 0);
    tmp = emptyxd;
    sig_ptr->raw = 0;
		/************************************
                If we have units, embed data in them.
                ************************************/
    if (pdu) {
      wu.data = out_ptr->pointer;
      wu.units = pdu;
      sig_ptr->data = (struct descriptor *)&wu;
    } else
      sig_ptr->data = out_ptr->pointer;
    status = MdsCopyDxXd((struct descriptor *)sig_ptr, &tmp);
    sig_ptr->data = keep_ptr;
    sig_ptr->raw = raw_ptr;
    MdsFree1Dx(out_ptr, NULL);
    *out_ptr = tmp;
  } else if (pdu) {
    struct descriptor_xd tmp;
    DESCRIPTOR_WITH_UNITS(wu, 0, 0);
    tmp = emptyxd;
    wu.data = out_ptr->pointer;
    wu.units = pdu;
    MdsCopyDxXd((struct descriptor *)&wu, &tmp);
    MdsFree1Dx(out_ptr, NULL);
    *out_ptr = tmp;
  }
  *cmode_ptr = cmode;
  return status;
}
