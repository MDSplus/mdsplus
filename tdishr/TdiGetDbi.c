/*	Tdi1GetDbi.C
	Get data base information by name.
		GETDBI(STRING, [OFFSET])

	The search name can be shortened to a unique match.
	The full search name should be used in stored trees.

	Ken Klare, LANL P-4	(c)1990,1991
*/
#include "tdirefstandard.h"
#include <dbidef.h>
#include <strroutines.h>
#include <string.h>
#include <stdlib.h>
#include <tdimessages.h>
#include <treeshr.h>
#include <mdsshr.h>

static DESCRIPTOR(pathtext, "PATH");
static DESCRIPTOR(coloncolon, "::");
#define EOL {0,DbiEND_OF_LIST,0,0}

static struct item {
	char		*item_name;
	char		item_code;
	unsigned char	item_dtype;
	unsigned char	item_length;
} table[] = {
{"DEFAULT",		DbiDEFAULT,		DTYPE_T,	0},
{"MAX_OPEN",		DbiMAX_OPEN,		DTYPE_L,	4},
{"MODIFIED",		DbiMODIFIED,		DTYPE_BU,	1},
{"NAME",		DbiNAME,		DTYPE_T,	0},
{"NUMBER_OPENED",	DbiNUMBER_OPENED,	DTYPE_L,	4},
{"OPEN_FOR_EDIT",	DbiOPEN_FOR_EDIT,	DTYPE_BU,	1},
{"OPEN_READONLY",	DbiOPEN_READONLY,	DTYPE_BU,	1},
{"SHOTID",		DbiSHOTID,		DTYPE_L,	4},
};
#define siztab sizeof(struct item)
#define numtab (sizeof(table)/siztab)

extern int TdiData();
extern int TdiUpcase();
extern int TdiGetData();
extern int TdiGetLong();
extern int TdiEvaluate();

static int		compare (
struct descriptor	*s1,
struct item		s2[1])
{
int			cmp, len1 = s1->length, len2 = strlen(s2[0].item_name);
char			c0;

	/****************************************
	If smaller is mismatch, go on.
	(If len1 == len2, got it: cmp=0.)
	If len1 > len2, omit trailing blanks.
	If len1 < len2, check for unique match.
	****************************************/
	if ((cmp = strncmp(s1->pointer, s2[0].item_name, len1 < len2 ? len1 : len2)) != 0) ;
	else if (len1 > len2) cmp = (c0 = s1->pointer[len2]) != ' ' && c0 != '\t';
	else if (len1 < len2) {
		if ((s2 != &table[0] && strncmp(s1->pointer, s2[-1].item_name, len1) == 0)
		|| (s2 != &table[numtab-1] && strncmp(s1->pointer, s2[1].item_name, len1) == 0)) cmp = -1;
	}
	return cmp;
}
TdiRefStandard(Tdi1GetDbi)
struct descriptor_d		string = {0,DTYPE_T,CLASS_D,0};
struct descriptor_xd	tmp = EMPTY_XD;
struct item			*key_ptr=0;
int				index;
DBI_ITM lst[] = {{sizeof(index),DbiINDEX,0,0},EOL,EOL};
        lst[0].pointer = (unsigned char *)&index;
	/**********************
	String of item to find.
	**********************/
	status = TdiData(list[0], &tmp MDS_END_ARG);
	if (status & 1) status = TdiUpcase(&tmp, &string MDS_END_ARG);
	if (status & 1) {
		key_ptr = (struct item *)bsearch(&string, table, numtab, siztab, (int (*)(const void *,const void *))compare);
		if (key_ptr == 0) status = TdiBAD_INDEX;
	}
	StrFree1Dx(&string);
	MdsFree1Dx(&tmp, NULL);
	/**********************************
	Somebody might want others in pool.
	**********************************/
	if (status & 1 && narg > 1) status = TdiGetLong(list[1], &index);
	else index = 0;
	/***********************
	Get the item asked for.
	Fixed length or varying.
	***********************/
	if (status & 1) {
		lst[1].code = key_ptr->item_code;
		if ((lst[1].buffer_length = key_ptr->item_length) != 0) {
			status = MdsGet1DxS((unsigned short *)&lst[1].buffer_length, &key_ptr->item_dtype, out_ptr);
			if (status & 1) {
				lst[1].pointer = (unsigned char *)out_ptr->pointer->pointer;
				status = TreeGetDbi(lst);
			}
		}
		else {
		    lst[1].buffer_length = 0;
			lst[1].pointer = NULL;
			status = TreeGetDbi(lst);
			if (status & 1)
			{
				struct descriptor ans = {0,DTYPE_T,CLASS_S,0};
				if (lst[1].pointer) {
				  ans.length = strlen((char *)lst[1].pointer);
				  ans.pointer = (char *)lst[1].pointer;
				}
				status = MdsCopyDxXd(&ans, out_ptr);
				if (ans.pointer) TreeFree(ans.pointer);
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
static int fixup_nid( int *pin, /* NID pointer */int	arg, struct descriptor_d *pout)
{
	int status = 0;
	char *path = TreeGetPath(*pin);
	if (path != NULL)
	{
		unsigned short len = (unsigned short)strlen(path);
		StrCopyR(pout,&len,path);
		TreeFree(path);
		status = 1;
	}
	return status;
}

static fixup_path(struct descriptor *pin, int	arg, struct descriptor_d *pout) {
	int status = 0;
	char *pathin = MdsDescrToCstring(pin);
	char *path = TreeAbsPath(pathin);
	MdsFree(pathin);
	if (path != NULL)
	{
		unsigned short len = (unsigned short)strlen(path);
		StrCopyR(pout,&len,path);
		TreeFree(path);
		status = 1;
	}
	return status;
}

TdiRefStandard(Tdi1Using)
void *ctx;
int nid, shot, stat1;
struct descriptor def = {0,DTYPE_T,CLASS_D,0}, expt = def;
unsigned char	omits[] = {DTYPE_PATH,0};

	ctx = TreeSaveContext(&ctx);
	/**********************
	Evaluate with current.
	Use current if omitted.
	Must get expt if shot.
	**********************/
	if (narg > 1 && status & 1) {
		if (list[1]) {
		struct descriptor_xd xd = EMPTY_XD;
			status = TdiGetData(omits, list[1], &xd);
			if (status & 1 && xd.pointer) switch (xd.pointer->dtype) {
			case DTYPE_T :
			case DTYPE_PATH :
				status = StrCopyDx(&def, xd.pointer);
				break;
			default :
				status = TdiINVDTYDSC;
				break;
			}
			MdsFree1Dx(&xd, NULL);
		}
		if (!list[1] || def.length == 0) {
			DBI_ITM def_itm[] = {{0,DbiDEFAULT,0,0},EOL};
			status = TreeGetDbi(def_itm);
			if (def_itm[0].pointer == NULL)
			{
				static DESCRIPTOR(top,"\\TOP");
				StrCopyDx(&def, &top);
				status = 1;
			}
			else
			{
				unsigned short len = (unsigned short)strlen((char *)def_itm[0].pointer);
				StrCopyR(&def,&len,def_itm[0].pointer);
				TreeFree(def_itm[0].pointer);
			}
			if (status & 1) {
				stat1 = StrPosition(&def, &coloncolon, 0) + 1;
				status = StrRight(&def, &def, &stat1);
			}
			if (status & 1) *def.pointer = '\\';
		}
	}
	if ((narg > 2 && list[2] || narg > 3 && list[3]) && status & 1) {
		if (list[2]) status = TdiGetLong(list[2], &shot);
		else {
		DBI_ITM shot_itm[] = {{sizeof(shot),DbiSHOTID,0,0},EOL};
                        shot_itm[0].pointer = (unsigned char *)&shot;
			status = TreeGetDbi(shot_itm);
		}

		if (status & 1)
		if (narg > 3 && list[3]) status = TdiData(list[3], &expt MDS_END_ARG);
		else {
		DBI_ITM expt_itm[] = {{0,DbiNAME,0,0},EOL};
			status = TreeGetDbi(expt_itm);
			if (expt_itm[0].pointer)
			{
				unsigned short len = (unsigned short)strlen((char *)expt_itm[0].pointer);
				StrCopyR(&expt,&len,expt_itm[0].pointer);
				TreeFree(expt_itm[0].pointer);
			}
		}
		/*********************
		Set new tree and path.
		Allow some rel paths.
		*********************/
		if (status & 1)
		{
			char *tree = MdsDescrToCstring(&expt);
			status = TreeOpen(tree, shot, 1);
			MdsFree(tree);
		}
	}
	if (narg > 1) {
		char *path = MdsDescrToCstring(&def);
		if (status & 1) status = TreeSetDefault(path, &nid);
		MdsFree(path);
		if (narg > 2) StrFree1Dx(&expt);
		StrFree1Dx(&def);
	}
	/***********************
	Evaluate with temporary.
	***********************/
	if (status & 1) {
	struct descriptor_xd tmp = EMPTY_XD;
		status = TdiEvaluate(list[0], &tmp MDS_END_ARG);
		if (status & 1) status = MdsCopyDxXdZ((struct descriptor *)&tmp, out_ptr, NULL,
			fixup_nid, NULL, fixup_path, NULL);
		MdsFree1Dx(&tmp, NULL);
	}
	if (ctx) TreeRestoreContext(ctx);
	if (status & 1) status = stat1;
	return status;
}
