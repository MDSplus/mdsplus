#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define gap 2			/*space between items*/
#define const /**/
#include <stdio.h>
#include <string.h>
#ifndef WIN32
#include <sybfront.h>
#include <sybdb.h>
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
#define SYBBIT		SQLBIT
#define SYBBINARY	SQLBINARY
#define SYBIMAGE	SQLIMAGE
#endif
static int	(*input)() = 0;
static int	(*output)() = 0;
static int const k0 = 0;
static int const k1 = 1;
static int	code = 0, list_id = 0, list2_id;

static void	(*flush)();
static void	(*next)();
static int	wide;
static int	head;
/*------------------------------DEFINE----------------------------------*/
void USERSQL_SET(xgets, xprintf, newline, newitem, width, heading)
int	(*xgets)();
int	(*xprintf)();
void	(*newline)();
void	(*newitem)();
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
int SQL_GET_ERR_TEXT(pmsg, plen)
char	*pmsg;
int	*plen;
{
	*plen = 0; /* was done directly to terminal by handler */
	return 1;
}
/*------------------------------ERROR DISPLAY---------------------------*/
int USERSQL_ERRORS() {
int	status = 1, len;
char	mess[511];
	status = SQL_GET_ERR_TEXT(mess, &len);
	if (len) {printf("%.*s\n", len, mess);}
	return status;
}
/*------------------------------OUTPUT DISPLAY--------------------------*/
/*
*	These are the select requests for output.
*	It is also called before the first for a
*	header with *prows==0, and after the last
*	for a comment with *prows==-(row_count+1).
*/
int	USERSQL_PUTS(dbproc, prows, pwide, rblob)
DBPROCESS	*dbproc;
int		*prows;
int		*pwide;
int		rblob;
{
 char		*pc, *pname, dtstr[27];
 void		*buf;
 int		carry = 0, pos = 0, status = 1, width = wide - 4;
 int		j, len, namelen, type, used;
 int		ncols = dbnumcols(dbproc);

	if (*prows < 0) return SUCCEED;
	if ((head & 3) == 2) {
		if (*prows == 0) return SUCCEED;
	}
	else if ((head & 3) == 1) {
		if (*prows > 0) output("%3d ", *prows);
		else output("Row ");
	}
	else if (*prows == 0) return SUCCEED;
	/***************************************************
	Creates left-justified text without trailing blanks.
	Creates left-justified numbers with some blanks.
	***************************************************/
	for (j = 1; j <= ncols; ++j) {
		/******************************************
		Compute display space we will use.
		Allow for sign and scale point of integers.
		TINY has 3 digits, SMALL has 5, L has 10,
		QUAD==>D_Float(57 bit) or G_Float, maybe.
		F_Float is 24 bit: -1.234567E+38 at worst.
		G_Float is 53 bit or 9 digits more.
		******************************************/
		len = dbcollen(dbproc, j);
		type = dbcoltype(dbproc, j);
		pname = dbcolname(dbproc, j);	/*null-terminated*/
		namelen = (pname == NULL) ? 0 : strlen(pname);
		switch (type) {
		case SYBTEXT :
		case SYBCHAR :		used = len;	break;
		case SYBINT4 :		used = 11;	break;
		case SYBINT2 :		used = 6;	break;
		case SYBINT1 :		used = 4;	break;
		case SYBFLT8 :		used = 15;	break; /*##should be 22*/
		case SYBREAL :		used = 13;	break;
		case SYBBIT :		used = 1;	break;
		case SYBDATETIME :	used = 26;	break;
		case SYBBINARY :
		case SYBIMAGE :		continue;
		case SYBMONEY :		used = len;	break;
		default :		used = 14;	break;
		}
		if (used < 4) used = 4;
		if ((head & 3) == 2) used += namelen + 2;
		else if (used < namelen) used = namelen;

		if (j > 1 && (head & 4)) output("\t");
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
			if (namelen > 0) output("%.*s: ", namelen, pname);
			carry -= (namelen + 2);
			used -= (namelen + 2);
		}
		else if (*prows == 0) {
			carry -= namelen;
			if (namelen > 0) output("%.*s", namelen, pname);
			continue;
		}
		/*********
		Data rows.
		*********/
		buf = dbdata(dbproc, j);
		len = dbdatlen(dbproc, j);
		if (len == 0 && !buf
		&& (rblob || type != SYBIMAGE)) {
			used = 4;
			output("NULL");
		}
		else	switch (type) {
		case SYBCHAR :		/*Remove trailing blanks*/
			for (pc = (char *)buf+len; --pc >= (char *)buf;)
				if (*pc != ' ') break;
			used = pc - (char *)buf + 1;
			if (used > 0) output("%.*s", used, (char *)buf);
			break;
		case SYBFLT8 :
			output("%-*g", used, *(double *)buf);
			break;
		case SYBINT4 :
			output("%-*ld", used, *(long *)buf);
			break;
		case SYBINT2 :
			output("%-*d", used, *(short *)buf);
			break;
		case SYBDATETIME :
		case SYBMONEY :
			used = dbconvert(dbproc, type, buf, len,
				SYBCHAR, (unsigned char *)dtstr, sizeof(dtstr)-1);
			if (used <= 0) {
				used = 6;
				strcpy(dtstr, "FAILED");
			}
			output("%.*s", used, dtstr);
			break;
		case SYBINT1 :
		case SYBBIT :
			output("%-*d", used, *(char *)buf);
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
int		USERSQL_GETS(pin, pout, nmarks, pwide)
char		*pin, *pout;
int		*nmarks, *pwide;
{
 int		status=1;
 char		*pi, *po, *pf;
 char		string[1024+38];

top:
	pi = pin;
	po = pout;
	/* find the markers, substitute, and skip them */
	for (; pf = strchr(pi, '?'); ++*nmarks, ++pi) {
		output("%.*s", pf-pi, pi);
		for (; pi < pf;) *po++ = *pi++;
		if (0 == (*input)(string)) strcpy(string, "NULL");
		strcpy(po, string);
		po += strlen(string);
	}
	strcpy(po, pi);	/*get the rest*/
	output("%s\n", pout);
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
