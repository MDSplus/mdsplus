static long	three = 3;
static long	nitem;		/*the row count*/
static int	nline;		/*character count on row*/
static char	*pline;		/*temporary line*/
#define MAXMSG 1024
static char	hold[MAXMSG];	/*decode/encode holder*/
static long	excess;		/*flag for unchecked argument count*/
static long	date;		/*flag for absolute day*/
#include <stdio.h>

typedef struct {
	unsigned m0:7;
	unsigned exp:8;
	unsigned sgn:1;
	unsigned short m1,m2,m3;}	DD;
typedef struct {
	unsigned m0:4;
	unsigned exp:11;
	unsigned sgn:1;
	unsigned short m1,m2,m3;}	GG;

static double CVTDG(double in)
{
  DD *d = (DD *)&in;
  GG g;
  g.sgn = d->sgn;
  g.exp = 1024-128+d->exp;
  g.m0 = (d->m0 >> 3);
  g.m1 = (d->m1 >> 3) | (d->m0 << 13);
  g.m2 = (d->m2 >> 3) | (d->m1 << 13);
  g.m3 = (d->m3 >> 3) | (d->m2 << 13);
  return *(double *)&g;
}

static double CVTGD(double in)
{
  GG *g = (GG *)&in;
  DD d;
  d.sgn = g->sgn;
  d.exp = 128-1024+g->exp;
  d.m0 = (g->m0 << 3) | (g->m1 >> 13);
  d.m1 = (g->m1 << 3) | (g->m2 >> 13);
  d.m2 = (g->m2 << 3) | (g->m3 >> 13);
  d.m3 = (g->m3 << 3);
  return *(double *)&d;
}

#pragma extern_model save
#pragma extern_model globalvalue
extern LIB$_INVSTRDES;
#pragma extern_model restore

#include <descrip.h>
#include <dvidef.h>
#include <math.h>	/*for pow definition*/
#include "SQL$INCLUDE.H"
#include <string.h>
#include <starlet.h>
#include <stdlib.h>
#include <str$routines.h>

extern void IEEE_TO_FLOAT();
extern void FLOAT_TO_IEEE();

extern int SQL_CLOSE();
extern int SQL_RELEASE();
extern int SQL_PREPARE();
extern int SQL_DESCRIBE_SELECT();
extern int SQL_DESCRIBE_PARM();
extern int SQL_DYNAMIC();
extern int SQL_EXECUTE();
extern int SQL_GET_ERR_TEXT();
extern int SQL_LIST_READ();
extern int SQL_LIST_WRITE();
extern int SQL_OPEN_CA();
extern int SQL_FETCH();
extern int USERSQL_ERRORS();
extern int USERSQL_SET();

typedef struct {
	unsigned long	l0;
	long		l1;
}	quad;

static $DESCRIPTOR(list_cu, "LIST_CU");
#include "export.h"
static struct SQLDA list_sele, list_mark;
static struct SQLCA list_ca = SQLCA_INIT;
static struct dsc$descriptor dhold = {sizeof(hold)-1,DSC$K_DTYPE_T,DSC$K_CLASS_S,hold};

static int	code = 0, list_id = 0, list2_id;
typedef struct {
	int	used;
	int	c;
	IDL_VPTR	*v;
}	ARGLIST;
static IDL_VPTR	vpcount, vperror, vpstatus, vptext;
static IDL_VARIABLE result = {IDL_TYP_LONG, 0};
static IDL_STRING	EMPTYSTRING = {0,0,0};
static double	HUGE_D = 1.7e+38;
static float	HUGE_F = 1.7e+38;
static long	HUGE_L = 0x7fffffff;
static long      quiet;
static short	HUGE_W = 0x7fff;
/*********************************************************/
static void IDLresize(k, dst)
long	k;
IDL_VPTR	dst;
{
 IDL_VPTR	tptr;
 void	*buf, *old;
 int	type, j;

	j = dst->value.arr->n_elts;
	if (k == j) return;
	type = dst->type;
	old = dst->value.arr->data;
	buf = IDL_MakeTempArray(type, 1, &k, IDL_BARR_INI_NOP, &tptr);
	if (k < j) j = k;
	switch (type) {
	case IDL_TYP_FLOAT : {
		float *in = old, *out = buf;
			for (;--j>=0;) *out++ = *in++;
			break;
		}
	case IDL_TYP_DOUBLE : {
		double *in = old, *out = buf;
			for (;--j>=0;) *out++ = *in++;
			break;
		}
	case IDL_TYP_LONG : {
		long *in = old, *out = buf;
			for (;--j>=0;) *out++ = *in++;
			break;
		}
	case IDL_TYP_INT : {
		short *in = old, *out = buf;
			for (;--j>=0;) *out++ = *in++;
			break;
		}
	case IDL_TYP_STRING : {
		IDL_STRING *in = old, *out = buf;
			for (;--j>=0;) {*out++ = *in; *in++ = EMPTYSTRING;}
			break;
		}
 	/***array of string descriptors***/
	}
	IDL_VarCopy(tptr, dst);
}
/*********************************************************/
int	IDLSQL_PUTS(struct SQLDA *sele, int *prows, ARGLIST *arg, int rblob)
{
 int		rows = *prows, used = arg->used;
 IDL_VPTR		*argv = arg->v, tptr, dst, uptr;
 void		*buf, *old;
 struct SQLVAR	*pda;
 int		ind, j, k, len, scale, type=-1;
 IDL_ALLTYPES	temp;
	static char adate[23+1]/*"dd-mmm-yyyy hh:mm:ss.cc" must be writable*/;
	$DESCRIPTOR(ddate, adate);
	int	isnotseg, ncol = sele->SQLD;

	if (rows == 0) {
		if (excess) {
			if (used + ncol <= arg->c) return 1;
		}
		else	if (used + ncol == arg->c) return 1;
		sprintf(hold, "Expect %d parameters + %d selects, %d named.",
			used, ncol, arg->c);
		return 0;
	}
	for (j = 0; j < ncol; ++j, ++used) {
		dst = *(argv+used);
		pda = &sele->SQLVARS[j];
		isnotseg = (pda->SQLTYPE & ~1) != SQL_TYPE_SEGMENT_ID;
		if (rows < -1) {
			if (isnotseg) IDLresize(-(rows+1), dst);	/*remove excess*/
			continue;
		}
		temp.str = EMPTYSTRING;
		if (rblob || isnotseg) {IDL_EXCLUDE_EXPR(dst);}
		len = pda->SQLLEN;
		scale = len >> 8;
		buf = pda->SQLDATA;
		ind = pda->SQLIND && *pda->SQLIND < 0 || rows < 0;
		switch (pda->SQLTYPE & ~1) {
		case SQL_TYPE_VARCHAR :
			type = IDL_TYP_STRING;
			if (!ind) {
				len = *(unsigned short *)buf;
				IDL_StrEnsureLength(&temp.str, len);
                                if (temp.str.s)
                                {
  				  temp.str.s[len] = '\0';
				  memcpy(temp.str.s, (char *)buf+sizeof(short), len);
                                }
			}
			break;
		case SQL_TYPE_CHAR :
			type = IDL_TYP_STRING;
			if (!ind) {
				IDL_StrEnsureLength(&temp.str, len);
                                if (temp.str.s)
                                {
				  temp.str.s[len] = '\0';
				  memcpy(temp.str.s, (char *)buf, len);
                                }
			}
			break;
		case SQL_TYPE_FLOAT :
			if (len == 4)	{type = IDL_TYP_FLOAT; temp.f = ind ? HUGE_F : *(float *)buf;}
			else		{type = IDL_TYP_DOUBLE; temp.d = ind ? HUGE_D : CVTGD(*(double *)buf);}
			break;
		case SQL_TYPE_INTEGER :
			if (scale) {type = IDL_TYP_DOUBLE; temp.d = ind ? HUGE_D: *(long *)buf / pow(10e0, (double)scale);}
			else {type = IDL_TYP_LONG; temp.l = ind ? HUGE_L : *(long *)buf;}
			break;
		case SQL_TYPE_SMALLINT :
			if (scale) {type = IDL_TYP_FLOAT; temp.f = ind ? HUGE_F : *(short *)buf / pow(10e0, (double)scale);}
			else {type = IDL_TYP_INT; temp.i = ind ? HUGE_W : *(short *)buf;}
			break;
		case SQL_TYPE_DATE : /***convert to text***/
			if (!ind)
			if (date) {
	 			type = IDL_TYP_DOUBLE;
				temp.d = ind ? HUGE_D : ((((quad *)buf)->l1 * pow(2e0, 32e0)
				+ ((quad *)buf)->l0) / pow(10e0, (double)scale) / 86400e7); /*units are 100ns*/
			}
			else {
				type = IDL_TYP_STRING;
				if (sys$asctim(0, &ddate, (int *)buf, 0) & 1)
					IDL_StrStore(&temp.str, ddate.dsc$a_pointer);
				else IDL_StrStore(&temp.str, "bad date");
			}
                        else {
			  type = IDL_TYP_STRING;
                          IDL_StrStore(&temp.str, "");
                        }
			break;
		case SQL_TYPE_QUADWORD : /***truncated***/
			type = IDL_TYP_DOUBLE;
			temp.d = ind ? HUGE_D : (((quad *)buf)->l1 * pow(2e0, 32e0))
			+ ((quad *)buf)->l0 / pow(10e0, (double)scale);
			break;
		case SQL_TYPE_TINYINT : /***convert to smallest signed***/
			if (scale) {type = IDL_TYP_FLOAT; temp.f = ind ? HUGE_F : *(char *)buf / pow(10e0, (double)scale);}
			else {type = IDL_TYP_INT; temp.i = ind ? HUGE_W : *(char *)buf;}
			break;
		case SQL_TYPE_SEGMENT_ID :
			if (len == 8) {
			char stmt[71];
			struct dsc$descriptor dstmt = {0,DSC$K_DTYPE_T,DSC$K_CLASS_S,0};
			IDL_STRING *pstring;
			int status = 1, jj;
                                dstmt.dsc$a_pointer = stmt;
				if (rblob && ind) {		/*NULL read for this row*/
					if (rows <= 1)
					IDL_StoreScalar(dst, IDL_TYP_STRING, (IDL_ALLTYPES *)&EMPTYSTRING);
					continue;
				}
				strcpy(stmt, "SELECT ");
				strncat(stmt, pda->SQLNAME, pda->SQLNAME_LEN);
				strcat(stmt, " WHERE CURRENT OF STREAM_DB");
				dstmt.dsc$w_length = strlen(stmt);
				if (code)	{SQL_CLOSE(&code, &list_cu); code = 0;}
				if (list_id)	{SQL_RELEASE(&code, &list_id); list_id = 0;}
				list_mark.SQLN = 0;
				list_sele.SQLN = 1;
				if (list_sele.SQLVARS[0].SQLDATA) free(list_sele.SQLVARS[0].SQLDATA);
				status = SQL_PREPARE(&code, &dstmt, &list_id);
				if (status & 1) status = SQL_DESCRIBE_SELECT(&code, &list_id, &list_sele);
				if (status & 1)
				if (rblob)	status = SQL_LIST_READ(&code, &list_id, &list_cu);
				else		status = SQL_LIST_WRITE(&code, &list_id, &list_cu);
				if (status & 1) status = SQL_OPEN_CA(&list_ca, &list_cu, &list_mark);
				if (!(status & 1)) {USERSQL_ERRORS(); goto err3;}
				if (rblob) {
					type = IDL_TYP_STRING;
					buf = IDL_MakeTempArray(type, 1, (long *)&list_ca.SQLERRD[3], IDL_BARR_INI_NOP, &uptr);
					pstring = (IDL_STRING *)uptr->value.arr->data;
					list_sele.SQLVARS[0].SQLDATA = IDL_GetScratch(&tptr, 1, list_ca.SQLERRD[1]+2);
					for (jj = 0; jj < list_ca.SQLERRD[3]; ++jj, ++pstring) {
						status = SQL_FETCH(&code, &list_cu, &list_sele);
						if (!(status & 1)) break;
						k = *(unsigned short *)list_sele.SQLVARS[0].SQLDATA;
						IDL_StrEnsureLength(pstring, k);
                                                if (pstring->s)
						  memcpy(pstring->s, (char *)list_sele.SQLVARS[0].SQLDATA+2, k);
					}
					IDL_Deltmp(tptr);
					if (status & 1)
					if (rows <= 1 || !(dst->flags & IDL_V_ARR)) {
						IDL_VarCopy(uptr, dst);				/*first good seg*/
					}
					else {							/*accumulate*/
					IDL_ARRAY	*aold=dst->value.arr,	*anew=uptr->value.arr;
					int	nold=aold->n_elts,	nnew=anew->n_elts,	nboth=nold+nnew;
					IDL_STRING	*pold=(IDL_STRING *)aold->data,	*pnew=(IDL_STRING *)anew->data,	*pboth;
						pboth = (IDL_STRING *)IDL_MakeTempArray(type, 1, (long *)&nboth, IDL_BARR_INI_NOP, &tptr);
						for (; --nold >= 0;) {*pboth++ = *pold; *pold++ = EMPTYSTRING;}
						for (; --nnew >= 0;) {*pboth++ = *pnew; *pnew++ = EMPTYSTRING;}
						IDL_Deltmp(uptr);
						IDL_VarCopy(tptr, dst);
					}
				}
				else {
				static $DESCRIPTOR(dins, "INSERT INTO CURSOR LIST_CU VALUES (?)");
					status = SQL_PREPARE(&code, &dins, &list2_id);
					list_mark.SQLN = 1;
					if (status & 1) status = SQL_DESCRIBE_PARM(&code, &list2_id, &list_mark);
					list_mark.SQLVARS[0].SQLTYPE = SQL_TYPE_CHAR;
					if (dst->type == IDL_TYP_UNDEF) jj = 0;
					else if (dst->flags & IDL_V_ARR) {
						pstring = (IDL_STRING *)dst->value.arr->data;
						jj = dst->value.arr->n_elts;
					}
					else {
						pstring = &dst->value.str;
						jj = 1;
					}
					if (dst->type == IDL_TYP_STRING) {
						for (; --jj >= 0 && status & 1; ++pstring) {
							list_mark.SQLVARS[0].SQLDATA = pstring->s;
							list_mark.SQLVARS[0].SQLLEN = pstring->slen;
							status = SQL_EXECUTE(&code, &list2_id, &list_mark);
						}
					}
					else status = LIB$_INVSTRDES;
					list_mark.SQLVARS[0].SQLDATA = 0;
					list_mark.SQLD = 0;
					list_mark.SQLN = 0;
					SQL_RELEASE(&code, &list2_id); list2_id = 0;
				}
				list_sele.SQLVARS[0].SQLDATA = 0;
				SQL_CLOSE(&code, &list_cu); code = 0;
err3:				SQL_RELEASE(&code, &list_id); list_id = 0;
				if (!(status & 1)) {
					sprintf(hold, "%5d max %5d seg %9d total",
					list_ca.SQLERRD[1],
					list_ca.SQLERRD[3],
					list_ca.SQLERRD[4]);
					return 0;
				}
			}
			else {
				sprintf(hold, "Seg-id len%6d", len);
				return 0;
			}
			continue;
		default :
			sprintf(hold, "Select %.*s of type %d is unsupport.",
			pda->SQLNAME_LEN, pda->SQLNAME, pda->SQLTYPE);
			return 0;
		}
		/*****************************************************************************
		* When we just add one more for each row the number of operations goes as n*n.
		* This way be grow the array geometrically and waste less time, n*log(n).
		*****************************************************************************/
		if (rows == -1) {IDL_StoreScalar(dst, type, &temp);}/*a scalar of the type*/
		else {
			if (rows == 1) {			/*initial guess for count*/
				buf = IDL_MakeTempArray(type, 1, &three, IDL_BARR_INI_NOP, &tptr);
				IDL_VarCopy(tptr, dst);
			}
			else if (rows > dst->value.arr->n_elts) {
				IDLresize(3*(rows-1), dst);	/*grab some more*/
			}
			buf = dst->value.arr->data;
			switch (type) {
			case IDL_TYP_FLOAT :	*((float *)buf	+(rows-1)) = temp.f;	break;
			case IDL_TYP_DOUBLE :	*(((double *)buf)	+(rows-1)) = temp.d;	break;
			case IDL_TYP_LONG :		*((long *)buf	+(rows-1)) = temp.l;	break;
			case IDL_TYP_INT :		*((short *)buf	+(rows-1)) = temp.i;	break;
			case IDL_TYP_STRING :	*((IDL_STRING *)buf	+(rows-1)) = temp.str;	break;
			}
		}
	}
	return 1;
}
/*********************************************************/
int		IDLSQL_GETS(struct SQLDA *mark, ARGLIST *arg) {
int		ncol = mark->SQLD;
struct SQLVAR	*pda;

int		used = arg->used;
IDL_VPTR		*argv = arg->v, src, src0, src1;
void		*buf;
int		count = 0, ind, j, len, scale;

	for (j = 0; j < ncol; ++j) {
		if (used >= arg->c) {
			sprintf(hold, "Expect %d parameters, %d given.", arg->used, arg->c);
			return 0;
		}
		src0 = *(argv+used);
		IDL_ENSURE_SIMPLE(src0);
		/** assumes nonzero array size **/
		if (excess && (src0->flags & IDL_V_ARR)) {
			src = IDL_Gettmp();
			buf = src0->value.arr->data;
		/** because IDL does not allow this for temporaries.	**/
		/**	len = src0->value.arr->elt_len;			**/
		/**	IDL_StoreScalar(src, src0->type, buf+count*len)	**/
		/** do it the long way **/
			src->type = src0->type;
			switch (src0->type) {
			case IDL_TYP_BYTE :		src->value.c	= *((UCHAR *)buf+count); break;
			case IDL_TYP_INT :		src->value.i	= *((short *)buf+count); break;
			case IDL_TYP_LONG :		src->value.l	= *((long *)buf+count); break;
			case IDL_TYP_FLOAT :	src->value.f	= *((float *)buf+count); break;
			case IDL_TYP_DOUBLE :	src->value.d	= *((double *)buf+count); break;
			case IDL_TYP_COMPLEX :	src->value.cmp	= *((IDL_COMPLEX *)buf+count); break;
			case IDL_TYP_STRING :	src->value.str	= *((IDL_STRING *)buf+count);
				src->value.str.stype = 0; /**not a copy, don't discard**/
				break;
			}
			if (++count >= src0->value.arr->n_elts) {
				count = 0;
				++used;
			}
		}
		else {
			src = src0;
			++used;
		}
		src1 = src;
		IDL_ENSURE_SCALAR(src);
		pda = &mark->SQLVARS[j];
		buf = pda->SQLDATA;
		len = pda->SQLLEN;
		scale = len >> 8;
		ind = 0;
		if (src->type == IDL_TYP_UNDEF) ind = -1;
		else switch (pda->SQLTYPE & ~1) {
		case SQL_TYPE_ASCIZ :
			IDL_ENSURE_STRING(src);
			if (src->value.str.slen == 0) ind = -1;
			else strncpy((char *)buf, src->value.str.s, len);
			break;
		case SQL_TYPE_VARCHAR :
			IDL_ENSURE_STRING(src);
			if (src->value.str.slen == 0) ind = -1;
			else {
				if (src->value.str.slen < len) len = src->value.str.slen;
                                *(unsigned short *)buf = len;
				memcpy(((char *)buf)+sizeof(short), src->value.str.s, len);
			}
			break;
		case SQL_TYPE_CHAR :
			IDL_ENSURE_STRING(src);
			if (src->value.str.slen == 0) ind = -1;
			else if (src->value.str.slen < len) {
				memcpy((char *)buf, src->value.str.s, src->value.str.slen);
				memset((char *)buf+src->value.str.slen, ' ',
					len - src->value.str.slen);		/*twf*/
			}
			else	memcpy((char *)buf, src->value.str.s, len);
			break;
		case SQL_TYPE_FLOAT :
			if	(len == 4) {src = IDL_CvtFlt(1, &src); *(float *)buf = src->value.f;}
			else if	(len == 8) {src = IDL_CvtDbl(1, &src); *(double *)buf = CVTDG(src->value.d);}
			else	ind = -1;
			break;
		case SQL_TYPE_INTEGER :
			if (scale) goto scaled;
			src = IDL_CvtLng(1, &src);
			*(long *)buf = src->value.l;
			break;
		case SQL_TYPE_SMALLINT :
			if (scale) goto scaled;
			src = IDL_CvtFix(1, &src);
			*(short *)buf = src->value.i;
			break;
		case SQL_TYPE_DATE :
			if (date) {
				*(double *)buf = CVTDG(src->value.d) / pow(10e0, (double)scale) * 86400e7;
				pda->SQLLEN = 8;
				pda->SQLTYPE = SQL_TYPE_FLOAT;
				break;
			}
			src = IDL_CvtString(1, &src, 0);
			{struct dsc$descriptor ddate = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};
                                ddate.dsc$w_length = src->value.str.slen;
                                ddate.dsc$a_pointer = src->value.str.s;
				ind = src->value.str.slen
				&& (str$upcase(&ddate, &ddate) & 1)
				&& (sys$bintim(&ddate, buf) & 1) ? 0 : -1;
			}
			break;
		case SQL_TYPE_QUADWORD : /*** truncation ***/
scaled:			src = IDL_CvtDbl(1, &src);
			*(double *)buf = CVTDG(src->value.d) / pow(10e0, (double)scale);
			pda->SQLLEN = 8;
			pda->SQLTYPE = SQL_TYPE_FLOAT;
			break;
		case SQL_TYPE_TINYINT :
			if (scale) goto scaled;
			src = IDL_BasicTypeConversion(1, &src, IDL_TYP_BYTE);
			*(UCHAR *)buf = src->value.c;
			break;
		default :
			sprintf(hold, "Param %.*s of unsupported type %d.",
			pda->SQLNAME_LEN, pda->SQLNAME, pda->SQLTYPE);
			return 0;
		}
		*pda->SQLIND = ind;
		if (src != src1) IDL_Deltmp(src);	/* conversion temp */
		if (src1 != src0) IDL_Deltmp(src1);	/* array element temp */
	}
	if (count) ++used;
	arg->used = used;
	return 1;
}
/*********************************************************/
static struct dsc$descriptor dtext = {0,DSC$K_DTYPE_T,DSC$K_CLASS_S,0};
static void set_them(rows, status)
int	rows;
int	status;
{
int	len = 0;

	if (vpcount) IDL_StoreScalar(vpcount, IDL_TYP_LONG, (IDL_ALLTYPES *)&rows);
	if (vpstatus) IDL_StoreScalar(vpstatus, IDL_TYP_LONG, (IDL_ALLTYPES *)&status);
	if (vperror) {
		IDL_StoreScalar(vperror, IDL_TYP_STRING, (IDL_ALLTYPES *)&EMPTYSTRING);
		if (!(status & 1)) {
			if (status) {
				SQL_GET_ERR_TEXT(&dhold, &len);
				hold[len] = '\0';
			}
			IDL_StrStore(&vperror->value.str, hold);
		}
	}
	if ((status != 1))
	if (!quiet) {
		if (status) USERSQL_ERRORS();
		else
		IDL_MessageErrno(IDL_M_NAMED_GENERIC, 0, IDL_MSG_RET, hold);
	}
}
/*********************************************************/
IDL_VPTR	IDLSQL_DYNAMIC(argc, argv_in, argk)
int	argc;
IDL_VPTR	argv_in[];
char	*argk;
{
int		rows, status;
IDL_VPTR		argv[IDL_MAXPARAMS+1];
ARGLIST		user_args;
static IDL_STRING host;
static IDL_KW_PAR kw_list[] = {
	{"COUNT",	IDL_TYP_UNDEF,	1,IDL_KW_OUT|IDL_KW_ZERO,	0,	IDL_CHARA(vpcount)},
	{"DATE",	IDL_TYP_LONG,	1,IDL_KW_ZERO,		0,	IDL_CHARA(date)},
	{"ERROR",	IDL_TYP_UNDEF,	1,IDL_KW_OUT|IDL_KW_ZERO,	0,	IDL_CHARA(vperror)},
	{"EXCESS",	IDL_TYP_LONG,	1,IDL_KW_ZERO,		0,	IDL_CHARA(excess)},
	{"QUIET",	IDL_TYP_LONG,	1,IDL_KW_ZERO,		0,	IDL_CHARA(quiet)},
	{"STATUS",	IDL_TYP_UNDEF,	1,IDL_KW_OUT|IDL_KW_ZERO,	0,	IDL_CHARA(vpstatus)},
	{NULL}};

        IEEE_TO_FLOAT(argc, argv_in);
        hold[0] = '\0';
	user_args.used = 0;
	user_args.c = argc-1;
	user_args.v = &argv[1];
	IDL_KWCleanup(IDL_KW_MARK);
	user_args.c = IDL_KWGetParams(argc, argv_in, argk, kw_list, argv, 1) - 1;
	IDL_ENSURE_STRING(argv[0]);
	IDL_ENSURE_SCALAR(argv[0]);
	dtext.dsc$w_length = strlen(dtext.dsc$a_pointer = argv[0]->value.str.s);
	status = SQL_DYNAMIC(
		IDLSQL_GETS,	/*routine to fill markers	*/
		IDLSQL_PUTS,	/*routine to store selctions	*/
		&dtext,		/*text string descriptor	*/
		&user_args,	/*value passed to GETS and PUTS	*/
		&rows);		/*output, number of rows	*/
	result.value.l = rows;
	set_them(rows, status);
	IDL_KWCleanup(IDL_KW_CLEAN);
        FLOAT_TO_IEEE(argc, argv_in);
	return &result;
}
/*********************************************************/
int	mygets(input)
char	*input;
{
if (IDL_Rline(input, 1024+38, IDL_STDIN_UNIT, 0, 1, "", 0)) return strlen(input);
else	return -1;
}
/*********************************************************/
static int	xprintf(form, arg1, arg2, arg3, arg4)
char	*form;
{
	return printf(form, arg1, arg2, arg3, arg4);
}
/*********************************************************/
static void	txtadd(form, arg1, arg2, arg3, arg4)
char	*form;
{
	nline += sprintf(pline+nline, form, arg1, arg2, arg3, arg4);
}
/*********************************************************/
static void	txtline(form, arg1, arg2, arg3, arg4)
char	*form;
{
	sprintf(pline+nline, "\r\n");
	nline += 2;
}
/*********************************************************/
static void	txtitem(form, arg1, arg2, arg3, arg4)
char	*form;
{
long	j;
IDL_VPTR	vpnew = 0;
IDL_STRING	*psold, *psnew;

	if (nitem > 0) psold = (void *)vptext->value.arr->data;
	++nitem;
	psnew = (void *)IDL_MakeTempArray(IDL_TYP_STRING, 1, &nitem, IDL_BARR_INI_NOP, &vpnew);
	for (j = nitem; --j > 0;) {
		*psnew++ = *psold;
		*psold++ = EMPTYSTRING;
	}
	IDL_StrStore(psnew, pline);
	nline = 0;
	IDL_VarCopy(vpnew, vptext);
}
/*********************************************************/
IDL_VPTR	IDLSQL_INTERACTIVE(argc, argv_in, argk)
int	argc;
IDL_VPTR	argv_in[];
char	*argk;
{
 int		rows, status;
 extern	USERSQL_GETS();
 extern	USERSQL_PUTS();
 IDL_VPTR		argv[1], vpline;
 static long   head;
 static int    head_present;
 static long   width;
 static int    width_present;
 static IDL_STRING host;
 static IDL_KW_PAR kw_list[] = {
	{"COUNT",	IDL_TYP_UNDEF,	1,IDL_KW_OUT|IDL_KW_ZERO,	0,	IDL_CHARA(vpcount)},
	{"ERROR",	IDL_TYP_UNDEF,	1,IDL_KW_OUT|IDL_KW_ZERO,	0,	IDL_CHARA(vperror)},
	{"HEAD",	IDL_TYP_LONG,	1,	       0,	&head_present,	IDL_CHARA(head)},
	{"QUIET",	IDL_TYP_LONG,	1,IDL_KW_ZERO,		0,	IDL_CHARA(quiet)},
	{"STATUS",	IDL_TYP_UNDEF,	1,IDL_KW_OUT|IDL_KW_ZERO,	0,	IDL_CHARA(vpstatus)},
	{"TEXT",	IDL_TYP_UNDEF,	1,IDL_KW_OUT|IDL_KW_ZERO,	0,	IDL_CHARA(vptext)},
	{"WIDTH",	IDL_TYP_LONG,	1,	       0,	&width_present,	IDL_CHARA(width)},
	{NULL}};

        IEEE_TO_FLOAT(argc, argv_in);
	IDL_KWCleanup(IDL_KW_MARK);
	IDL_KWGetParams(argc, argv_in, argk, kw_list, argv, 1);
	IDL_ENSURE_STRING(argv[0]);
	IDL_ENSURE_SCALAR(argv[0]);
	if (!head_present) head = 2;
	if (!width_present || width <= 0) width = IDL_FileTermColumns();
	dtext.dsc$w_length = strlen(dtext.dsc$a_pointer = argv[0]->value.str.s);
	if (vptext) {
		nitem = 0;
		nline = 0;
		IDL_StoreScalar(vptext, IDL_TYP_STRING, (IDL_ALLTYPES *)&EMPTYSTRING);
		USERSQL_SET(mygets, txtadd, txtline, txtitem, width, head);
		pline = IDL_GetScratch(&vpline, 32768, sizeof(char));
	}
	else	USERSQL_SET(mygets, xprintf, xprintf, xprintf, width, head);
	status = SQL_DYNAMIC(
		USERSQL_GETS,	/*routine to fill markers	*/
		USERSQL_PUTS,	/*routine to store selctions	*/
		&dtext,		/*text string descriptor	*/
		(ARGLIST *)&width,/*value passed to GETS and PUTS	*/
		&rows);		/*output, number of rows	*/
	result.value.l = rows;
	if (vptext) IDL_Deltmp(vpline);
	set_them(rows, status);
	IDL_KWCleanup(IDL_KW_CLEAN);
        FLOAT_TO_IEEE(argc, argv_in);
	return &result;
}
/*********************************************************/
IDL_VPTR	IDLSQL_LOGOUT(argc, argv_in, argk)
int	argc;
IDL_VPTR	argv_in[];
char	*argk;
{
  result.value.l = 0;
  return &result;
}

/*********************************************************/
IDL_VPTR	IDLSQL_LOGIN(argc, argv_in, argk)
int	argc;
IDL_VPTR	argv_in[];
char	*argk;
{
  result.value.l = 0;
  return &result;
}

/*********************************************************/
IDL_VPTR	IDLSQL_SETDBTYPE(argc, argv_in, argk)
{
  result.value.l = 0;
  return &result;
}
