#include <stdio.h>
#include <config.h>
#include "export.h"
static IDL_LONG 	three = 3;
static IDL_LONG 	nitem;		/*the row count*/
static int	nline;		/*character count on row*/
static char	*pline;		/*temporary line*/
#define MAXMSG 1024
static char	hold[MAXMSG];	/*decode/encode holder*/
static int	excess;		/*flag for unchecked argument count*/
static int	date;		/*flag for absolute day*/

typedef struct {
	unsigned int	l0;
	int		l1;
}	quadword;

#ifdef __toupper
#undef __toupper
#endif
#define __toupper(c) (((c) >= 'a' && (c) <= 'z') ? (c) & 0xDF : (c))

#ifdef __VMS
#include <descrip.h>
#include <starlet.h>
#include <math.h>	/*for pow definition*/
extern void FLOAT_TO_IEEE();
extern void IEEE_TO_FLOAT();
extern void CvtConvertFloat();
static float CVTSF(float in)
{
  float ans;
  CvtConvertFloat(&in, DSC$K_DTYPE_FS, &ans, DSC$K_DTYPE_F);
  return ans;
}
static double CVTTD(double in)
{
  double ans;
  CvtConvertFloat(&in, DSC$K_DTYPE_FT, &ans, DSC$K_DTYPE_D);
  return ans;
}
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
#else
#define CVTGD(a) a
#define CVTDG(a) a
#define CVTTD(a) a
#define CVTSF(a) a
#endif

extern int SQL_DYNAMIC();
extern int USERSQL_ERRORS();
extern int USERSQL_SET();
extern void Logout_Sybase();
extern char *GetDBMsgText();
extern int GetDBStatus();

#ifndef WIN32
#include "sybfront.h"
#include "sybdb.h"
#else
#include <windows.h>
#include <sqlfront.h>
#include <sqldb.h>
#define SYBCHAR		SQLCHAR 
#define SYBTEXT		SQLTEXT
#define SYBFLT8		SQLFLT8
#define SYBREAL		SQLFLT4
#define SYBINT4		SQLINT4
#define SYBINT2		SQLINT2 
#define SYBINT1		SQLINT1
#define SYBDATETIME	SQLDATETIME
#define SYBMONEY	SQLMONEY
#endif
#include <string.h>

static int	code = 0, list_id = 0, list2_id;
typedef struct {
	int	used;
	int	c;
	IDL_VPTR	*v;
}	ARGLIST;
static IDL_VPTR	vpcount, vperror, vpstatus, vptext;
static IDL_VARIABLE result = {IDL_TYP_LONG, 0};
static IDL_STRING	EMPTYSTRING = {0,0,0};
static const double	HUGE_D = 1.7e+38;
static const float	HUGE_F = (float)1.7e+38;
static const int	HUGE_L = 0x7fffffff;
static int      quiet;
static const short	HUGE_W = 0x7fff;
/*********************************************************/
static void IDLresize(k, dst)
IDL_LONG	k;
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
		int *in = old, *out = buf;
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
				strncat(hold, GetDBMsgText(), MAXMSG-1);
				len = strlen(hold);
				hold[len] = '\0';
			}
			IDL_StrStore(&vperror->value.str, hold);
		}
	}
	if ((status != 1))
	if (!quiet) {
				strncat(hold, GetDBMsgText(), MAXMSG-1);
				len = strlen(hold);
		IDL_MessageErrno(IDL_M_NAMED_GENERIC, 0, IDL_MSG_RET, hold);
	}
}

/*********************************************************/
static int	mygets(input)
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
int	j;
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
#define MAXPARSE 16384
static char buf[MAXPARSE];
static char *AddDaysToQuery(char *qry)
{
  char *p1, *p2;
  for (p1=buf, p2=qry; *p2; p1++, p2++)
     *p1 = __toupper(*p2);
  *p1 = 0;
  if (strstr(buf, "TODAY") || strstr(buf, "TOMORROW") || strstr(buf, "YESTERDAY")) {
    strcpy(buf, "declare @today datetime; set @today = cast(getdate() as varchar (12));");
    strcat(buf, "declare @tomorrow datetime; set @tomorrow = cast(dateadd(day, 1, getdate()) as varchar (12));");
    strcat(buf, "declare @yesterday datetime; set @yesterday = cast(dateadd(day, -1, getdate()) as varchar (12));");
    strcat(buf, qry);
    return buf;
   }
   else 
     return qry;
}
/*********************************************************/
static int	Puts(dbproc, prows, arg, rblob)
DBPROCESS	*dbproc;
int		*prows;
ARGLIST		*arg;
int		rblob;
{
 int		rows = *prows, used = arg->used;
 IDL_VPTR		*argv = arg->v, tptr, dst;
 void		*buf;
 int		ind, j, len, type=-1;
 IDL_ALLTYPES	temp;
	char	ddate[28];
	int	isnotseg = 1, ncol = dbnumcols(dbproc);

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
		if (rows < -1) {
			if (isnotseg) IDLresize(-(rows+1), dst);/*remove excess*/
			continue;
		}
		temp.str = EMPTYSTRING;
		if (rblob || isnotseg) {IDL_EXCLUDE_EXPR(dst);}
		len = dbdatlen(dbproc, j+1);
		buf = dbdata(dbproc, j+1);
		ind = len == 0 && buf == (void *)NULL || rows < 0;
		type = dbcoltype(dbproc, j+1);
		switch (type) {
		case SYBCHAR :
		case SYBTEXT :
			if (!ind) {
				IDL_StrEnsureLength(&temp.str, len);
                                if (temp.str.s)
                                {
				  temp.str.s[len] = '\0';
				  memcpy(temp.str.s, (char *)buf, len);
                                }
			}
			type = IDL_TYP_STRING;
			break;
		case SYBFLT8 :
			temp.d = ind ? HUGE_D : CVTTD(*(double *)buf);
			type = IDL_TYP_DOUBLE;
			break;
		case SYBREAL :
			temp.f = ind ? HUGE_F : CVTSF(*(float *)buf);
			type = IDL_TYP_FLOAT;
			break;
		case SYBINT4 :
			temp.l = ind ? HUGE_L : *(int *)buf;
			type = IDL_TYP_LONG;
			break;
		case SYBINT2 :
			temp.i = ind ? HUGE_W : *(short *)buf;
			type = IDL_TYP_INT;
			break;
		case SYBINT1 :			/***convert tiny to smallest signed***/
			temp.i = ind ? HUGE_W : (short)(*(char *)buf);
			type = IDL_TYP_INT;
			break;
		case SYBDATETIME :		/***convert date to double***/
		    if (date) 
		    {		/*Julian day 3 million?? is 17-Nov-1858*/
			  int yr, mo, da, hr, mi, se, th, leap;
			  int status = dbconvert(dbproc,type,buf,len,SYBCHAR,(unsigned char*)ddate,sizeof(ddate)-1);

#ifdef VMS
			  quadword big_time;
			  struct dsc$descriptor_s date_dsc = {22, DSC$K_DTYPE_T, DSC$K_CLASS_S, (char *)ddate};
                        	ddate[20] = '.';
                                ddate[3] = ddate[0];
                                ddate[0] = ddate[4];
                                ddate[4] = __toupper(ddate[1]);
                                ddate[1] = ddate[5];
                                ddate[5] = __toupper(ddate[2]);
                                ddate[2] = '-';
                                ddate[6] = '-';
				if (status < 0) {temp.d = HUGE_D; break;}
                        	status = sys$bintim(&date_dsc, &big_time);
	 			type = IDL_TYP_DOUBLE;
                                if(status&1) {
                                  double t1;
                                  double t2;
                                  t1 = (double)big_time.l1 * pow(2e0, 32e0);
                                  t2 = (double)big_time.l0; 
                                  temp.d = (t1+t2)/ 86400e7;
                                }
                                else
                                  temp.d = HUGE_D;
                                break;
#else
			static char *moname = "JanFebMarAprMayJunJulAugSepOctNovDec";
			static int day[] = {0,31,59,90,120,151,181,212,243,273,304,334};
			char	mon[4], *moptr, ampm[3];
				type = IDL_TYP_DOUBLE;
				if (status < 0) {temp.d = HUGE_D; break;}
				ddate[status] = '\0';
				sscanf(ddate, "%3s %2d %4d %2d:%2d:%2d:%3d%2s",
					mon, &da, &yr, &hr, &mi, &se, &th, ampm);
				if (hr == 12) hr = 0;
				if (ampm[0] == 'P') hr += 12;
				moptr = strstr(moname, mon);
				if (moptr) mo = (moptr - moname)/3; else mo = 0;
				leap = yr/4 - yr/100 + yr/400;
				leap += mo >= 2 && yr%4 == 0 && (yr%100 != 0 || yr%400 == 0);
				temp.d = (double)(yr * 365 + day[mo] + da + leap - 678941);
				temp.d += (double)(th + 1000*(se + 60*(mi + 60*hr)))/86400000.;
				break;
#endif
		  }

		  else 
		  {  
		    /***convert to text***/
		    if (!ind) 
		    {
		      int status;
#ifdef WORDS_BIGENDIAN
		      unsigned long hi = *(unsigned long *)buf;
		      *(unsigned long *)buf = *((unsigned long *)buf+1);
		      *((unsigned long *)buf+1) = hi;
#endif
		      status = dbconvert(dbproc,type,buf,len,SYBCHAR,(unsigned char *)ddate,sizeof(ddate)-1);
		      if (status >= 0) {
			ddate[status] = '\0'; 
		      } else strcpy(ddate, "FAILED");
		      IDL_StrStore(&temp.str, ddate);
		    }
		    type = IDL_TYP_STRING;
		    break;
		  }

		case SYBMONEY :			/***convert to text***/
			if (!ind) {
			int status = dbconvert(dbproc,type,buf,len,SYBCHAR,(unsigned char *)ddate,sizeof(ddate)-1);
				if (status >= 0) ddate[status] = '\0'; else strcpy(ddate, "FAILED");
				IDL_StrStore(&temp.str, ddate);
			}
			type = IDL_TYP_STRING;
			break;
		default :
			sprintf(hold, "Select %s of type %d is unsupport, call Ken.",
			dbcolname(dbproc, j+1), type);
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
			case IDL_TYP_LONG :		*((int *)buf	+(rows-1)) = temp.l;	break;
			case IDL_TYP_INT :		*((short *)buf	+(rows-1)) = temp.i;	break;
			case IDL_TYP_STRING :	*((IDL_STRING *)buf	+(rows-1)) = temp.str;	break;
			}
		}
	}
	return 1;
}
/*********************************************************/
#define bufchk \
if (pout >= pout_end) {\
  sprintf(hold, "String buffer overflow %d characters exceeded", MAXPARSE);\
  return 0;\
}
#define bufchk2(len) \
if ((pout+len) >= pout_end) {\
  sprintf(hold, "String buffer overflow %d characters exceeded", MAXPARSE);\
  return 0;\
}

static int	Gets(pin, pout, nmarks, arg)
char		*pin, *pout;
int		*nmarks;
ARGLIST 	*arg;
{
int		used = arg->used;
IDL_VPTR		*argv = arg->v, src0;
IDL_VARIABLE	src;
void		*buf;
int		count = 0;
char		*pout_end = pout+MAXPARSE;

	while (*pin)          {	
        bufchk
        switch (*pin) {	
        default :			/* copy text */
		*pout++ = *pin++;
		break;
	case '\'' :			/* quoted string */
		while (*pout++ = *pin) {bufchk if (*pin++ == '\'') break; }
		break;
	case '"' :			/* quoted string */
		while (*pout++ = *pin) {bufchk if (*pin++ == '"') break; }
		break;
	case '?' : ++pin;		/* parameter marker */
		++*nmarks;
		if (used >= arg->c) {
			sprintf(hold, "Expect >= %d parameters, %d given.", arg->used, arg->c);
			return 0;
		}
		src0 = *(argv+used);
		IDL_ENSURE_SIMPLE(src0);
		/** assumes nonzero array size **/
		if (excess && (src0->flags & IDL_V_ARR)) {
			buf = src0->value.arr->data;
			src.type = src0->type;
			switch (src0->type) {
			case IDL_TYP_BYTE :		src.value.c	= *((UCHAR *)buf+count); break;
			case IDL_TYP_INT :		src.value.i	= *((short *)buf+count); break;
			case IDL_TYP_LONG :		src.value.l	= *((int *)buf+count); break;
			case IDL_TYP_FLOAT :	src.value.f	= *((float *)buf+count); break;
			case IDL_TYP_DOUBLE :	src.value.d	= *((double *)buf+count); break;
			case IDL_TYP_COMPLEX :	src.value.cmp	= *((IDL_COMPLEX *)buf+count); break;
			case IDL_TYP_STRING :	src.value.str	= *((IDL_STRING *)buf+count);
				src.value.str.stype = 0; /**not a copy, don't discard**/
				break;
			}
			if (++count >= src0->value.arr->n_elts) {
				count = 0;
				++used;
			}
		}
		else {
			src = *src0;
			++used;
		}
		switch (src0->type) {
		case IDL_TYP_UNDEF	: {bufchk2(4) strcpy(pout, "NULL"); break;}
		case IDL_TYP_BYTE	: {bufchk2(10) sprintf(pout, "%d", (int)src.value.c); break;}
		case IDL_TYP_INT	: {bufchk2(10) sprintf(pout, "%d", (int)src.value.i); break;}
		case IDL_TYP_LONG	: {bufchk2(10) sprintf(pout, "%d", (int)src.value.l); break;}
		case IDL_TYP_FLOAT	: {bufchk2(10) sprintf(pout, "%14.8g", (double)src.value.f); break;}
		case IDL_TYP_DOUBLE	: {bufchk2(10) sprintf(pout, "%24.17g", (double)src.value.d); break;}
		case IDL_TYP_COMPLEX	: {bufchk2(10) sprintf(pout, "%14.8g", (double)src.value.f); break; /*real part*/}
		case IDL_TYP_STRING	: {bufchk2(strlen(src.value.str.s))  sprintf(pout, "'%s'", src.value.str.s); break;}
		default :
			sprintf(hold, "Bad IDL %d marker type %d.", used, src0->type);
			return 0;
		}
		pout += strlen(pout);
		break;
	}
	}
        
	*pout = '\0';
	if (count) ++used;
	arg->used = used;
	return 1;
}

/*********************************************************/
IDL_VPTR	IDLSQL_SETDBTYPE(argc, argv_in, argk)
{
  result.value.l = 0;
  return &result;
}

extern int Login_Sybase();

/*********************************************************/
IDL_VPTR	IDLSQL_LOGIN(int argc, IDL_VPTR argv[], char *argk)
{
  int status;
  int i;
  if (argc != 3) {
    IDL_MessageErrno(IDL_M_NAMED_GENERIC, 0, IDL_MSG_RET, "There must be exactly 3 arguments to IDLSQL_LOGIN");
  }

  for (i=0; i<3; i++) {
    IDL_ENSURE_STRING(argv[i]);
    IDL_ENSURE_SCALAR(argv[i]);
  }
  status = Login_Sybase(argv[0]->value.str.s, argv[1]->value.str.s, argv[2]->value.str.s);
  result.value.l = status;
  return &result;
}

/*********************************************************/
IDL_VPTR	IDLSQL_LOGOUT(argc, argv_in, argk)
{
  Logout_Sybase();
  result.value.l = 0;
  return &result;
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

#ifdef __VMS
    IEEE_TO_FLOAT(argc, argv_in);
#endif

        hold[0] = '\0';
	user_args.used = 0;
	user_args.c = argc-1;
	user_args.v = &argv[1];
	IDL_KWCleanup(IDL_KW_MARK);
	user_args.c = IDL_KWGetParams(argc, argv_in, argk, kw_list, argv, 1) - 1;
	IDL_ENSURE_STRING(argv[0]);
	IDL_ENSURE_SCALAR(argv[0]);
        if (strchr(argv[0]->value.str.s, ';')) {
          strcpy(hold, "Only one SQL statement allowed per call (no ';'s");
   	  result.value.l = 0;
          set_them(-1, 0); 
          return &result;
        }
	status = SQL_DYNAMIC(
		Gets,	/*routine to fill markers	*/
		Puts,	/*routine to store selctions	*/
                AddDaysToQuery(argv[0]->value.str.s),
		&user_args,	/*value passed to GETS and PUTS	*/
		&rows);		/*output, number of rows	*/
        status = GetDBStatus();
	result.value.l = rows;
	set_them(rows, status);
	IDL_KWCleanup(IDL_KW_CLEAN);
#ifdef __VMS
    FLOAT_TO_IEEE(argc, argv_in);
#endif
	return &result;
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
 static int   head;
 static int    head_present;
 static int   width;
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

#ifdef __VMS
    IEEE_TO_FLOAT(argc, argv_in);
#endif
	IDL_KWCleanup(IDL_KW_MARK);
	IDL_KWGetParams(argc, argv_in, argk, kw_list, argv, 1);
	IDL_ENSURE_STRING(argv[0]);
	IDL_ENSURE_SCALAR(argv[0]);
	if (!head_present) head = 2;
	if (!width_present || width <= 0) width = IDL_FileTermColumns();
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
                AddDaysToQuery(argv[0]->value.str.s),
		(ARGLIST *)&width,/*value passed to GETS and PUTS	*/
		&rows);		/*output, number of rows	*/
	result.value.l = rows;
	if (vptext) IDL_Deltmp(vpline);
	set_them(rows, status);
	IDL_KWCleanup(IDL_KW_CLEAN);
#ifdef __VMS
        FLOAT_TO_IEEE(argc, argv_in);
#endif
	return &result;
}
