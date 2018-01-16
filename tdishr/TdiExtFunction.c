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
/*      Tdi1ExtFunction.C
        Call a routine in an image.
        An EXTERNAL FUNCTION has pointers to
        1       descriptor of image logical name (may be null for TDISHR entries, default SYS$SHARE:.EXE)
        2       descriptor of routine name (for TDISHR image these must be TDI$xxxx.)
        3...    descriptors of (ndesc-2) arguments
        You must check number of arguments.
        Limit is 253 arguments.
                result = image->entry(in1, ...)
        TDI defined functions may be invoked:
                name ( [PRIVATE vbl|PUBLIC vbl|expr],...)
        These execute functions previously defined by
                FUN [PUBLIC|PRIVATE] name ( [OPTIONAL] [IN|INOUT|OUT] vbl,...) stmt
        If unknown, then try to find file, compile it, and
                if the name matches, execute it.

        (1) image==0 && known function ? do DO_FUN(entry).
        (2) FIND_IMAGE_SYMBOL(image or "MDS$FUNCTIONS", entry) found ? do symbol.
        (3) FOPEN image->routine as routine with related specification image.FUN or MDS$PATH:.FUN,
                read file, compile,
                if compiled is a function and has same name as routine, DO_FUN(entry).
        (4) three strikes and you are out.

        Ken Klare, LANL P-4     (c)1989,1990,1991
        NEED we chase logical names if not in first image?
*/
#define DEF_FREED
#define DEF_FREEXD
#include <pthread_port.h>
#include <stdio.h>
#include "tdirefstandard.h"
#include <libroutines.h>
#include <strroutines.h>
#include <string.h>
#include <tdishr_messages.h>
#include <stdlib.h>
#include <mdsshr.h>
#include <mds_stdarg.h>
#include <STATICdef.h>



extern unsigned short OpcDescr;
extern unsigned short OpcFun;
extern unsigned short OpcPrivate;
extern unsigned short OpcPublic;
extern unsigned short OpcRef;
extern unsigned short OpcVal;

extern int TdiFaultHandler();
extern int TdiData();
extern int TdiDoFun();
extern int TdiGetLong();
extern int TdiAllocated();
extern int TdiPutIdent();
extern int TdiCompile();
extern int TdiEvaluate();

STATIC_CONSTANT struct descriptor_d EMPTY_D = { 0, DTYPE_T, CLASS_D, 0 };

int TdiFindImageSymbol(struct descriptor_d *image, struct descriptor_d *entry, int (**symbol) ())
{
  return LibFindImageSymbol(image, entry, symbol);
}

extern int TdiFindSymbol();

struct _tmp {
  int n;
  struct descriptor_xd a[253];
};
static void tmp_cleanup(void* tmp_in) {
  struct _tmp* tmp = (struct _tmp*)tmp_in;
  for (; --tmp->n >= 0;)
    MdsFree1Dx(&tmp->a[tmp->n], NULL);
}
int Tdi1ExtFunction(int opcode __attribute__ ((unused)),
		    int narg,
		    struct descriptor *list[],
		    struct descriptor_xd *out_ptr)
{
  int status;
  struct descriptor_d image = EMPTY_D, entry = EMPTY_D;
  FREED_ON_EXIT(&image);
  FREED_ON_EXIT(&entry);
  status = MDSplusSUCCESS;
  if (list[0])
    status = TdiData(list[0], &image MDS_END_ARG);
  if STATUS_OK
    status = TdiData(list[1], &entry MDS_END_ARG);
  if (STATUS_NOT_OK)
    goto done;
	/**************************
        Quickly do known functions.
        **************************/
  int (*routine) ();
  if (image.length == 0) {
    status = StrUpcase((struct descriptor *)&entry, (struct descriptor *)&entry);
    if STATUS_OK
      status = TdiDoFun(&entry, narg - 2, &list[2], out_ptr);
    goto done;
  }

  status = TdiFindImageSymbol(&image, &entry, &routine);
	/**********************************************
	Requires: image found and routine symbol found.
	**********************************************/
  if STATUS_OK {
    int j = 0;
    struct _tmp tmp = {0};
    pthread_cleanup_push(tmp_cleanup,(void*)&tmp);
    status = MDSplusSUCCESS;
    struct descriptor_function *pfun;
    struct descriptor *new[256];
    *(int*)&new[0] = narg - 1;
    new[narg - 1] = (struct descriptor *)out_ptr;
    unsigned short code;
    for (j = 2; j < narg && STATUS_OK; ++j) {
      pfun = (struct descriptor_function *)(new[j - 1] = list[j]);
      if (pfun) {
	if (pfun->dtype == DTYPE_FUNCTION) {
	/****************************************
	Special forms used for VMS and LIB calls.
	****************************************/
	  code = *(unsigned short *)pfun->pointer;
	  if (code == OpcDescr) {
	    tmp.a[tmp.n] = EMPTY_XD;
	    status = TdiData(pfun->arguments[0], &tmp.a[tmp.n] MDS_END_ARG);
	    new[j - 1] = (struct descriptor *)tmp.a[tmp.n++].pointer;
	  } else if (code == OpcRef) {
	    tmp.a[tmp.n] = EMPTY_XD;
	    status = TdiData(pfun->arguments[0], &tmp.a[tmp.n] MDS_END_ARG);
            if STATUS_NOT_OK break; // .pointer == NULL
	    new[j - 1] = (struct descriptor *)tmp.a[tmp.n++].pointer->pointer;
	  } else if (code == OpcVal)
	    status = TdiGetLong(pfun->arguments[0], &new[j - 1]);
	  else if (code == OpcPrivate || code == OpcPublic)
	  goto ident;
	} else if (pfun->dtype == DTYPE_IDENT) {
ident: ;
	/************************************
         Handle multiple outputs to variables.
         So far only DSQL needs this.
         ************************************/
	  unsigned char test;
	  struct descriptor dtest = { sizeof(test), DTYPE_BU, CLASS_S, 0 };
	  dtest.pointer = (char *)&test;
	  status = TdiAllocated(pfun, &dtest MDS_END_ARG);
	  if (status && !test)
	    status = TdiPutIdent(pfun, 0);
	}
      }
    }
	/*************************
         Same form as system calls.
         Watch, may not be XD.
         *************************/
    if STATUS_OK {
      struct descriptor_s out = { sizeof(void *) , DTYPE_POINTER, CLASS_S , LibCallg(&new[0], routine) };
      MdsCopyDxXd((struct descriptor*)&out, out_ptr);
    }
    pthread_cleanup_pop(1);
  } else
    printf("%s\n", LibFindImageSymbolErrString());
done: ;
  FREED_NOW(&entry);
  FREED_NOW(&image);
  return status;
}
