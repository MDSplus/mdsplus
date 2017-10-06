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
/*      Tdi1GetNci.C
        Get node characteristic information by name.
        WARNING this routine is an exception to rule that NIDs and PATHs are evaluated.
        You may use GETNCI(GETNCI(.A,"CHILDREN"),"MEMBER_NIDS) as GETNCI can handle NID arrays.
        WARNING an array of NIDs does not act like a NID/PATH vector because [.a,.b] has .a and .b evaluated.
        Thus use a vector of text [".a",".b"] or [GETNCI(.a,"NID"), GETNCI(.b,"NID")].

        The search name can be shortened to a unique match.
        The full search name should be used in stored trees.
                GETNCI(nid-path,"item_name",["usage_name"])
        Note nid-path may be a nid (.XRAY),
        a path (.XRAY as forward reference in compile), it may be wildcarded (.*RAY),
        text or text expression (".XRAY"//":DESCRIPTION"), or
        long or long_unsigned expression of node number (1) or arrays of these.

        Bit tests: GETNCI(nid-path,"IS_CHILD") tests state of PARENT_RELATIONSHIP and gives $TRUE/$FALSE.
        Special: GETNCI(nid-path,"NID_NUMBER") gives nid as longword.
        Special: GETNCI(nid-path,"RECORD") gives value at NID, EVALUATE(nid-path) is evaluated.
        Special: GETNCI(,"item_name") gives information about default/current node.

        Ken Klare, LANL P-4     (c)1989,1990,1991,1992
        NEED parameter for sizeof(NODE_NAME).
        NEED to be careful about any new names. They could take away uniqueness from existing code.
*/
#include <STATICdef.h>
#undef MAX
#define EOL {0,NciEND_OF_LIST,0,0}
#define NID_NUMBER -1
#define RECORDIDX -2
#include <string.h>
#include <stdlib.h>
#include <ncidef.h>
#define NciM_ON (NciM_STATE | NciM_PARENT_STATE)
#include <usagedef.h>
#include <strroutines.h>
#include <tdishr_messages.h>
#include <treeshr.h>
#include <mdsshr.h>
#include <stdio.h>
#include "tdirefstandard.h"
#include "tdinelements.h"



extern unsigned short OpcVector;

extern int TdiGetData();
extern int TdiData();
extern int TdiUpcase();
extern int Tdi1Vector();
int TdiGetRecord(int nid, struct descriptor_xd *out);

STATIC_CONSTANT struct item {
  char *item_name;
  unsigned int item_mask;
  unsigned int item_test;
  char item_code;
  unsigned char item_dtype;
  unsigned short item_length;
} table[] = {
  {
  "BROTHER", 0, 0, NciBROTHER, DTYPE_NID, 4}, {
  "CACHED", NciM_CACHED, NciM_CACHED, NciGET_FLAGS, DTYPE_BU, 1}, {
  "CHILD", 0, 0, NciCHILD, DTYPE_NID, 4}, {
  "CHILDREN_NIDS", 0, NciNUMBER_OF_CHILDREN, NciCHILDREN_NIDS, DTYPE_NID, 0}, {
  "CLASS", 0, 0, NciCLASS, DTYPE_BU, 1}, {
  "CLASS_STR", 0, 0, NciCLASS_STR, DTYPE_T, 0}, {
  "COMPRESSIBLE", NciM_COMPRESSIBLE, NciM_COMPRESSIBLE, NciGET_FLAGS, DTYPE_BU, 1}, {
  "COMPRESS_ON_PUT", NciM_COMPRESS_ON_PUT, NciM_COMPRESS_ON_PUT, NciGET_FLAGS, DTYPE_BU, 1}, {
  "COMPRESS_SEGMENTS", NciM_COMPRESS_SEGMENTS, NciM_COMPRESS_SEGMENTS, NciGET_FLAGS, DTYPE_BU, 1}, {
  "CONGLOMERATE_ELT", 0, 0, NciCONGLOMERATE_ELT, DTYPE_WU, 2}, {
  "CONGLOMERATE_NIDS", 0, NciNUMBER_OF_ELTS, NciCONGLOMERATE_NIDS, DTYPE_NID, 0}, {
  "DATA_IN_NCI", 0, 0, NciDATA_IN_NCI, DTYPE_BU, 1}, {
  "DEPTH", 0, 0, NciDEPTH, DTYPE_L, 4}, {
  "DISABLED", NciM_STATE, NciM_STATE, NciGET_FLAGS, DTYPE_BU, 1}, {
  "DO_NOT_COMPRESS", NciM_DO_NOT_COMPRESS, NciM_DO_NOT_COMPRESS, NciGET_FLAGS, DTYPE_BU, 1}, {
  "DTYPE", 0, 0, NciDTYPE, DTYPE_BU, 1}, {
  "DTYPE_STR", 0, 0, NciDTYPE_STR, DTYPE_T, 0}, {
  "ERROR_ON_PUT", 0, 0, NciERROR_ON_PUT, DTYPE_BU, 1}, {
  "ESSENTIAL", NciM_ESSENTIAL, NciM_ESSENTIAL, NciGET_FLAGS, DTYPE_BU, 1}, {
  "FULLPATH", 0, 0, NciFULLPATH, DTYPE_T, 0}, {
  "GET_FLAGS", 0, 0, NciGET_FLAGS, DTYPE_LU, 4}, {
  "INCLUDE_IN_PULSE", NciM_INCLUDE_IN_PULSE, NciM_INCLUDE_IN_PULSE, NciGET_FLAGS, DTYPE_BU, 1}, {
  "IO_STATUS", 0, 0, NciIO_STATUS, DTYPE_LU, 4}, {
  "IO_STV", 0, 0, NciIO_STV, DTYPE_LU, 4}, {
  "IS_CHILD", 0xffff, NciK_IS_CHILD, NciPARENT_RELATIONSHIP, DTYPE_BU, 1}, {
  "IS_MEMBER", 0xffff, NciK_IS_MEMBER, NciPARENT_RELATIONSHIP, DTYPE_BU, 1}, {
  "LENGTH", 0, 0, NciLENGTH, DTYPE_LU, 4}, {
  "MEMBER", 0, 0, NciMEMBER, DTYPE_NID, 4}, {
  "MEMBER_NIDS", 0, NciNUMBER_OF_MEMBERS, NciMEMBER_NIDS, DTYPE_NID, 0}, {
  "MINPATH", 0, 0, NciMINPATH, DTYPE_T, 0}, {
  "NID_NUMBER", 0, 0, NID_NUMBER, DTYPE_L, 4}, {
  "NID_REFERENCE", NciM_NID_REFERENCE, NciM_NID_REFERENCE, NciGET_FLAGS, DTYPE_BU, 1}, {
  "NODE_NAME", 0, 0, NciNODE_NAME, DTYPE_T, 12}, {
  "NO_WRITE_MODEL", NciM_NO_WRITE_MODEL, NciM_NO_WRITE_MODEL, NciGET_FLAGS, DTYPE_BU, 1}, {
  "NO_WRITE_SHOT", NciM_NO_WRITE_SHOT, NciM_NO_WRITE_SHOT, NciGET_FLAGS, DTYPE_BU, 1}, {
  "NUMBER_OF_CHILDREN", 0, 0, NciNUMBER_OF_CHILDREN, DTYPE_L, 4}, {
  "NUMBER_OF_ELTS", 0, 0, NciNUMBER_OF_ELTS, DTYPE_L, 4}, {
  "NUMBER_OF_MEMBERS", 0, 0, NciNUMBER_OF_MEMBERS, DTYPE_L, 4}, {
  "ON", NciM_ON, 0, /*special */ NciGET_FLAGS, DTYPE_BU, 1},
  {
  "ORIGINAL_PART_NAME", 0, 0, NciORIGINAL_PART_NAME, DTYPE_T, 0}, {
  "OWNER_ID", 0, 0, NciOWNER_ID, DTYPE_LU, 4}, {
  "PARENT", 0, 0, NciPARENT, DTYPE_NID, 4}, {
  "PARENT_DISABLED", NciM_PARENT_STATE, NciM_PARENT_STATE, NciGET_FLAGS, DTYPE_BU, 1}, {
  "PARENT_RELATIONSHIP", 0, 0, NciPARENT_RELATIONSHIP, DTYPE_L, 4}, {
  "PARENT_STATE", NciM_PARENT_STATE, NciM_PARENT_STATE, NciGET_FLAGS, DTYPE_BU, 1}, {
  "PATH", 0, 0, NciPATH, DTYPE_T, 0}, {
  "PATH_REFERENCE", NciM_PATH_REFERENCE, NciM_PATH_REFERENCE, NciGET_FLAGS, DTYPE_BU, 1}, {
  "RECORD", 0, 0, RECORDIDX, DTYPE_L, 0}, {
  "RFA", 0, 0, NciRFA, DTYPE_Q, 8}, {
  "RLENGTH", 0, 0, NciRLENGTH, DTYPE_LU, 4}, {
  "SEGMENTED", NciM_SEGMENTED, NciM_SEGMENTED, NciGET_FLAGS, DTYPE_BU, 1}, {
  "SETUP_INFORMATION", NciM_SETUP_INFORMATION, NciM_SETUP_INFORMATION, NciGET_FLAGS, DTYPE_BU, 1}, {
  "STATE", NciM_STATE, NciM_STATE, NciGET_FLAGS, DTYPE_BU, 1}, {
  "STATUS", 0, 0, NciSTATUS, DTYPE_LU, 4}, {
  "TIME_INSERTED", 0, 0, NciTIME_INSERTED, DTYPE_QU, 8}, {
  "USAGE", 0, 0, NciUSAGE, DTYPE_BU, 1}, {
  "USAGE_ACTION", 0xff, TreeUSAGE_ACTION, NciUSAGE, DTYPE_BU, 1}, {
  "USAGE_ANY", 0xff, TreeUSAGE_ANY, NciUSAGE, DTYPE_BU, 1}, {
  "USAGE_AXIS", 0xff, TreeUSAGE_AXIS, NciUSAGE, DTYPE_BU, 1}, {
  "USAGE_COMPOUND_DATA", 0xff, TreeUSAGE_COMPOUND_DATA, NciUSAGE, DTYPE_BU, 1}, {
  "USAGE_DEVICE", 0xff, TreeUSAGE_DEVICE, NciUSAGE, DTYPE_BU, 1}, {
  "USAGE_DISPATCH", 0xff, TreeUSAGE_DISPATCH, NciUSAGE, DTYPE_BU, 1}, {
  "USAGE_NONE", 0xff, TreeUSAGE_NONE, NciUSAGE, DTYPE_BU, 1}, {
  "USAGE_NUMERIC", 0xff, TreeUSAGE_NUMERIC, NciUSAGE, DTYPE_BU, 1}, {
  "USAGE_SIGNAL", 0xff, TreeUSAGE_SIGNAL, NciUSAGE, DTYPE_BU, 1}, {
  "USAGE_STR", 0, 0, NciUSAGE_STR, DTYPE_T, 0}, {
  "USAGE_STRUCTURE", 0xff, TreeUSAGE_STRUCTURE, NciUSAGE, DTYPE_BU, 1}, {
  "USAGE_SUBTREE", 0xff, TreeUSAGE_SUBTREE, NciUSAGE, DTYPE_BU, 1}, {
  "USAGE_TASK", 0xff, TreeUSAGE_TASK, NciUSAGE, DTYPE_BU, 1}, {
  "USAGE_TEXT", 0xff, TreeUSAGE_TEXT, NciUSAGE, DTYPE_BU, 1}, {
  "USAGE_WINDOW", 0xff, TreeUSAGE_WINDOW, NciUSAGE, DTYPE_BU, 1}, {
  "VERSIONS", NciM_VERSIONS, NciM_VERSIONS, NciGET_FLAGS, DTYPE_BU, 1}, {
"WRITE_ONCE", NciM_WRITE_ONCE, NciM_WRITE_ONCE, NciGET_FLAGS, DTYPE_BU, 1},};

#define siztab sizeof(struct item)
#define numtab (sizeof(table)/siztab)

STATIC_CONSTANT struct usage_item {
  char *usage_name;
  char usage_code;
} usage_table[] = {
  {
  "ACTION", TreeUSAGE_ACTION,}, {
  "ANY", TreeUSAGE_ANY,}, {
  "AXIS", TreeUSAGE_AXIS,}, {
  "COMPOUND_DATA", TreeUSAGE_COMPOUND_DATA,}, {
  "DEVICE", TreeUSAGE_DEVICE,}, {
  "DISPATCH", TreeUSAGE_DISPATCH,}, {
  "NONE", TreeUSAGE_NONE,}, {
  "NUMERIC", TreeUSAGE_NUMERIC,}, {
  "SIGNAL", TreeUSAGE_SIGNAL,}, {
  "STRUCTURE", TreeUSAGE_STRUCTURE,}, {
  "SUBTREE", TreeUSAGE_SUBTREE,}, {
  "TASK", TreeUSAGE_TASK,}, {
  "TEXT", TreeUSAGE_TEXT,}, {
"WINDOW", TreeUSAGE_WINDOW,},};

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

int Tdi1GetNci(int opcode __attribute__ ((unused)),
	       int narg,
	       struct descriptor *list[],
	       struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  STATIC_CONSTANT struct descriptor_d EMPTY_D = { 0, DTYPE_T, CLASS_D, 0 };
  STATIC_CONSTANT DESCRIPTOR_A(arr0, 1, DTYPE_B, 0, 960);
  struct descriptor_a *holda_ptr = 0;
  struct descriptor_d string = EMPTY_D;
  struct descriptor_xd nids = EMPTY_XD, tmp = EMPTY_XD, holdxd = EMPTY_XD;
  struct item *key_ptr = 0;
  int class = 0, j, outcount = 0;
  unsigned short step;
  int dtype = 0, flag, nelem = 0, retlen, len = 0;
  int nid;
  char *hold_ptr = 0, *dat_ptr = 0;
  unsigned char omits[] = { DTYPE_NID, DTYPE_PATH, 0 };
  int wild, usage_mask;
  void *pctx = NULL;
  unsigned short maxlen = 0;

  if (list[0]) {
    status = TdiGetData(omits, list[0], &nids);
    if STATUS_OK {
      len = nids.pointer->length;
      class = nids.pointer->class;
      dtype = nids.pointer->dtype;
      dat_ptr = nids.pointer->pointer;
      N_ELEMENTS(nids.pointer, nelem);
		/*** scalar non-text/path gives scalar if possible ***/
		/*** a non-array text/path element without wilds gives scalar if possible ***/
      if (STATUS_OK && class != CLASS_A)
	switch (dtype) {
	case DTYPE_T:
	case DTYPE_PATH:
	  {
	    struct descriptor path_d = { 0, DTYPE_PATH, CLASS_S, 0 };
	    char *path;
	    path_d.length = (unsigned char)len;
	    path_d.pointer = dat_ptr;
	    path = MdsDescrToCstring(&path_d);
	    if (!(TreeFindNode(path, &nid) & 1))
	      class = CLASS_A;
	    MdsFree(path);
	  }
	}
    }
  } else {
    status = TreeGetDefaultNid(&nid);
    len = sizeof(nid);
    class = CLASS_S;
    dtype = DTYPE_NID;
    dat_ptr = (char *)&nid;
    nelem = 1;
  }
	/**********************
        String of item to find.
        **********************/
  if STATUS_OK
    status = TdiData(list[1], &tmp MDS_END_ARG);
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
	/********************
        String of node usage.
        ********************/
  if (narg > 2 && list[2]) {
    usage_mask = 0;
    if STATUS_OK
      status = TdiData(list[2], &tmp MDS_END_ARG);
    if STATUS_OK
      status = TdiUpcase(&tmp, &tmp MDS_END_ARG);
    if STATUS_OK {
      struct descriptor allow = *tmp.pointer;
      int length;
      struct descriptor usage = { 0, DTYPE_T, CLASS_S, 0 };
      int nallow;
      N_ELEMENTS(tmp.pointer, nallow);
      length = allow.length;
      allow.class = CLASS_S;
      for (; --nallow >= 0; allow.pointer += length) {
	struct usage_item *puse = &usage_table[sizeof(usage_table) / sizeof(usage_table[0])];
	allow.length = length;
	while (((char *)allow.pointer)[allow.length - 1] == ' ')
	  allow.length--;
	for (; --puse >= &usage_table[0];) {
	  usage.length = (unsigned short)strlen(usage.pointer = puse->usage_name);
	  if (StrCompare(&allow, &usage) == 0) {
	    usage_mask |= 1 << puse->usage_code;
	    break;
	  }
	}
	if (puse < &usage_table[0])
	  status = TdiBAD_INDEX;
      }
    }
    if (usage_mask & (1 << TreeUSAGE_ANY))
      usage_mask = 0xffff;
  } else
    usage_mask = 0xffff;
	/********************
        Guess at holder size.
        ********************/
  if STATUS_OK {
    step = key_ptr->item_length == 0 ? sizeof(struct descriptor_xd) : key_ptr->item_length;
    status = MdsGet1DxA((struct descriptor_a *)&arr0, &step, &key_ptr->item_dtype, &holdxd);
  }
  if STATUS_OK {
    holda_ptr = (struct descriptor_a *)holdxd.pointer;
    hold_ptr = holda_ptr->pointer;
    memset(hold_ptr, 255, holda_ptr->arsize);
  }
	/**************************************
        Loop over arrays of NIDs or TEXT names.
        **************************************/
  for (; STATUS_OK && --nelem >= 0; dat_ptr += len) {
		/**************
                Convert to NID.
                **************/
    wild = 0;
 more:switch (dtype) {
    case DTYPE_PATH:
    case DTYPE_T:
      {
	struct descriptor path_d = { 0, DTYPE_PATH, CLASS_S, 0 };
	char *path;
	path_d.length = (unsigned short)len;
	path_d.pointer = dat_ptr;
	path = MdsDescrToCstring(&path_d);
	if STATUS_OK
	  status = TreeFindNodeWild(path, &nid, &pctx, usage_mask);
	MdsFree(path);
      }
      if (status == TreeNMN || status == TreeNNF) {
	TreeFindNodeEnd(&pctx);
	pctx = NULL;
	if (status == TreeNMN)
	  status = MDSplusSUCCESS;
	continue;
      }
      wild = 1;
      break;
    case DTYPE_L:
    case DTYPE_LU:
    case DTYPE_NID:
      nid = *(int *)dat_ptr;
      break;
    default:
      status = TdiINVDTYDSC;
      break;
    }
		/****************************
                Double size when you run out.
                ****************************/
    if ((unsigned int)(outcount * step) >= holda_ptr->arsize) {
      holda_ptr->arsize *= 2;
      if STATUS_OK
	status = MdsGet1DxA(holda_ptr, &step, &key_ptr->item_dtype, &tmp);
      if STATUS_OK {
	memset(tmp.pointer->pointer, 255, holda_ptr->arsize);
	memcpy(tmp.pointer->pointer, holda_ptr->pointer, holda_ptr->arsize / 2);
      }
      MdsFree1Dx(&holdxd, NULL);
      holdxd = tmp;
      holda_ptr = (struct descriptor_a *)holdxd.pointer;
      hold_ptr = holda_ptr->pointer + outcount * step;
      tmp = EMPTY_XD;
    }
	/*****************************
        Store either data or pointers.
        *****************************/
    if (STATUS_NOT_OK)
      break;
    if (key_ptr->item_code == NID_NUMBER)
      *(int *)hold_ptr = nid;
    else if (key_ptr->item_code == RECORDIDX) {
      *(struct descriptor_xd *)hold_ptr = EMPTY_XD;
      status = TdiGetRecord(nid, (struct descriptor_xd *)hold_ptr);
    }
		/********************************
                Logical results for bit or value.
                ********************************/
    else if (key_ptr->item_mask) {
      NCI_ITM masked[2] = { {sizeof(flag), 0, 0, 0}
      , EOL
      };
      flag = 0;
      masked[0].code = key_ptr->item_code;
      masked[0].pointer = (unsigned char *)&flag;
      masked[0].return_length_address = &retlen;
      if STATUS_OK
	status = TreeGetNci(nid, masked);
      if STATUS_OK
/*
                        {
                          char ans=0;
                          switch (retlen)
                          {
                          case 1: flag = (int)(*(char *)&flag); break;
                          case 2: flag = (int)(*(short *)&flag); break;
                          case 4: flag = (int)(*(int *)&flag); break;
                          }
                          *(char *)hold_ptr = (char)((unsigned short)(flag & key_ptr->item_mask) == key_ptr->item_test);
                        }
*/
	*(char *)hold_ptr = (char)((flag & key_ptr->item_mask) == key_ptr->item_test);
    }
		/*************************
                Only nid arrays are here.
                First get size, then data.
                *************************/
    else if (key_ptr->item_test) {
      unsigned char dtype = (unsigned char)DTYPE_NID;
      unsigned short dlen = sizeof(nid);
      array arr = *(array *) & arr0;
      NCI_ITM tested[2] = { {sizeof(int), 0, 0, 0}, EOL };
      tested[0].code = key_ptr->item_test;
      tested[0].pointer = (unsigned char *)&arr.arsize;
      tested[0].return_length_address = &retlen;
      status = TreeGetNci(nid, tested);
      *(struct descriptor_xd *)hold_ptr = EMPTY_XD;
      if STATUS_OK
	status = MdsGet1DxA((struct descriptor_a *)&arr, &dlen,
			    &dtype, (struct descriptor_xd *)hold_ptr);
      if STATUS_OK {
	struct descriptor_a *aptr = (struct descriptor_a *)
	    (((struct descriptor_xd *)hold_ptr)->pointer);
	NCI_ITM listed[2] = { {0, 0, 0, 0}, EOL };
	listed[0].buffer_length = (short)aptr->arsize;
	listed[0].code = key_ptr->item_code;
	listed[0].pointer = (unsigned char *)aptr->pointer;
	listed[0].return_length_address = &retlen;
	if (aptr->arsize)
	  status = TreeGetNci(nid, listed);
	aptr->dtype = DTYPE_LU;
      }
    }
		/***************************
                Do dynamic text arrays here.
                ***************************/
    else if (key_ptr->item_length == 0) {
      NCI_ITM texted[2] = { {0, 0, 0, 0}
      , EOL
      };
      texted[0].code = key_ptr->item_code;
      texted[0].return_length_address = &retlen;
      *(struct descriptor_d *)hold_ptr = EMPTY_D;
      status = TreeGetNci(nid, texted);
      if (STATUS_OK && texted[0].pointer) {
	unsigned short len = (unsigned short)strlen((char *)texted[0].pointer);
	if (maxlen < len)
	  maxlen = len;
	StrCopyR((struct descriptor *)hold_ptr, &len, texted[0].pointer);
	TreeFree(texted[0].pointer);
      }
    }
		/***********************************
                Fixed length items can be done here.
                Skip len=0 NIDs (missing).
                ***********************************/
    else {
      NCI_ITM fixed[2] = { {0, 0, 0, 0}
      , EOL
      };
      fixed[0].buffer_length = key_ptr->item_length;
      fixed[0].code = key_ptr->item_code;
      fixed[0].pointer = (unsigned char *)hold_ptr;
      fixed[0].return_length_address = &retlen;
      status = TreeGetNci(nid, fixed);
      if (retlen == 0)
	goto skip;
    }
    ++outcount;
    hold_ptr += step;
 skip:if (STATUS_OK && wild)
      goto more;
  }
  MdsFree1Dx(&nids, NULL);
  MdsFree1Dx(&tmp, NULL);
  if STATUS_OK {
    holda_ptr->arsize = hold_ptr - holda_ptr->pointer;
    if (key_ptr->item_length) {
      if (outcount == 1 && class != CLASS_A)
	holda_ptr->class = CLASS_S;
      status = MdsCopyDxXd((struct descriptor *)holda_ptr, out_ptr);
    } else if (outcount == 0)
      MdsFree1Dx(out_ptr, NULL);
    else if (outcount == 1 && class != CLASS_A)
      status = MdsCopyDxXd((struct descriptor *)holda_ptr->pointer, out_ptr);
    else if (maxlen > 0) {
      array arr = *(array *) & arr0;
      unsigned char dtype = DTYPE_T;
      arr.arsize = outcount;
      status =
	  MdsGet1DxA((struct descriptor_a *)&arr, &maxlen, &dtype, (struct descriptor_xd *)out_ptr);
      if STATUS_OK {
	char *p;
	struct descriptor_xd *dp;
	int j;
	memset(out_ptr->pointer->pointer, 32, outcount * maxlen);
	for (j = 0, p = out_ptr->pointer->pointer, dp =
	     (struct descriptor_xd *)holda_ptr->pointer; j < outcount; j++, p += maxlen, dp++) {
	  memcpy(p, dp->pointer, dp->length);
	}
      }

    } else {
      unsigned short dlen = sizeof(int *);
      unsigned char dtype = (unsigned char)DTYPE_L;
      status = MdsGet1DxA(holda_ptr, &dlen, &dtype, &tmp);
      if STATUS_OK {
	char **xd_ptr = (char **)tmp.pointer->pointer;
	hold_ptr = holda_ptr->pointer;
	for (j = outcount; --j >= 0; hold_ptr += step)
	  *xd_ptr++ = hold_ptr;
	status = Tdi1Vector(OpcVector, outcount, tmp.pointer->pointer, out_ptr);
      }
      MdsFree1Dx(&tmp, NULL);
    }
  }
  if (outcount && key_ptr->item_length == 0)
    for (j = outcount, hold_ptr -= step; --j >= 0; hold_ptr -= step)
      MdsFree1Dx((struct descriptor_xd *)hold_ptr, NULL);
  MdsFree1Dx(&holdxd, NULL);
  if (STATUS_OK && key_ptr->item_dtype == DTYPE_NID && out_ptr->pointer)
    out_ptr->pointer->dtype = DTYPE_NID;
  if (STATUS_OK && out_ptr->pointer && out_ptr->pointer->class == CLASS_A &&
      ((struct descriptor_a *)(out_ptr->pointer))->arsize == 0)
    status = TreeNNF;

  return status;
}
