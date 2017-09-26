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
/*      Tdi1GetDbi.C
        Get data base information by name.
                GETDBI(STRING, [OFFSET])

        The search name can be shortened to a unique match.
        The full search name should be used in stored trees.

        Ken Klare, LANL P-4     (c)1990,1991
*/
#include <STATICdef.h>
#include "tdirefstandard.h"
#include <dbidef.h>
#include <strroutines.h>
#include <string.h>
#include <stdlib.h>
#include <tdishr_messages.h>
#include <treeshr.h>
#include <mdsshr.h>

STATIC_CONSTANT DESCRIPTOR(coloncolon, "::");
#define EOL {0,DbiEND_OF_LIST,0,0}

STATIC_CONSTANT struct item {
  char *item_name;
  char item_code;
  unsigned char item_dtype;
  unsigned char item_length;
} table[] = {
  {
  "DEFAULT", DbiDEFAULT, DTYPE_T, 0}, {
  "MAX_OPEN", DbiMAX_OPEN, DTYPE_L, 4}, {
  "MODIFIED", DbiMODIFIED, DTYPE_BU, 1}, {
  "NAME", DbiNAME, DTYPE_T, 0}, {
  "NUMBER_OPENED", DbiNUMBER_OPENED, DTYPE_L, 4}, {
  "OPEN_FOR_EDIT", DbiOPEN_FOR_EDIT, DTYPE_BU, 1}, {
  "OPEN_READONLY", DbiOPEN_READONLY, DTYPE_BU, 1}, {
"SHOTID", DbiSHOTID, DTYPE_L, 4},};

#define siztab sizeof(struct item)
#define numtab (sizeof(table)/siztab)

extern int TdiData();
extern int TdiUpcase();
extern int TdiGetData();
extern int TdiGetLong();
extern int TdiEvaluate();

STATIC_ROUTINE int compare(struct descriptor *s1, struct item s2[1])
{
  int cmp, len1 = s1->length, len2 = strlen(s2[0].item_name);
  char c0;

	/****************************************
        If smaller is mismatch, go on.
        (If len1 == len2, got it: cmp=0.)
        If len1 > len2, omit trailing blanks.
        If len1 < len2, check for unique match.
        ****************************************/
  if ((cmp = strncmp(s1->pointer, s2[0].item_name, len1 < len2 ? len1 : len2)) != 0) ;
  else if (len1 > len2)
    cmp = (c0 = s1->pointer[len2]) != ' ' && c0 != '\t';
  else if (len1 < len2) {
    if ((s2 != &table[0]
	 && strncmp(s1->pointer, s2[-1].item_name, len1) == 0)
	|| (s2 != &table[numtab - 1]
	    && strncmp(s1->pointer, s2[1].item_name, len1) == 0))
      cmp = -1;
  }
  return cmp;
}

int Tdi1GetDbi(int opcode __attribute__ ((unused)),
	       int narg,
	       struct descriptor *list[],
	       struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_d string = { 0, DTYPE_T, CLASS_D, 0 };
  struct descriptor_xd tmp = EMPTY_XD;
  struct item *key_ptr = 0;
  int index;
  DBI_ITM lst[] = { {sizeof(index), DbiINDEX, 0, 0}
  , EOL, EOL
  };
  lst[0].pointer = (unsigned char *)&index;
	/**********************
        String of item to find.
        **********************/
  status = TdiData(list[0], &tmp MDS_END_ARG);
  if STATUS_OK
    status = TdiUpcase(&tmp, &string MDS_END_ARG);
  if STATUS_OK {
    key_ptr =
	(struct item *)bsearch(&string, table, numtab, siztab, (int (*)(const void *, const void *))
			       compare);
    if (key_ptr == 0)
      status = TdiBAD_INDEX;
  }
  StrFree1Dx(&string);
  MdsFree1Dx(&tmp, NULL);
	/**********************************
        Somebody might want others in pool.
        **********************************/
  if (STATUS_OK && narg > 1)
    status = TdiGetLong(list[1], &index);
  else
    index = 0;
	/***********************
        Get the item asked for.
        Fixed length or varying.
        ***********************/
  if STATUS_OK {
    lst[1].code = key_ptr->item_code;
    if ((lst[1].buffer_length = key_ptr->item_length) != 0) {
      status = MdsGet1DxS((unsigned short *)&lst[1].buffer_length, &key_ptr->item_dtype, out_ptr);
      if STATUS_OK {
	lst[1].pointer = (unsigned char *)out_ptr->pointer->pointer;
	status = TreeGetDbi(lst);
      }
    } else {
      lst[1].buffer_length = 0;
      lst[1].pointer = NULL;
      status = TreeGetDbi(lst);
      if STATUS_OK {
	struct descriptor ans = { 0, DTYPE_T, CLASS_S, 0 };
	if (lst[1].pointer) {
	  ans.length = strlen((char *)lst[1].pointer);
	  ans.pointer = (char *)lst[1].pointer;
	}
	status = MdsCopyDxXd(&ans, out_ptr);
	if (ans.pointer)
	  TreeFree(ans.pointer);
      }
    }
  }
  return status;
}

/*-------------------------------------------------------------------
        Use a different default, shot, or experiment.
                USING(expression, [DEFAULT], [SHOTID], [EXPT])
        Note that DEFAULT may be NID/PATH and will not be data at same.
*/
STATIC_ROUTINE int fixup_nid(int *pin, /* NID pointer */
			     int arg __attribute__ ((unused)),
			     struct descriptor_d *pout)
{
  char *path = TreeGetPath(*pin);
  if (path != NULL) {
    unsigned short len = (unsigned short)strlen(path);
    StrCopyR((struct descriptor *)pout, &len, path);
    TreeFree(path);
    return MDSplusSUCCESS;
  }
  return MDSplusERROR;
}

STATIC_ROUTINE int fixup_path(struct descriptor *pin,
			      int arg __attribute__ ((unused)),
			      struct descriptor_d *pout)
{
  char *pathin = MdsDescrToCstring(pin);
  char *path = TreeAbsPath(pathin);
  MdsFree(pathin);
  if (path != NULL) {
    unsigned short len = (unsigned short)strlen(path);
    StrCopyR((struct descriptor *)pout, &len, path);
    TreeFree(path);
    return MDSplusSUCCESS;
  }
  return MDSplusERROR;
}

int Tdi1Using(int opcode __attribute__ ((unused)),
	      int narg,
	      struct descriptor *list[],
	      struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  void *ctx;
  int reset_ctx = 0;
  int nid, shot;
  unsigned short stat1;
  struct descriptor_d def = { 0, DTYPE_T, CLASS_D, 0 }, expt = def;
  unsigned char omits[] = { DTYPE_PATH, 0 };

	/**********************
        Evaluate with current.
        Use current if omitted.
        Must get expt if shot.
        **********************/
  if (narg > 1 && STATUS_OK) {
    if (list[1]) {
      struct descriptor_xd xd = EMPTY_XD;
      status = TdiGetData(omits, list[1], &xd);
      if (STATUS_OK && xd.pointer)
	switch (xd.pointer->dtype) {
	case DTYPE_T:
	case DTYPE_PATH:
	  status = StrCopyDx((struct descriptor *)&def, xd.pointer);
	  break;
	default:
	  status = TdiINVDTYDSC;
	  break;
	}
      MdsFree1Dx(&xd, NULL);
    }
    if (!list[1] || def.length == 0) {
      DBI_ITM def_itm[] = { {0, DbiDEFAULT, 0, 0}
      , EOL
      };
      status = TreeGetDbi(def_itm);
      if (def_itm[0].pointer == NULL) {
	STATIC_CONSTANT DESCRIPTOR(top, "\\TOP");
	StrCopyDx((struct descriptor *)&def, (struct descriptor *)&top);
	status = MDSplusSUCCESS;
      } else {
	unsigned short len = (unsigned short)strlen((char *)def_itm[0].pointer);
	StrCopyR((struct descriptor *)&def, &len, def_itm[0].pointer);
	TreeFree(def_itm[0].pointer);
      }
      if STATUS_OK {
	stat1 = StrPosition((struct descriptor *)&def, (struct descriptor *)&coloncolon, 0) + 1;
	status = StrRight((struct descriptor *)&def, (struct descriptor *)&def, &stat1);
      }
      if STATUS_OK
	*def.pointer = '\\';
    }
  }
  if ((narg > 2) && ((list[2] != 0) || ((narg > 3) && (list[3] != 0)))
      && STATUS_OK) {
    if (list[2])
      status = TdiGetLong(list[2], &shot);
    else {
      DBI_ITM shot_itm[] = { {sizeof(shot), DbiSHOTID, 0, 0}
      , EOL
      };
      shot_itm[0].pointer = (unsigned char *)&shot;
      status = TreeGetDbi(shot_itm);
    }

    if STATUS_OK {
      if (narg > 3 && list[3])
	status = TdiData(list[3], &expt MDS_END_ARG);
      else {
	DBI_ITM expt_itm[] = { {0, DbiNAME, 0, 0}
	, EOL
	};
	status = TreeGetDbi(expt_itm);
	if (expt_itm[0].pointer) {
	  unsigned short len = (unsigned short)strlen((char *)expt_itm[0].pointer);
	  StrCopyR((struct descriptor *)&expt, &len, expt_itm[0].pointer);
	  TreeFree(expt_itm[0].pointer);
	}
      }
    }
		/*********************
                Set new tree and path.
                Allow some rel paths.
                *********************/
    if STATUS_OK {
      char *tree = MdsDescrToCstring((struct descriptor *)&expt);
      ctx = TreeSwitchDbid(0);
      reset_ctx = 1;
      status = TreeOpen(tree, shot, 1);
      MdsFree(tree);
    }
  }
  if (narg > 1) {
    char *path = MdsDescrToCstring((struct descriptor *)&def);
    if STATUS_OK
      status = TreeSetDefault(path, &nid);
    MdsFree(path);
    if (narg > 2)
      StrFree1Dx(&expt);
    StrFree1Dx(&def);
  }
	/***********************
        Evaluate with temporary.
        ***********************/
  if STATUS_OK {
    struct descriptor_xd tmp = EMPTY_XD;
    status = TdiEvaluate(list[0], &tmp MDS_END_ARG);
    if STATUS_OK
      status = MdsCopyDxXdZ((struct descriptor *)&tmp, out_ptr, NULL,
			    fixup_nid, NULL, fixup_path, NULL);
    MdsFree1Dx(&tmp, NULL);
  }
  if (reset_ctx) {
    while (TreeClose(0, 0) & 1) ;
    TreeFreeDbid(TreeSwitchDbid(ctx));

  }
  return status;
}
