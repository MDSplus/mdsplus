/*	Tdi1SQL.C
	An interface to dynamic SQL within TDI.
	Links are dynamic to keep out Rdb libraries until wanted.
	rows = ISQL("SQL-string")
	Interaction is with terminal.

	rows = DSQL("SQL-string", question-mark-replacements, ... output-variables, ...)
	If inputs are missing they are supplied as indicator asserted.
	Useful only for INSERT and UPDATE statements, probably.

	Output may be a variable (defined or not)
	or text string of variable name but not an expression for name.
	If an output for selection XXX is NULL pointer
	then variable of name _XXX is created.
	Note for calculated select, name will be just "_".
	Note same names from different tables will be folded together.

	The SQL string sElEcT is a blob write.

	Ken Klare, LANL P-4	(c)1991,1992
*/
#define _MOVC3(a,b,c) memcpy(c,b,a)
#include <mdsdescrip.h>
#include <mdsdescrip.h>
#include <math.h>	/*for pow definition*/
#include "tdirefstandard.h"
#include <string.h>
#include <strroutines.h>
#include <libroutines.h>
#include <stdlib.h>
#include <stdio.h>
#include <tdimessages.h>
#include <mdsshr.h>
#include <mds_stdarg.h>
#ifdef __VMS
#include <dvidef.h>	/*for line width*/
#include <starlet.h>
#endif

#ifdef  __ALPHA			/**---AXP---**/
#undef   DTYPE_D
#define  DTYPE_D DTYPE_G
#endif

extern int TdiFindImageSymbol();
extern int TdiGetLong();
extern int TdiGetIdent();
extern int TdiPutIdent();
extern int TdiData();
extern int TdiVector();
extern int TdiText();

/*"mds$source:[sqlshr.work]sql$include.h"*/
/*----------------------------------------------------------*/
struct SQLCA {
        char    SQLCAID[8];     /*identifier of this block      */
        int     SQLCABC;        /*byte count of this block      */
        int     SQLCODE;        /*error code                    */
        struct {
                short   SQLERRML;
                char    SQLERRMC[70];
        }       SQLERRM;        /*error message                 */
        int     SQLERRD[6];     /*[0] unused                    */
                                /*[1] longest segment           */
                                /*[2] number of rows            */
                                /*[3] number of segments        */
                                /*[4-5] quadword total bytes    */
        struct {
                char    SQLWARN0,SQLWARN1,SQLWARN2,SQLWARN3;
                char    SQLWARN4,SQLWARN5,SQLWARN6,SQLWARN7;
        }       SQLWARN;        /*unused DB2 compatibility      */
        char    SQLEXT[8];      /*unknown extension             */
};
#define SQLCA_INIT      {"SQLCA   ",128,0,{0,""},{0,0,0,0,0,0},\
                {'\0','\0','\0','\0','\0','\0','\0','\0'},"        "}
/*----------------------------------------------------------*/
#define SQL_TYPE_VARCHAR	448 /*short counted string	*/
#define SQL_TYPE_CHAR		452 /*fixed length string	*/
#define SQL_TYPE_FLOAT		480 /*REAL*4 or REAL*8		*/
#define SQL_TYPE_DECIMAL	484 /*never returned (scalable)	*/
#define SQL_TYPE_INTEGER	496 /*signed long (scalable)	*/
#define SQL_TYPE_SMALLINT	500 /*signed word (scalable)	*/
#define SQL_TYPE_DATE		502 /*use SYS$BINTIM SYS$ASCTIM	*/
#define SQL_TYPE_QUADWORD	504 /*quadword (scalable)	*/
#define SQL_TYPE_ASCIZ		506 /*never returned		*/
#define SQL_TYPE_SEGMENT_ID	508 /*8-byte			*/
#define SQL_TYPE_TINYINT	514 /*signed byte		*/
#define SQL_TYPE_VARBYTE	516 /*no nulls, only fetches of list elements*/
/*----------------------------------------------------------*/
struct SQLVAR {
	short		SQLTYPE;	/*data type as above	*/
	unsigned short	SQLLEN;		/*data length (scale)	*/
	char		*SQLDATA;	/*pointer to data	*/
	short		*SQLIND;	/*pointer to indicator	*/
	short		SQLNAME_LEN;	/*name length		*/
	char		SQLNAME[30];	/*name			*/
};
struct SQLDA {
	char	SQLDAID[8];	/*identifier			*/
	int	SQLABC;		/*byte count			*/
	short	SQLN;		/*allocated number (program)	*/
	short	SQLD;		/*described number		*/
	struct	SQLVAR	SQLVARS[1];
};

static DESCRIPTOR(dunderscore,	"_");
static int	code = 0, list_id = 0, list2_id;
static DESCRIPTOR(list_cu, "LIST_CU");
static struct SQLDA list_sele, list_mark;
static struct SQLCA list_ca = SQLCA_INIT;
typedef struct {
	unsigned int	l0;
	int		l1;
}	quadw;
typedef struct {
	int			used;
	int			c;
	struct descriptor	**v;
}	ARGLIST;
static double	HUGE_D = 1.7e+38;
static float	HUGE_F = (float)1.7e+38;
static quadw	HUGE_Q = {0xffffffff,0x7fffffff};
static int	HUGE_L = 0x7fffffff;
static short	HUGE_W = 0x7fff;
static char	HUGE_B = 0x7f;

static DESCRIPTOR(ddate, "dd-mmm-yyyy hh:mm:ss.cc");
static int	width = 0, head = 2;
static int	(*USERSQL_ERRORS)() = 0;
static int	(*USERSQL_GETS)() = 0;
static int	(*USERSQL_PUTS)() = 0;
static int	(*USERSQL_SET)() = 0;
static int	(*SQL_DYNAMIC)() = 0;
static int	(*SQL_CLOSE)() = 0;
static int	(*SQL_DESCRIBE_PARM)() = 0;
static int	(*SQL_DESCRIBE_SELECT)() = 0;
static int	(*SQL_EXECUTE)() = 0;
static int	(*SQL_FETCH)() = 0;
static int	(*SQL_LIST_READ)() = 0;
static int	(*SQL_LIST_WRITE)() = 0;
static int	(*SQL_OPEN_CA)() = 0;
static int	(*SQL_PREPARE)() = 0;
static int	(*SQL_RELEASE)() = 0;
/*********************************************************/
static int	TDISQL_LINK(char *name, int (**routine)()) {
static DESCRIPTOR(dimage,	"MDSSQLSHR");
struct descriptor dname = {0,DTYPE_T,CLASS_S,0};
int	status;
dname.length = (unsigned short)strlen(name);
dname.pointer = name;
status = TdiFindImageSymbol(&dimage, &dname, routine);
return status;
}
/*********************************************************/
int	TDISQL_PUTS(struct SQLDA *sele, int *prows, ARGLIST *arg, int rblob) {
int				rows = *prows, used = arg->used;
struct descriptor_xd	**argv = (struct descriptor_xd **)arg->v, tmp = EMPTY_XD;
struct descriptor		*dst;
struct descriptor_d		madeup = {0,DTYPE_T,CLASS_D,0};
char				*buf;
struct SQLVAR			*pda;
int				ind, j, len, scale, type=0, status = 1;
static struct descriptor name = {0,DTYPE_T,CLASS_S,0};

	if (rows == 0) {
		/*if (used + sele->SQLD < arg->c) status = TdiEXTRA_ARG;*/
		/*if (used + sele->SQLD > arg->c) status = TdiMISS_ARG;*/
	}
	else for (j = 0; j < sele->SQLD && status & 1; ++j, ++used) {
		pda = &sele->SQLVARS[j];
		dst = (struct descriptor *)(used + sele->SQLD > arg->c ? 0 : *(argv+used));
		while (dst && dst->dtype == DTYPE_DSC) dst = (struct descriptor *)dst->pointer;
		if (dst == 0 && (rblob || (pda->SQLTYPE & ~1) != SQL_TYPE_SEGMENT_ID)) {
			name.length = pda->SQLNAME_LEN;
			name.pointer = pda->SQLNAME;
 			status = StrConcat(dst = (struct descriptor *)&madeup, &dunderscore, &name MDS_END_ARG);
			if (!(status & 1)) break;
			dst->dtype = DTYPE_IDENT;
		}
		if (rows == -1) status = TdiPutIdent(dst, &tmp);
		if (rows <= 0) continue;
		len = pda->SQLLEN;
		scale = len >> 8;
		buf = (char *)pda->SQLDATA;
		ind = pda->SQLIND && *pda->SQLIND < 0;
		switch (pda->SQLTYPE & ~1) {
		case SQL_TYPE_VARCHAR :
			type = DTYPE_T;
			if (!ind) {
				len = *(unsigned short *)buf;;
				buf += sizeof(short);
			}
			break;
		case SQL_TYPE_CHAR :
			type = DTYPE_T;
			break;
		case SQL_TYPE_FLOAT :
			if (len == 4)	{type = DTYPE_F; if (ind) buf = (char *)&HUGE_F;}
			else		{type = DTYPE_D; if (ind) buf = (char *)&HUGE_D;}
			break;
		case SQL_TYPE_INTEGER :
			if (scale) {
				len = sizeof(double);
				type = DTYPE_D;
				if (ind) buf = (char *)&HUGE_D;
				else { double *dptr = (double *)buf; *dptr = *(int *)buf / pow(10e0, (double)scale);}
			}
			else		{type = DTYPE_L; if (ind) buf = (char *)&HUGE_L;}
			break;
		case SQL_TYPE_SMALLINT :
			if (scale) {
				len = sizeof(float);
				type = DTYPE_F;
				if (ind) buf = (char *)&HUGE_F;
				else { float *fptr = (float *)buf; *fptr = (float)(*(short *)buf / pow(10e0, (double)scale));}
			}
			else		{type = DTYPE_W; if (ind) buf = (char *)&HUGE_W;}
			break;
		case SQL_TYPE_DATE : /***convert to text***/
			type = DTYPE_T;
			LibSysAscTim(0, &ddate, buf, 0);
			buf = ddate.pointer;
			len = ddate.length;
			break;
		case SQL_TYPE_QUADWORD : /***truncated if scaled***/
			if (scale) {
				len = sizeof(double);
				type = DTYPE_D;
				if (ind) buf = (char *)&HUGE_D;
				else  {double *dptr = (double *)buf;	*dptr = (((quadw *)buf)->l1 * pow(2e0, 32e0)
				+ ((quadw *)buf)->l0) / pow(10e0, (double)scale);}
			}
			else		{type = DTYPE_Q; if (ind) buf = (char *)&HUGE_Q;}
			break;
		case SQL_TYPE_TINYINT :
			if (scale) {
				len = sizeof(float);
				type = DTYPE_F;
				if (ind) buf = (char *)&HUGE_F;
				else {float *fptr = (float *)buf; *fptr = (float)(*(char *)buf / pow(10e0, (double)scale));}
			}
			else		{type = DTYPE_B; if (ind) buf = (char *)&HUGE_B;}
			break;
		case SQL_TYPE_SEGMENT_ID :
			if (len == 8) {
			char stmt[71];
			struct descriptor dstmt = {0,DTYPE_T,CLASS_S,0};
			char *pstring;
			int status = 1, jj=0;
                                dstmt.pointer = stmt;
				if (rblob && ind) {		/*NULL read for this row*/
					if (rows <= 1) TdiPutIdent(dst, &EMPTY_XD);
					continue;
				}
				strcpy(stmt, "SELECT ");
				strncat(stmt, pda->SQLNAME, pda->SQLNAME_LEN);
				strcat(stmt, " WHERE CURRENT OF STREAM_DB");
				dstmt.length = (unsigned short)strlen(stmt);
	if (SQL_RELEASE == 0) {
		if (status & 1) status = TDISQL_LINK("SQL_CLOSE", &SQL_CLOSE);
		if (status & 1) status = TDISQL_LINK("SQL_DESCRIBE_PARM", &SQL_DESCRIBE_PARM);
		if (status & 1) status = TDISQL_LINK("SQL_DESCRIBE_SELECT", &SQL_DESCRIBE_SELECT);
		if (status & 1) status = TDISQL_LINK("SQL_EXECUTE", &SQL_EXECUTE);
		if (status & 1) status = TDISQL_LINK("SQL_FETCH", &SQL_FETCH);
		if (status & 1) status = TDISQL_LINK("SQL_LIST_READ", &SQL_LIST_READ);
		if (status & 1) status = TDISQL_LINK("SQL_LIST_WRITE", &SQL_LIST_WRITE);
		if (status & 1) status = TDISQL_LINK("SQL_OPEN_CA", &SQL_OPEN_CA);
		if (status & 1) status = TDISQL_LINK("SQL_PREPARE", &SQL_PREPARE);
		if (status & 1) status = TDISQL_LINK("SQL_RELEASE", &SQL_RELEASE);
		if (!(status & 1)) return 0;
	}
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
				int lll = list_ca.SQLERRD[1], nnn = list_ca.SQLERRD[3], k;
				struct descriptor_xd ud = EMPTY_XD;
				DESCRIPTOR_A(uarr, 1, 0, 0, 0);
                                unsigned char dtype = (unsigned char)DTYPE_T;
                                        uarr.arsize = nnn;
					status = MdsGet1DxA((struct descriptor_a *)&uarr, (unsigned short *)&lll, 
                                               &dtype, &ud);
					if (!(status & 1)) goto err1;
					pstring = ud.pointer->pointer;
					list_sele.SQLVARS[0].SQLDATA = malloc(lll+3);
					for (jj = 0; jj < nnn; ++jj, pstring+=lll) {
						status = SQL_FETCH(&code, &list_cu, &list_sele);
						if (!(status & 1)) break;
						k = *(unsigned short *)list_sele.SQLVARS[0].SQLDATA;
						memcpy(pstring, (char *)list_sele.SQLVARS[0].SQLDATA+2, k);
						if (k < lll) memset(pstring+k, ' ', lll-k);
					}
					free(list_sele.SQLVARS[0].SQLDATA);
					if (status & 1)
					if (rows <= 1) {
						status = TdiPutIdent(dst, &ud);		/*first good seg*/
					}
					else {							/*accumulate*/
						status = TdiData(dst, &tmp MDS_END_ARG);
						if (status & 1) status = TdiVector(&tmp, &ud, &tmp MDS_END_ARG);
						if (status & 1) status = TdiPutIdent(dst, &tmp);
					}
					MdsFree1Dx(&ud, NULL);
				}
				else {
				static DESCRIPTOR(dins, "INSERT INTO CURSOR LIST_CU VALUES (?)");
					status = SQL_PREPARE(&code, &dins, &list2_id);
					list_mark.SQLN = 1;
					if (status & 1) status = SQL_DESCRIBE_PARM(&code, &list2_id, &list_mark);
					list_mark.SQLVARS[0].SQLTYPE = SQL_TYPE_CHAR;
					if (status & 1) status = TdiData(dst, &tmp MDS_END_ARG);
					if (status & 1) status = TdiText(&tmp, &tmp MDS_END_ARG);
					if (status & 1) {
					struct descriptor_a *parr = (struct descriptor_a *)tmp.pointer;
						list_mark.SQLVARS[0].SQLDATA = parr->pointer;
						list_mark.SQLVARS[0].SQLLEN = parr->length;
						switch (parr->class) {
						case CLASS_S : case CLASS_D : jj = 1; break;
						case CLASS_A :
							jj = (int)parr->arsize/(int)parr->length;
							break;
						default : status = TdiINVCLADSC;
						}
						for (; --jj >= 0 && status & 1;) {
							status = SQL_EXECUTE(&code, &list2_id, &list_mark);
							list_mark.SQLVARS[0].SQLDATA += list_mark.SQLVARS[0].SQLLEN;
						}
					}
					list_mark.SQLVARS[0].SQLDATA = 0;
					list_mark.SQLD = 0;
					list_mark.SQLN = 0;
					SQL_RELEASE(&code, &list2_id); list2_id = 0;
				}
err1:				list_sele.SQLVARS[0].SQLDATA = 0;
				SQL_CLOSE(&code, &list_cu); code = 0;
err3:				SQL_RELEASE(&code, &list_id); list_id = 0;
				if (!(status & 1)) {
					printf("%5d max %5d seg %9d total",
					list_ca.SQLERRD[1],
					list_ca.SQLERRD[3],
					list_ca.SQLERRD[4]);
					return status;
				}
			}
			else {
				printf("Seg-id len%6d", len);
				return 0;
			}
			continue;
		default :
			status = TdiINVDTYDSC;
			break;
		}
		if (status & 1) {
		struct descriptor keep = {0,0,CLASS_S,0};
                        keep.length = (unsigned short)len;
			keep.dtype = (unsigned char)type;
                        keep.pointer = buf;
			if (rows == 1) {
			struct descriptor_xd dummy = {0,DTYPE_DSC,CLASS_XD,0,0};
                                dummy.pointer = &keep;
				status = TdiPutIdent(dst, &dummy);
			}
			else {
				status = TdiGetIdent(dst, &tmp);
				if (status & 1) status = TdiVector(&tmp, &keep, &tmp MDS_END_ARG);
				if (status & 1) status = TdiPutIdent(dst, &tmp);
			}
		}
	}
	MdsFree1Dx(&tmp, NULL);
	StrFree1Dx(&madeup);
	return status;
}
/*********************************************************/
int	TDISQL_GETS(struct SQLDA *mark, ARGLIST *arg) {
int				used = arg->used;
struct descriptor_xd	**argv = (struct descriptor_xd **)arg->v, tmp = EMPTY_XD;
struct SQLVAR			*pda;
int				ind, j, len, scale, status = 1;

	arg->used += mark->SQLD;
	if (arg->used > arg->c) return TdiMISS_ARG;
	for (j = 0; j < mark->SQLD && status & 1; ++j, ++used, ++argv) {
	struct descriptor	src = {0, 0, CLASS_S, 0};
                src.length = (unsigned short)(len = (pda = &mark->SQLVARS[j])->SQLLEN);
                src.pointer = pda->SQLDATA;
		scale = len >> 8;
		ind = 0;
		if (!argv) ind = -1;
		else switch (pda->SQLTYPE & ~1) {
		case SQL_TYPE_ASCIZ :
		case SQL_TYPE_CHAR :
		case SQL_TYPE_VARCHAR :
			if (argv
			&& ((status = TdiData(*argv, &tmp MDS_END_ARG)) & 1)
			&& (tmp.pointer->length)
			&& ((status = TdiText(&tmp, &tmp MDS_END_ARG)) & 1)) {
                                short *sptr = (short *)src.pointer;
				len = tmp.pointer->length;
				if (len > src.length) len = src.length;
				*sptr++ = (unsigned short)len;
                                src.pointer = (char *)sptr;
				_MOVC3(len, tmp.pointer->pointer, src.pointer);
				pda->SQLTYPE = SQL_TYPE_VARCHAR;
			}
			else ind = -1;
			goto fetched;
		case SQL_TYPE_FLOAT :
			if (len == 4)	src.dtype = DTYPE_F;
			else		src.dtype = DTYPE_D;
			break;
		case SQL_TYPE_INTEGER :
			if (!scale)	src.dtype = DTYPE_L;
			else	{src.dtype = DTYPE_D; pda->SQLTYPE = SQL_TYPE_FLOAT; pda->SQLLEN = 8;}
			break;
		case SQL_TYPE_SMALLINT :
			if (!scale)	src.dtype = DTYPE_W;
			else	{src.dtype = DTYPE_F; pda->SQLTYPE = SQL_TYPE_FLOAT; pda->SQLLEN = 4;}
			break;
		case SQL_TYPE_DATE :
			if (argv
			&& ((status = TdiData(*argv, &ddate MDS_END_ARG)) & 1)
			&& ((StrUpcase(&ddate, &ddate)) & 1)
			&& ((LibConvertDateString(&ddate, src.pointer)) & 1)) ;
			else ind = -1;
			goto fetched;
		case SQL_TYPE_QUADWORD : /*** truncation if scaled ***/
			if (!scale)	src.dtype = DTYPE_Q;
			else	{src.dtype = DTYPE_D; pda->SQLTYPE = SQL_TYPE_FLOAT; pda->SQLLEN = 8;}
			break;
		case SQL_TYPE_TINYINT :
			if (!scale)	src.dtype = DTYPE_B;
			else	{src.dtype = DTYPE_F; pda->SQLTYPE = SQL_TYPE_FLOAT; pda->SQLLEN = 4;}
			break;
		default :
			status = TdiINVDTYDSC;
			break;
		}
		status = TdiData(*argv, &tmp MDS_END_ARG);
		ind = -1;
		if (status & 1 && tmp.pointer) switch (tmp.pointer->dtype) {
		case DTYPE_MISSING :
			break;
		case DTYPE_F :
		case DTYPE_D :
			if (*(unsigned short *)tmp.pointer == 0x8000) break;
		/**************************************
		Falls through, except reserved operand.
		**************************************/
		default :
			status = TdiData(&tmp, &src MDS_END_ARG);
			ind = 0;
			break;
		}
fetched:	*pda->SQLIND = (short)ind;
	}
	MdsFree1Dx(&tmp, NULL);
	return status;
}
/*********************************************************/
TdiRefStandard(Tdi1Dsql)
int		rows = 0;
ARGLIST		user_args = {0};
struct descriptor dtext = {0,DTYPE_T,CLASS_D,0};
struct descriptor drows = {sizeof(rows),DTYPE_L,CLASS_S,0};
        drows.pointer = (char *)&rows;
	user_args.c = narg-1;
	user_args.v = &list[1];

	if (SQL_DYNAMIC == 0) status = TDISQL_LINK("SQL_DYNAMIC", &SQL_DYNAMIC);
	if (status & 1) status = TdiData(list[0], &dtext MDS_END_ARG);
	if (status & 1) status = SQL_DYNAMIC(
		TDISQL_GETS,	/*routine to fill markers	*/
		TDISQL_PUTS,	/*routine to store selctions	*/
		&dtext,		/*text string descriptor	*/
		&user_args,	/*value passed to GETS and PUTS	*/
		&rows);		/*output, number of rows	*/
	StrFree1Dx(&dtext);
	if (status & 1) status = MdsCopyDxXd(&drows, out_ptr);
	return status;
}
/*********************************************************/
TdiRefStandard(Tdi1Isql)
ARGLIST		user_args = {0};
int		rows = 0;
struct descriptor dtext = {0,DTYPE_T,CLASS_D,0};
struct descriptor drows = {sizeof(rows),DTYPE_L,CLASS_S,0};
        drows.pointer = (char *)&rows;
	user_args.c = narg-1;
	user_args.v = &list[1];
	if (USERSQL_SET == 0) {
		if (status & 1) status = TDISQL_LINK("SQL_DYNAMIC", &SQL_DYNAMIC);
		if (status & 1) status = TDISQL_LINK("USERSQL_ERRORS", &USERSQL_ERRORS);
		if (status & 1) status = TDISQL_LINK("USERSQL_GETS", &USERSQL_GETS);
		if (status & 1) status = TDISQL_LINK("USERSQL_PUTS", &USERSQL_PUTS);
		if (status & 1) status = TDISQL_LINK("USERSQL_SET", &USERSQL_SET);
	}

#ifdef __VMS
	if (width == 0) 
        {
          static DESCRIPTOR(output_device,	"SYS$OUTPUT");
          LibGetDvi(&DVI$_DEVBUFSIZ, 0, &output_device, &width, 0, 0);
        }
#endif
        width = width > 0 ? width : 132;
	if (status & 1) status = TdiData(list[0], &dtext MDS_END_ARG);
	if (status & 1) {
		USERSQL_SET(gets, printf, printf, printf, width, head);
		status = SQL_DYNAMIC(
			USERSQL_GETS,	/*routine to fill markers	*/
			USERSQL_PUTS,	/*routine to store selections	*/
			&dtext,		/*text string descriptor	*/
			&width,		/*value passed to GETS and PUTS	*/
			&rows);		/*output, number of rows	*/
		if (!(status & 1)) USERSQL_ERRORS();
	}
	StrFree1Dx(&dtext);
	if (status & 1) status = MdsCopyDxXd(&drows, out_ptr);
	return status;
}
/*********************************************************/
TdiRefStandard(Tdi1IsqlSet)
	if (narg >= 1 && list[0])		status = TdiGetLong(list[0], &width);
	if (narg >= 2 && list[1] && status & 1) status = TdiGetLong(list[1], &head);
	return status;
}
