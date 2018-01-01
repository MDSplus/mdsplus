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
#include <mdsplus/mdsconfig.h>
#include <string.h>
#include <STATICdef.h>
#include <mdsshr.h>
#include "mdsshrp.h"
#include "mdsshrthreadsafe.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <libroutines.h>
#include <strroutines.h>

/*========================================================
 * "Define"s and structure definitions ...
 *=======================================================*/
#define ALREADY_DISPLAYED (int)0x80000000

/**********************************************************************
* MDSMSG.C --
*
* Routine for printing status information ...
*
* History:
*  06-Apr-2001  TRG  Start/end each MdsMsg line with '\r', esp for IDL.
*  05-Jan-1998  TRG  Create.
*
************************************************************************/


/*****************************************************************
 * MdsGetMsg:
 *****************************************************************/
EXPORT char *MdsGetMsg(		/* Return: addr of "status" string      */
		 int sts	/* <r> sts value                        */
    )
{
  STATIC_CONSTANT DESCRIPTOR(msg_files, "MDSMSG_PATH:*Msg.*");
  STATIC_CONSTANT DESCRIPTOR(getmsg_nam, "getmsg");
  struct descriptor_d filnam = { 0, DTYPE_T, CLASS_D, 0 };
  const char *facnam, *msgnam, *msgtext;
  int status = 0;
  void *ctx = 0;
  STATIC_CONSTANT const char *severity[] = { "W", "S", "E", "I", "F", "?", "?", "?" };
  int (*getmsg) (int, const char **, const char **, const char **);

  status = MdsGetStdMsg(sts, &facnam, &msgnam, &msgtext);
  if STATUS_OK {
    sprintf((MdsShrGetThreadStatic())->MdsGetMsg_text, "%%%s-%s-%s, %s", facnam,
	    severity[sts & 0x7], msgnam, msgtext);
    return (MdsShrGetThreadStatic())->MdsGetMsg_text;
  }
  while (STATUS_NOT_OK && (LibFindFile((struct descriptor *)&msg_files, (struct descriptor *)&filnam, &ctx) & 1)) {
    status = LibFindImageSymbol(&filnam, &getmsg_nam, &getmsg);
    if STATUS_OK {
      status = (*getmsg) (sts, &facnam, &msgnam, &msgtext);
      if STATUS_OK
	sprintf((MdsShrGetThreadStatic())->MdsGetMsg_text, "%%%s-%s-%s, %s", facnam,
		severity[sts & 0x7], msgnam, msgtext);
    }
  }
  LibFindFileEnd(&ctx);
  if STATUS_NOT_OK
    sprintf((MdsShrGetThreadStatic())->MdsGetMsg_text, "%%NONAME-%s-NOMSG, Message number 0x%08X",
	    severity[sts & 0x7], sts);
  return (MdsShrGetThreadStatic())->MdsGetMsg_text;
}

EXPORT void MdsGetMsgDsc(int status, struct descriptor *out)
{
  MdsGetMsg(status);
  (MdsShrGetThreadStatic())->MdsGetMsgDsc_tmp.length =
      (unsigned short)strlen((MdsShrGetThreadStatic())->MdsGetMsgDsc_tmp.pointer);
  StrCopyDx(out, &(MdsShrGetThreadStatic())->MdsGetMsgDsc_tmp);
  return;
}

	/*****************************************************************
	 * MdsMsg:
	 *****************************************************************/
EXPORT int MdsMsg(			/* Return: sts provided by user         */
	    int sts		/* <r> status code                      */
	    , char const fmt[]	/* <r> format statement                     */
	    , ...		/* <r:opt> arguments to fmt[]               */
    )
{
  int write2stdout;
  va_list ap;			/* arg ptr                              */

  write2stdout = isatty(fileno(stdout)) ^ isatty(fileno(stderr));
  if ((sts & ALREADY_DISPLAYED) && (sts != -1))
    return (sts);

  if (fmt) {
    va_start(ap, fmt);		/* initialize "ap"                      */
    vsprintf((MdsShrGetThreadStatic())->MdsMsg_text, fmt, ap);
    if (sts) {
      MDSfprintf(stderr, "%s\n\r    sts=%s\n\n\r", (MdsShrGetThreadStatic())->MdsMsg_text,
		 MdsGetMsg(sts));
      if (write2stdout)
	MDSfprintf(stdout, "%s\n\r    sts=%s\n\n\r",
		   (MdsShrGetThreadStatic())->MdsMsg_text, MdsGetMsg(sts));
    } else {
      MDSfprintf(stderr, "%s\n\r", (MdsShrGetThreadStatic())->MdsMsg_text);
      if (write2stdout)
	MDSfprintf(stdout, "%s\n\r", (MdsShrGetThreadStatic())->MdsMsg_text);
    }
  } else {
    MDSfprintf(stderr, "%s:  sts=%s\n\r", (MdsShrGetThreadStatic())->MdsMsg_text, MdsGetMsg(sts));
    if (write2stdout)
      MDSfprintf(stdout, "%s:  sts=%s\n\r", (MdsShrGetThreadStatic())->MdsMsg_text, MdsGetMsg(sts));
  }

  return (sts | ALREADY_DISPLAYED);
}

#ifdef MAIN
void main()
{
  MdsMsg(MDSDCL_STS_SUCCESS, 0);
  MDSprintf("\n");
  MdsMsg(CLI_STS_PRESENT, 0);
  MDSprintf("\n");
  MdsMsg(CCL_STS_SUCCESS, 0);
  MDSprintf("\n");
  MdsMsg(TCL_STS_SUCCESS, 0);
  MDSprintf("\n");
  MdsMsg(TreeNORMAL, 0);
  MDSprintf("\n");
  MdsMsg(LibNOTFOU, 0);
  MDSprintf("\n");
  MdsMsg(StrMATCH, 0);
  MDSprintf("\n");
  MdsMsg(SsINTOVF, 0);
  MDSprintf("\n");
}
#endif
