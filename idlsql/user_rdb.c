#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define gap 2			/*space between items*/
#include <math.h>
#include <descrip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <starlet.h>
#include <str$routines.h>
#include <lib$routines.h>
#include "SQL$INCLUDE.H"
extern struct rdb$message_vector RDB$MESSAGE_VECTOR;
extern int SQL$GET_ERROR_TEXT();
extern int SQL_CLOSE();
extern int SQL_RELEASE();
extern int SQL_PREPARE();
extern int SQL_DESCRIBE_SELECT();
extern int SQL_LIST_READ();
extern int SQL_LIST_WRITE();
extern int SQL_OPEN_CA();
extern int SQL_FETCH();
extern int SQL_DESCRIBE_PARM();
extern int SQL_EXECUTE();
static $DESCRIPTOR(const list_cu, "LIST_CU");
static struct SQLDA list_sele, list_mark;
static struct SQLCA list_ca = SQLCA_INIT;
typedef struct {
	unsigned long	l0;
	long		l1;
} quad;
static int	(*input)(char txt[]);
static int	(*output)(char *form, ...);
static int const k0 = 0;
static int const k1 = 1;
static int	code = 0, list_id = 0, list2_id;

static void	(*flush)();
static void	(*next)();
static int	wide;
static int	head;
/*------------------------------DEFINE----------------------------------*/
void USERSQL_SET(xgets, xprintf, newline, newitem, width, heading)
int	(*xgets)(char *);
int	(*xprintf)(char *, ...);
void	(*newline)(char *);
void	(*newitem)(char *);
int	width;
int	heading;
{
	input = xgets;
	output = xprintf;
	flush = newline;
	next = newitem;
	wide = width;
	head = heading;
}
/*------------------------------GET ERROR TEXT--------------------------*/
/*
*	This is a replacement for SQL$GET_ERROR_TEXT,
*	which does not work, different vectors?
*	Note: *pmsg must be a static (non-dynamic) descriptor.
*/
int SQL_GET_ERR_TEXT(struct dsc$descriptor *pmsg, int *plen) {
	*plen = 0;
	return SQL$GET_ERROR_TEXT(pmsg, plen);
}
/*------------------------------ERROR DISPLAY---------------------------*/
int USERSQL_ERRORS() {
int	status = 1, len;
char	mess[511];
struct dsc$descriptor dmess = {sizeof(mess),DSC$K_DTYPE_T,DSC$K_CLASS_S,0};
/*SYS$PUTMSG(&RDB$MESSAGE_VECTOR, 0, 0, 0);*/
        dmess.dsc$a_pointer = mess;
	status = SQL_GET_ERR_TEXT(&dmess, &len);
	if (len) {printf("%.*s\n", len, dmess.dsc$a_pointer);}
	return status;
}
/*------------------------------OUTPUT DISPLAY--------------------------*/
/*
*	These are the select requests for output.
*	It is also called before the first for a
*	header with *prows==0, and after the last
*	for a comment with *prows==-(row_count+1).
*/
int	USERSQL_PUTS(struct SQLDA *sele, const int *prows, int *pwide, int rblob) {
double		temp;
char		*pc;
void		*buf;
struct SQLVAR	*pda;
int		carry = 0, j, len, pos = 0, scale, status = 1, used, type, width = wide - 4;
char            date_c[24];
struct dsc$descriptor ddate = {sizeof(date_c)-1, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};
	ddate.dsc$a_pointer = date_c;
        date_c[23] = 0;
	if (*prows < 0) return 1;
	if ((head & 3) == 2) {
		if (*prows == 0) return 1;
	}
	else if ((head & 3) == 1) {
		if (*prows > 0) output("%3d ", *prows);
		else output("Row ");
	}
	else if (*prows == 0) return 1;
	/***************************************************
	Creates left-justified text without trailing blanks.
	Creates left-justified numbers with some blanks.
	***************************************************/
	for (j = 0; j < sele->SQLD; ++j) {
		/******************************************
		Compute display space we will use.
		Allow for sign and scale point of integers.
		TINY has 3 digits, SMALL has 5, L has 10,
		QUAD==>D_Float(57 bit) or G_Float, maybe.
		F_Float is 24 bit: -1.234567E+38 at worst.
		G_Float is 53 bit or 9 digits more.
		******************************************/
		pda = &sele->SQLVARS[j];
		len = pda->SQLLEN;
		scale = len >> 8;
		type = pda->SQLTYPE;
		switch (type & ~1) {
		case SQL_TYPE_ASCIZ :
		case SQL_TYPE_VARCHAR :
		case SQL_TYPE_CHAR :	used = len;	break;
		case SQL_TYPE_FLOAT :
			if (len == 8)	used = 22;
			else		used = 13;	break;
		case SQL_TYPE_INTEGER : used = scale ? MAX(scale + 3, 12) : 11; break;
		case SQL_TYPE_SMALLINT :used = scale ? MAX(scale + 3, 7) : 6;	break;
		case SQL_TYPE_DATE :	used = 23;	break;
		case SQL_TYPE_QUADWORD :used = 22;	break;
		case SQL_TYPE_TINYINT : used = scale ? MAX(scale + 3, 5) : 4;	break;
		case SQL_TYPE_SEGMENT_ID :used = 35;	break;
		default :		used = 14;	break;
		}
		if (used < 4) used = 4;
		if ((head & 3) == 2) used += pda->SQLNAME_LEN + 2;
		else if (used < pda->SQLNAME_LEN) used = pda->SQLNAME_LEN;

		if (j > 0 && (head & 4)) output("\t");
		else if (pos > 10 && pos + used > width || carry < 0) {
			flush("\n");
			if ((head & 3) == 1) output(">>> ");
			pos = 0;
			carry = 0;
		}
		else if (carry > 0) output("%*s", carry, "");
		pos += used + gap;
		carry = used + gap;

		if ((head & 3) == 2) {
			output("%.*s: ", pda->SQLNAME_LEN, pda->SQLNAME);
			carry -= (pda->SQLNAME_LEN + 2);
			used -= (pda->SQLNAME_LEN + 2);
		}
		else if (*prows == 0) {
			carry -= pda->SQLNAME_LEN;
			output("%.*s", pda->SQLNAME_LEN, pda->SQLNAME);
			continue;
		}
		/*********
		Data rows.
		*********/
		buf = pda->SQLDATA;
		if (pda->SQLIND && *pda->SQLIND < 0 && (rblob || (type & ~1) != SQL_TYPE_SEGMENT_ID)) {
			used = 4;
			output("NULL");
		}
		else	switch (type & ~1) {
		case SQL_TYPE_ASCIZ :		/* used by GETS processing */
			used = strlen((char *)buf);
			output("%s", (char *)buf);
			break;
		case SQL_TYPE_VARCHAR :
			used = *(unsigned short *)buf;
			output("%.*s", used, (unsigned short *)buf+1);
			break;
		case SQL_TYPE_CHAR :		/*Remove trailing blanks*/
			for (pc = (char *)buf+len; --pc >= (char *)buf;)
				if (*pc != ' ') break;
			used = pc - (char *)buf + 1;
			output("%.*s", used, (char *)buf);
			break;
		case SQL_TYPE_FLOAT :
#ifdef __ALPHA
			if	(len == 8)
                        {
                          char ans[64];
                          sprintf(ans,"%-*g", used, *(double *)buf);
                          output("%s", ans);
                        }
                        else if (len == 4)
                        {
                          char ans[64];
                          sprintf(ans,"%-*g", used, *(float *)buf);
                          output("%s", ans);
                        }
#else
			if	(len == 8) output("%-*g", used, *(double *)buf);
			else if (len == 4) output("%-*g", used, *(float *)buf);
#endif
			else	{used = 9, output("real*%4d", len);}
			break;
		 case SQL_TYPE_INTEGER :
			if (scale) {
				temp = *(long *)buf / pow(10e0, (double)scale);
#ifdef __ALPHA
				output("%-*.*f", used, scale, *(__int64 *)&temp);
#else
				output("%-*.*f", used, scale, temp);
#endif
			}
			else	output("%-*ld", used, *(long *)buf);
			break;
		case SQL_TYPE_SMALLINT :
			if (scale) {
				temp = *(short *)buf / pow(10e0, (double)scale);
#ifdef __ALPHA
				output("%-*.*f", used, scale, *(__int64 *)&temp);
#else
				output("%-*.*f", used, scale, temp);
#endif
			}
			else	output("%-*d", used, *(short *)buf);
			break;
		case SQL_TYPE_DATE :
			if (!(sys$asctim(0, &ddate, (quad *)buf, 0) & 1))
				output("%*s", used, "error");
			else output("%*s", used, ddate.dsc$a_pointer);
			break;
		case SQL_TYPE_QUADWORD : /*** truncation ***/
			temp = (((quad *)buf)->l0 + ((quad *)buf)->l1 * pow(2e0, 32e0)) / pow(10e0,(double)scale);
#ifdef __ALPHA
			output("%-*.*f", used, scale, *(__int64 *)&temp);
#else
			output("%-*.*f", used, scale, temp);
#endif
			break;
		case SQL_TYPE_TINYINT :
			if (scale) {
				temp = *(char *)buf / pow(10e0, (double)scale);
#ifdef __ALPHA
				output("%-*.*f", used, scale, *(__int64 *)&temp);
#else
				output("%-*.*f", used, scale, temp);
#endif
			}
			else	output("%-*d", used, *(char *)buf);
			break;
		case SQL_TYPE_SEGMENT_ID : /* line limit is 1024 in a literal string ???*/
			if (len == 8) {
			char stmt[71], tmp[1024];
			struct dsc$descriptor dstmt = {0,DSC$K_DTYPE_T,DSC$K_CLASS_S,0};
				dstmt.dsc$a_pointer = stmt;
				strcpy(stmt, "SELECT ");
				strncat(stmt, pda->SQLNAME, pda->SQLNAME_LEN);
				strcat(stmt, " WHERE CURRENT OF STREAM_DB");
				dstmt.dsc$w_length = strlen(stmt);
				if (code)	{SQL_CLOSE(&code, &list_cu); code = 0;}
				if (list_id)	{SQL_RELEASE(&code, &list_id); list_id = 0;}
				if (list2_id)	{SQL_RELEASE(&code, &list2_id); list2_id = 0;}
				list_mark.SQLD = 0;
				list_sele.SQLD = 0;
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
				used = MAX(list_sele.SQLVARS[0].SQLLEN, list_ca.SQLERRD[1]);
				if (rblob) {
					list_sele.SQLVARS[0].SQLDATA = malloc(used + 2);
					for (scale = 0; scale < list_ca.SQLERRD[3]; ++scale) {
						status = SQL_FETCH(&code, &list_cu, &list_sele);
						if (!(status & 1)) break;
						used = *(unsigned short *)list_sele.SQLVARS[0].SQLDATA;
						if (scale > 0 || pos - carry + used > width) flush("\n");
						output("%.*s", used, (char *)list_sele.SQLVARS[0].SQLDATA+2);
					}
					free(list_sele.SQLVARS[0].SQLDATA);
				}
				else {
				static $DESCRIPTOR(const dins, "INSERT INTO CURSOR LIST_CU VALUES (?)");
					output("VARBYTE*%d: ", list_sele.SQLVARS[0].SQLLEN);
					status = SQL_PREPARE(&code, &dins, &list2_id);
					list_mark.SQLN = 1;
					if (status & 1) status = SQL_DESCRIBE_PARM(&code, &list2_id, &list_mark);
					list_mark.SQLVARS[0].SQLTYPE = SQL_TYPE_CHAR;
					list_mark.SQLVARS[0].SQLDATA = tmp;
					for (scale = 0; status & 1; ++scale) {
						if (input(tmp) == 0) break;
						list_mark.SQLVARS[0].SQLLEN = strlen(tmp);
						status = SQL_EXECUTE(&code, &list2_id, &list_mark);
					}
					list_mark.SQLVARS[0].SQLDATA = 0;
					list_mark.SQLD = 0;
					list_mark.SQLN = 0;
					SQL_RELEASE(&code, &list2_id); list2_id = 0;
				}
				list_sele.SQLVARS[0].SQLDATA = 0;
				if (!(status & 1)) {
					output("%5d max %5d seg %9d total",
					list_ca.SQLERRD[1],
					list_ca.SQLERRD[3],
					list_ca.SQLERRD[4]);
				}
				SQL_CLOSE(&code, &list_cu); code = 0;
err3:				SQL_RELEASE(&code, &list_id); list_id = 0;
			}
			else {
				output("Seg-id len%6d", len);
				used = 16;
			}
			break;
		default :
			output("type=%3d%6d", type, len);
			used = 14;
			break;
		}
		carry -= used;
	}
	if ((head & 3) || *prows > 0) flush("\n");
	if (head && *prows >=0) next(head & 4 ? "" : "-----------\n");
	return (status);
}
/*------------------------------INPUT REQUEST---------------------------*/
/*
*	These are the parameter markers in an SQL
*	request. They are the question marks.
*/
int	USERSQL_GETS(struct SQLDA *mark, int *pwide) {
 double		temp;
 int		ind, j, len, scale, status = 1, type;
 void		*buf;
 char		string[1024+38];
 struct SQLVAR	*pda;
 struct dsc$descriptor dstring = {0,DSC$K_DTYPE_T,DSC$K_CLASS_S,0};
		dstring.dsc$a_pointer = string;
top:
	for (j = 0; j < mark->SQLD; ++j) {
		pda = &mark->SQLVARS[j];
		output("Enter %.*s ", pda->SQLNAME_LEN, pda->SQLNAME);
		buf = pda->SQLDATA;
		len = pda->SQLLEN;
		scale = len >> 8;
		ind = 0;
		temp = 0;
		type = pda->SQLTYPE;
		switch (type & ~1) {
		case SQL_TYPE_ASCIZ :
		case SQL_TYPE_VARCHAR :
		case SQL_TYPE_CHAR :
			output("Character*%d: ", len);
			if (0 == (*input)(string)) break;
			/*caution, strncpy fills trailing nulls, may omit final null*/
			strncpy((char *)buf, string, len);
			pda->SQLTYPE = SQL_TYPE_ASCIZ;
			ind = 1;
			break;
		case SQL_TYPE_FLOAT :
			output("Float*%d: ", len);
			if (0 == (*input)(string)) break;
			if	(len == 4) ind = sscanf(string, "%f%", (float *)buf);
			else if	(len == 8) ind = sscanf(string, "%lf%", (double *)buf);
			else	ind = -1;
			break;
		case SQL_TYPE_INTEGER :
			output("Integer(%d): ", scale);
			if (0 == (*input)(string)) break;
			ind = sscanf(string, "%lf%", &temp);
			*(long *)buf = temp * pow(10e0, (double)scale);
			break;
		case SQL_TYPE_SMALLINT :
			output("Smallint(%d): ", scale);
			if (0 == (*input)(string)) break;
			ind = sscanf(string, "%lf", &temp);
			*(short *)buf = temp * pow(10e0, (double)scale);
			break;
		case SQL_TYPE_DATE :
			output("dd-mmm-yyyy hh:mm:ss.cc : ");
			if (0 == (*input)(string)) break;
			dstring.dsc$w_length = strlen(string);
			ind = dstring.dsc$w_length
			&& (str$upcase(&dstring, &dstring) & 1)
			&& (lib$convert_date_string(&dstring, (quad *)buf) & 1) ? 1 : -1;
			break;
		case SQL_TYPE_QUADWORD : /*** truncation ***/
			output("Quadword(%d): ", scale);
			if (0 == (*input)(string)) break;
			ind = sscanf(string, "%lf%", (quad *)buf);
			pda->SQLTYPE = SQL_TYPE_FLOAT;
			break;
		case SQL_TYPE_TINYINT :
			output("Tinyint(%d): ", scale);
			if (0 == (*input)(string)) break;
			ind = sscanf(string, "%lf%", &temp);
			*(char *)buf = temp * pow(10e0, (double)scale);
			break;
		case SQL_TYPE_SEGMENT_ID :
			output("%%ISQL SEGMENT_ID uses sElEcT");
		default :
			output("%%ISQL unknown type %d len %d", type, len);
			ind = -1;
			break;
		}
		*pda->SQLIND = ind > 0 ? 0 : -1;
 	}
	status = USERSQL_PUTS(mark, &k0, pwide, 1);
	status = USERSQL_PUTS(mark, &k1, pwide, 1);
	while (1) {
		output("OK[Y/N/Q] (Y)? ");
		if (0 == (*input)(string) || string[0] == 'Q' || string[0] == 'q') {
			return 3;
		}
		if (string[0] == '\0' || string[0] == 'Y' || string[0] == 'y') {
			return status;
		}
		if (string[0] == 'N' || string[0] == 'n') goto top;
	}
}
