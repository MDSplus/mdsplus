/*	DYNAMIC.C
	A dynamic SQL interface. (Single threaded.)
	EXTERNAL user_gets, user_puts
VMS	status = SQL_DYNAMIC(user_gets, user_puts, %descr(text), args, nrows)
UNIX	status = SQL_DYNAMIC(user_gets, user_puts, text_string, args, nrows)
	If there is an error you must do the ROLLBACK (if allowed).
	Undocumented that MODULE routines return VMS status.

	There must be external routines:
	(1) USER_GETS supplies information for the parameter markers.
		GETS is only called if markers are present.
		Markers are ?'s in the source statement.
	(3) USER_PUTS acts on the results of a selected row.
		It is only called for select statements.
		It is called before with the rowcount = 0,
		and after with rowcount = -(rowcount + 1).
	You are allowed in VAX-Rdb to append a null to CHAR and VARCHAR.

	The basic dynamic memory used for indicators and descriptors
	is not released. It grows to the largest request.
	Numbers and short strings use the block with the indicators.
	Other strings are freed.

	Sun-Sybase version expects a file /home/HOST/USER/.user_info with
	one line of database username and one line of password.
	You are USER on machine HOST--this is your home directory, usually.
	PROTECT THIS FILE FROM WORLD READ.

	Ken Klare, LANL P-4	(c)1991,1992,1993
	Mar 93, Sybase version from Fred Wysocki's work and the manuals.
	Ken Klare, LANL P-4	23-Jul-1993 merged VAX-Rdb and Sun-Sybase
        Josh Stillerman, MIT PSFC  5/99 - Split RDB/SYB code
*/
#pragma extern_model save
#pragma extern_model globalvalue
extern SQL$_NO_TXNOUT;
#pragma extern_model restore

#include <descrip.h>
#include <stdlib.h>
#include <string.h>
#include <str$routines.h>
#include "SQL$INCLUDE.H"

extern int SQL_CLOSE();
extern int SQL_PREPARE();
extern int SQL_RELEASE();
extern int SQL_DESCRIBE_SELECT();
extern int SQL_DESCRIBE_PARM();
extern int SQL_TABLE_READ();
extern int SQL_TABLE_READ_WRITE();
extern int SQL_OPEN_CODE();
extern int SQL_FETCH();
extern int SQL_EXECUTE_CA();

#pragma extern_model save
#pragma extern_model common_block noshr
extern struct rdb$message_vector RDB$MESSAGE_VECTOR;
#pragma extern_model restore

#define GUESS_PARAMS	10

static $DESCRIPTOR(curname, "STREAM_DB");
static int	stmt_id = 0;
static char	*cursor = 0;
static struct block {
	short		small[5];/*storage for short stuff*/
	unsigned short	ind;	/*an indicator*/
}	*pmarkblo = 0, *pseleblo = 0;
static struct SQLDA	*pmark = 0, *psele = 0;
static int mark_guess = GUESS_PARAMS;
static int sele_guess = GUESS_PARAMS;

static struct _free_list { int stmt_id;
			   struct _free_list *next;
                         } *FreeList = 0;
/*----------------------------------------------------------*/
static void AddToFreeList(int *stmt_id)
{
   struct _free_list *n = malloc(sizeof(struct _free_list));
   n->next = FreeList;
   n->stmt_id = *stmt_id;
   FreeList = n;
   *stmt_id = 0;
}

void SQL_FREE_LOCAL()
{
   struct _free_list *n;
   for (n=FreeList; n; n=FreeList)
   {
     int sqlcode;
     FreeList = n->next;
     SQL_RELEASE(&sqlcode,&n->stmt_id);
     free(n);
   }
}
     
/*----------------------------------------------------------*/
static void	dealloc(
struct SQLDA	*pda,
struct block	*pblock) {
struct SQLVAR	*pvar = pda->SQLVARS;
int	j;

	for (j = pda->SQLN; --j >= 0;)
	if (pvar[j].SQLDATA && pvar[j].SQLDATA != (char *)&pblock[j].small) {
		free(pvar[j].SQLDATA);
		pvar[j].SQLDATA = 0;
	}
	return;
}
/*----------------------------------------------------------*/
static void	enalloc(
struct SQLDA	*pda,
struct block	*pblock) {
struct SQLVAR	*pvar = pda->SQLVARS;
int	j, len, numb = pda->SQLD;

	dealloc(pda, pblock);
	for (j = 0; j < numb; ++j) {
		len = pvar[j].SQLLEN;
		switch (pvar[j].SQLTYPE & ~1) {
		case SQL_TYPE_VARCHAR :	len+=3;	break;
		case SQL_TYPE_CHAR :	len++;	break;
		default :			break;
		}
		pvar[j].SQLIND = (short *)&pblock[j].ind;
		*pvar[j].SQLIND = 0;
		pvar[j].SQLDATA = (len > sizeof(pblock->small))
			? malloc(len) : &pblock[j].small;
	}
	return;
}
/*----------------------------------------------------------*/
int	SQL_DYNAMIC(
int			USER_GETS(),	/*routine to fill markers	*/
int			USER_PUTS(),	/*routine to store selctions	*/
struct dsc$descriptor	*ptext,		/*text string address		*/
int			*user_args,	/*value passed to GETS and PUTS	*/
int			*prows)		/*output, number of rows	*/
{
int	status = 1, stat2, sqlcode;
static struct SQLCA sqlca = SQLCA_INIT;
struct rdb$message_vector temp_msg;

	/************************************
	Allocate memory for descriptor areas.
	Increase allocation if it is filled.
	************************************/
	*prows = -1;
	if (cursor) {SQL_CLOSE(&sqlcode, cursor); cursor = 0;}
	RDB$MESSAGE_VECTOR.RDB$LU_NUM_ARGUMENTS = 1;
	RDB$MESSAGE_VECTOR.RDB$LU_STATUS = 1;
	if (stmt_id) {SQL_RELEASE(&sqlcode, &stmt_id); stmt_id = 0;}
	status = SQL_PREPARE(&sqlcode, ptext, &stmt_id);
	if (!(status & 1)) goto err1;
	while (1) {
		if (!psele) {
			stat2 = sizeof(struct SQLDA)+sizeof(struct SQLVAR)*(sele_guess-1);
			psele = malloc(stat2);
			memset(psele, 0, stat2);
			pseleblo = malloc(sizeof(struct block) * sele_guess);
		}
		psele->SQLN = sele_guess;
		psele->SQLD = -1;
		status = SQL_DESCRIBE_SELECT(&sqlcode, &stmt_id, psele);
		if (psele->SQLN >= psele->SQLD) break;
		sele_guess = psele->SQLD;
		dealloc(psele, pseleblo);
		free(pseleblo); free(psele); psele = 0;
	}
	if (!(status & 1)) goto err1;
	/****************************************
	Any markers must be filled by the caller.
	****************************************/
	while (1) {
		if (!pmark) {
			stat2 = sizeof(struct SQLDA)+sizeof(struct SQLVAR)*(mark_guess-1);
			pmark = malloc(stat2);
			memset(pmark, 0, stat2);
			pmarkblo = malloc(sizeof(struct block) * mark_guess);
		}
		pmark->SQLN = mark_guess;
		pmark->SQLD = -1;
		status = SQL_DESCRIBE_PARM(&sqlcode, &stmt_id, pmark);
		if (pmark->SQLN >= pmark->SQLD) break;
		mark_guess = pmark->SQLD;
		dealloc(pmark, pmarkblo);
		free(pmarkblo); free(pmark); pmark = 0;
	}
	if (!(status & 1)) goto err1;
	/*******************************************
	Allocate buffers for markers and indicators.
	Have the user fill them in.
	*******************************************/
	if (pmark->SQLD > 0) {
		enalloc(pmark, pmarkblo);
		status = USER_GETS(pmark, user_args);
		RDB$MESSAGE_VECTOR.RDB$LU_STATUS = status;
		if (!(status & 1)) goto err1;
	}
	/**********************************************
	Allocate buffers for selections and indicators.
	Select fields are returned to the user.
	**********************************************/
	if (psele->SQLD > 0) {
	int	rowcount = 0, rblob = strncmp("sElEcT", ptext->dsc$a_pointer, 6);
		cursor = (char *)&curname;
		if (rblob) status = SQL_TABLE_READ(&sqlcode, &stmt_id, cursor);
		else status = SQL_TABLE_READ_WRITE(&sqlcode, &stmt_id, cursor);
		if (!(status & 1)) goto err1;
		status = SQL_OPEN_CODE(&sqlcode, cursor, pmark);
		if (!(status & 1)) goto err1;
		status = USER_PUTS(psele, &rowcount, user_args, rblob);
		RDB$MESSAGE_VECTOR.RDB$LU_STATUS = status;
		if (!(status & 1)) goto err1;
		enalloc(psele, pseleblo);
		while (status & 1) {
			status = SQL_FETCH(&sqlcode, cursor, psele);
			if (sqlcode == SQL_EOS) {status = 1; break;}
			if (!(status & 1)) goto err2;
			++rowcount;
			status = USER_PUTS(psele, &rowcount, user_args, rblob);
			RDB$MESSAGE_VECTOR.RDB$LU_STATUS = status;
		}
		temp_msg = RDB$MESSAGE_VECTOR;
		stat2 = SQL_CLOSE(&sqlcode, cursor);
		cursor = 0;
		if (!(status & 1)) RDB$MESSAGE_VECTOR = temp_msg;
		else status = stat2;
		if (status & 1) {
			*prows = rowcount;
			rowcount = -(rowcount + 1);
			status = USER_PUTS(psele, &rowcount, user_args, rblob);
			RDB$MESSAGE_VECTOR.RDB$LU_STATUS = status;
		}
err2:		dealloc(psele, pseleblo);
	}
	else {
		static struct dsc$descriptor uptext = {0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0};
		static $DESCRIPTOR(declare_transation,"DECLARE TRANSACTION");
                static $DESCRIPTOR(set_transaction,"SET TRANSACTION");
                static $DESCRIPTOR(update,"UPDATE");
                int index;
                int sindex;
		status = SQL_EXECUTE_CA(&sqlca, &stmt_id, pmark);
                str$upcase(&uptext,ptext);
                if (str$find_first_substring(&uptext,&index,&sindex,&declare_transation))
                  AddToFreeList(&stmt_id);
                if (str$find_first_substring(&uptext,&index,&sindex,&update))
                  AddToFreeList(&stmt_id);
		*prows = sqlca.SQLERRD[2];
	}
err1:	temp_msg = RDB$MESSAGE_VECTOR;
	if (pmark) dealloc(pmark, pmarkblo);
	if (cursor) {SQL_CLOSE(&sqlcode, cursor); cursor = 0;}
	if (stmt_id) {SQL_RELEASE(&sqlcode, &stmt_id); stmt_id = 0;}
	if (!(status & 1)) RDB$MESSAGE_VECTOR = temp_msg;
	if (RDB$MESSAGE_VECTOR.RDB$LU_STATUS == SQL$_NO_TXNOUT)
		RDB$MESSAGE_VECTOR.RDB$LU_STATUS = 1;
	return (RDB$MESSAGE_VECTOR.RDB$LU_STATUS);
}
