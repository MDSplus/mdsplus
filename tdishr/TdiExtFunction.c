/*	Tdi1ExtFunction.C
	Call a routine in an image.
	An EXTERNAL FUNCTION has pointers to
	1	descriptor of image logical name (may be null for TDISHR entries, default SYS$SHARE:.EXE)
	2	descriptor of routine name (for TDISHR image these must be TDI$xxxx.)
	3...	descriptors of (ndesc-2) arguments
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

	Ken Klare, LANL	P-4	(c)1989,1990,1991
	NEED we chase logical names if not in first image?
*/
#include <stdio.h>
#include "tdirefstandard.h"
#include <libroutines.h>
#include <strroutines.h>
#include <string.h>
#include <tdimessages.h>
#include <stdlib.h>
#include <mdsshr.h>
#include <mds_stdarg.h>
#include <STATICdef.h>

STATIC_CONSTANT char *cvsrev =
    "@(#)$RCSfile$ $Revision$ $Date$";

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

STATIC_CONSTANT DESCRIPTOR(dnul, "\0");
STATIC_CONSTANT DESCRIPTOR(dfun, ".fun\0");
STATIC_CONSTANT DESCRIPTOR(def_name, "dna=");
#if defined(__VMS)
STATIC_CONSTANT DESCRIPTOR(def_image, "MDS$FUNCTIONS");
STATIC_CONSTANT DESCRIPTOR(def_path, "MDS$PATH:");
#elif defined(WIN32)
STATIC_CONSTANT DESCRIPTOR(def_image, "MdsFunctions");
STATIC_CONSTANT DESCRIPTOR(def_path, "MDS_PATH:");
#else
STATIC_CONSTANT DESCRIPTOR(def_image, "MdsFunctions");
STATIC_CONSTANT DESCRIPTOR(def_path, "MDS_PATH:");
#endif
STATIC_CONSTANT struct descriptor_d EMPTY_D = { 0, DTYPE_T, CLASS_D, 0 };

int TdiFindImageSymbol(struct descriptor_d *image,
		       struct descriptor_d *entry, int (**symbol) ())
{
    LibEstablish(LibSigToRet);
    return LibFindImageSymbol(image, entry, symbol);
}

int Tdi1ExtFunction(int opcode, int narg, struct descriptor *list[],
		    struct descriptor_xd *out_ptr)
{
    int status = 1;
    FILE *unit;
    int j, ntmp = 0, (*routine) ();
    struct descriptor_function *pfun, *pfun2;
    struct descriptor_d image = EMPTY_D, entry = EMPTY_D;
    struct descriptor_xd tmp[253];
    struct descriptor *new[256];
    unsigned short code;
    int geterror = 0;

    LibEstablish(TdiFaultHandler);
    if (list[0])
	status = TdiData(list[0], &image MDS_END_ARG);
    if (status & 1)
	status = TdiData(list[1], &entry MDS_END_ARG);
    if (status & 1)
	status = StrUpcase(&entry, &entry);
    if (!(status & 1))
	goto done;

	/**************************
	Quickly do known functions.
	**************************/
    if (image.length == 0) {
	status = TdiDoFun(&entry, narg - 2, &list[2], out_ptr);
	if (status != TdiUNKNOWN_VAR)
	    goto done;
	/*
	   status = TdiFindImageSymbol((struct descriptor_d *)&def_image, &entry, &routine);
	 */
    } else {
	status = TdiFindImageSymbol(&image, &entry, &routine);
	if (!(status & 1))
	    geterror = 1;
    }

	/**********************************************
	Requires: image found and routine symbol found.
	**********************************************/
    if (status & 1) {
	new[0] = (struct descriptor *)(long)(narg - 1);
	new[narg - 1] = (struct descriptor *)out_ptr;
	for (j = 2; j < narg && status & 1; ++j) {
	    pfun = (struct descriptor_function *)(new[j - 1] = list[j]);
	    if (pfun) {
		if (pfun->dtype == DTYPE_FUNCTION) {
		    /****************************************
                     Special forms used for VMS and LIB calls.
                     ****************************************/
		    code = *(unsigned short *)pfun->pointer;
		    if (code == OpcDescr) {
			tmp[ntmp] = EMPTY_XD;
			status =
			    TdiData(pfun->arguments[0], &tmp[ntmp] MDS_END_ARG);
			new[j - 1] = (struct descriptor *)tmp[ntmp++].pointer;
		    } else if (code == OpcRef) {
			tmp[ntmp] = EMPTY_XD;
			status =
			    TdiData(pfun->arguments[0], &tmp[ntmp] MDS_END_ARG);
			if (status & 1)
			    new[j - 1] =
				(struct descriptor *)tmp[ntmp].pointer->pointer;
			++ntmp;
		    } else if (code == OpcVal)
			status = TdiGetLong(pfun->arguments[0], &new[j - 1]);
		    else if (code == OpcPrivate || code == OpcPublic)
			goto ident;
		}
	    /************************************
             Handle multiple outputs to variables.
             So far only DSQL needs this.
             ************************************/
		else {
		    if (pfun->dtype == DTYPE_IDENT) {
 ident:
			{
			    unsigned char test;
			    struct descriptor dtest =
				{ sizeof(test), DTYPE_BU, CLASS_S, 0 };
			    dtest.pointer = (char *)&test;
			    status = TdiAllocated(pfun, &dtest MDS_END_ARG);
			    if (status && !test)
				status = TdiPutIdent(pfun, 0);
			}
		    }
		}
	    }
	}
	/*************************
         Same form as system calls.
         Watch, may not be XD.
         *************************/
	if (status & 1) {
	    status = (long)LibCallg(&new[0], routine);
	}
	for (; --ntmp >= 0;)
	    MdsFree1Dx(&tmp[ntmp], NULL);
    }
	/***************
     Gather, compile.
	***************/
    else {
	struct descriptor file = { 0, DTYPE_T, CLASS_D, 0 };
	status =
	    StrConcat(&file,
		      list[0] ? (struct descriptor *)&image : (struct descriptor
							       *)&def_path,
		      &entry, &dfun MDS_END_ARG);
	if (status & 1) {
	    void *ctx = 0;
	    struct descriptor dcs = { 0, DTYPE_T, CLASS_S, 0 };
	    LibFindFileRecurseCaseBlind(&file, &file, &ctx);
	    LibFindFileEnd(&ctx);
	    StrAppend(&file, &dnul);
	    unit = fopen(file.pointer, "rb");
	    if (unit) {
		long flen;
		fseek(unit, 0, SEEK_END);
		flen = ftell(unit);
		flen = (flen > 0xffff) ? 0xffff : flen;
		fseek(unit, 0, SEEK_SET);
		dcs.pointer = (char *)malloc(flen);
		dcs.length = (unsigned short)flen;
		fread(dcs.pointer, (size_t) flen, 1, unit);
		fclose(unit);
		status = TdiCompile(&dcs, out_ptr MDS_END_ARG);
		free(dcs.pointer);
	    } else
		status = TdiUNKNOWN_VAR;
	}
	StrFree1Dx(&file);
	/******************************************************
         If it is a FUN then define it and do it with arguments.
         Otherwise, just do it if does not have arguments.
         ******************************************************/
	if (status & 1 && out_ptr) {
	    pfun = (struct descriptor_function *)out_ptr->pointer;
	    if (pfun->dtype == DTYPE_FUNCTION) {
		code = *(unsigned short *)pfun->pointer;
		if (code == OpcFun) {
		    pfun2 = (struct descriptor_function *)pfun->arguments[0];
		    if (pfun2->dtype == DTYPE_FUNCTION) {
			code = *(unsigned short *)pfun2->pointer;
			if (code == OpcPrivate || code == OpcPublic)
			    pfun2 =
				(struct descriptor_function *)pfun2->
				arguments[0];
		    }
		    StrUpcase(pfun2, pfun2);
		    if (StrCompare(&entry, pfun2) == 0) {
			tmp[0] = EMPTY_XD;
			status = MdsCopyDxXd(pfun->arguments[0], &tmp[0]);
			if (status & 1)
			    status = TdiEvaluate(out_ptr, out_ptr MDS_END_ARG);
			if (status & 1)
			    status =
				TdiDoFun(tmp[0].pointer, narg - 2, &list[2],
					 out_ptr);
			MdsFree1Dx(&tmp[0], NULL);
			goto done;
		    }
		}
	    }
	    if (narg > 2)
		status = TdiEXTRA_ARG;
	    else
		status = TdiEvaluate(out_ptr, out_ptr MDS_END_ARG);
	}
    }
 done:
    StrFree1Dx(&entry);
    StrFree1Dx(&image);
    if (geterror)
	printf("%s\n", LibFindImageSymbolErrString());
    return status;
}
