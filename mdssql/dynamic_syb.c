/* define  RETRY_CONNECTS to have the code retry connects on login when
   there is a failure */
/***********************************************************************/
#define MAXPARSE 16384
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#ifndef WIN32
#include <sybfront.h>
#include <sybdb.h>
#else
#include <windows.h>
#include <sqlfront.h>
#include <sqldb.h>
#define dbloginfree dbfreelogin
#endif
static LOGINREC		*loginrec = 0;
static DBPROCESS	*dbproc = 0;

/*------------------------------FATAL ERROR----------------------------------*/
static void Fatal(msg, arg)
char *msg;
char *arg;
{
	fprintf(stderr, msg, arg);
	exit(ERREXIT);
}

#define MAXMSG 1024
static int DBSTATUS;
static char DBMSGTEXT[MAXMSG];

static void strcatn(char *dst, const char *src, int max)
{
  int dstlen = strlen(dst);
  int srclen  = strlen(src);
  if ((dstlen+srclen) < (max-1))
    strcat(dst, src);
  else
    strncpy(&dst[dstlen], src, max-srclen-1);
}

/*
   Prototypes for Err_Handler and Msg_Handler slightly different between
   SQLSERVER and SYBASE
*/
#ifdef WIN32
#define cnst const
#else
#define cnst
#define DBUSMALLINT int
#endif

/*------------------------------ERROR HANDLER--------------------------------*/

/*------------------------------ERROR HANDLER--------------------------------*/
static int Err_Handler(DBPROCESS *dbproc, int severity, int dberr, int oserr, 
                                           cnst char *dberrstr, cnst char *oserrstr)
{
        /* if we have run out of licences then return cancel
           so we can wait and try again */
        if (DBSTATUS == 18460)  {
          return INT_CANCEL;
        }
        if (!dbproc) return INT_CANCEL;
        if (DBDEAD(dbproc)) return INT_CANCEL;
#ifdef WIN32
        if (dberr != SQLEPWD  ) {
#else
        if (dberr != SYBERDNR  ) {
#endif
          strcatn(DBMSGTEXT, dberrstr, MAXMSG);
          strcatn(DBMSGTEXT, "\n", MAXMSG);
          if (oserr != DBNOERR) {
            strcat(DBMSGTEXT, oserrstr); 
            strcat(DBMSGTEXT, "\n");
          }
        }
        return INT_CANCEL;
}


/*------------------------------MESSAGE HANDLER------------------------------*/
static int Msg_Handler(DBPROCESS *dbproc, DBINT msgno, int msgstate, int severity, 
					   cnst char *msgtext, cnst char *servername, cnst char *procname, 
					   DBUSMALLINT line)
{
        char msg[512];
        DBSTATUS = 1;
	if (msgno == 5701) return 0;	/*just a USE DATABASE notice*/
	if (severity) {
		sprintf(msg, "\nMsg %ld, Level %d, State %d\n",
			msgno, severity, msgstate);
                strcatn(DBMSGTEXT, msg, MAXMSG);
		if (servername)
			if (strlen(servername)) {
				sprintf(msg, "Server '%s', ", servername);
	            strcatn(DBMSGTEXT, msg, MAXMSG);
             }
		if (procname)
			if (strlen(procname)) {
				sprintf(msg, "Procedure '%s', ", procname);
	             strcatn(DBMSGTEXT, msg, MAXMSG);
			}
		if (line) {
			sprintf(msg, "Line %d", line);
	                strcatn(DBMSGTEXT, msg, MAXMSG);
		}
	}
    strcatn(DBMSGTEXT, msgtext, MAXMSG);
    DBSTATUS = msgno;
    return 0;  /* try to continue */
}

int GetDBStatus()
{
  return DBSTATUS;
}

char *GetDBMsgText()
{
  return DBMSGTEXT;
}

/*------------------------------DISCONNECT-----------------------------------*/
void Logout_Sybase() {
  if (loginrec) dbloginfree(loginrec), loginrec = 0;
  if (dbproc) dbexit(), dbproc = 0;
}
/*------------------------------CONNECT--------------------------------------*/
int	Login_Sybase(char *host, char *user, char *pass)
{

#ifdef RETRY_CONNECTS
 int try;
#endif

 if (dbproc)
    Logout_Sybase();

  if (loginrec) {
    dbloginfree(loginrec);
    loginrec = 0;
  }
  if (dbinit() == FAIL) Fatal("Login_Sybase: Can't init DB-library\n");
  dbmsghandle(Msg_Handler);
  dberrhandle(Err_Handler);
  loginrec = dblogin();
#ifdef Win32
  if (user == 0) {
	  DBSETLSECURE(loginrec);
  } else {
#endif
	DBSETLUSER(loginrec, user);
	DBSETLPWD(loginrec, pass);
#ifdef Win32
  }
#endif
  DBSETLAPP(loginrec, "MdsSql");
#ifdef RETRY_CONNECTS
#ifdef  __VMS
extern void decc$sleep();
#define Sleep decc$sleep()
#endif
  for (try = 0; ((dbproc==0) && (try < 10)); try++) {
     DBMSGTEXT[0] = 0;
     dbproc = dbopen(loginrec, host);
     if (!dbproc) {
        Sleep(100);
     }
  }
#else
     DBMSGTEXT[0] = 0;
     dbproc = dbopen(loginrec, host);
#endif
  if (!dbproc) return 0;
  return SUCCEED;
}
/*------------------------------DYNAMIC--------------------------------------*/
int	SQL_DYNAMIC(USER_GETS, USER_PUTS, ptext, user_args, prows)
  int	(*USER_GETS)();	/*routine to fill markers	*/
  int	(*USER_PUTS)();	/*routine to store selctions	*/
  char	*ptext;		/*text string address		*/
  int	*user_args;	/*value passed to GETS and PUTS	*/
  int	*prows; {	/*output, number of rows	*/

        int	status=1, nmarks=0;
        char	parsed[MAXPARSE];

        if (dbproc == 0) {
          DBSTATUS = 0;
          strcpy(DBMSGTEXT, "SET_DATABASE must preceed any DSQL calls");
          return 0;
        }

        DBSTATUS = 1;
        DBMSGTEXT[0] = 0;
	*prows = -1;
	if (strchr(ptext, '?')) {
                parsed[0]='\0';
		status = USER_GETS(ptext, parsed, &nmarks, user_args);
	}
	if (!(status & 1)) return status;
	status = dbcmd(dbproc, nmarks ? parsed : ptext);
	if (status != SUCCEED) goto close;
	status = dbsqlexec(dbproc);
	if (status == SUCCEED) {
	int	rowcount = 0, rblob = strncmp("sElEcT", nmarks ? parsed : ptext, 6);
		while ((status = dbresults(dbproc)) == SUCCEED) {
			status = USER_PUTS(dbproc, &rowcount, user_args, rblob);
			if (status == SUCCEED)
			while ((status = dbnextrow(dbproc)) != NO_MORE_ROWS) {
				++rowcount;
				status = USER_PUTS(dbproc, &rowcount, user_args, rblob);
				if (!status) goto close;
			}
		}
		if (status == NO_MORE_ROWS || status == NO_MORE_RESULTS) {
			*prows = rowcount;
			rowcount = -(rowcount + 1);
			status = USER_PUTS(dbproc, &rowcount, user_args, rblob);
		}
		dbcancel(dbproc);
	}
close:
	DBSTATUS = status;
	return status;
}

DBINT   (*USERSQL_dbconvert)() = 0;
int   (*USERSQL_dbnumcols)() = 0;
char *   (*USERSQL_dbcolname)() = 0;
DBINT   (*USERSQL_dbdatlen)() = 0;
BYTE *   (*USERSQL_dbdata)() = 0;
int   (*USERSQL_dbcoltype)() = 0;

int DBLIB_Initialize() {
  USERSQL_dbconvert = &dbconvert;
  USERSQL_dbnumcols = &dbnumcols;
  USERSQL_dbcolname = &dbcolname;
  USERSQL_dbdatlen = &dbdatlen;
  USERSQL_dbdata = &dbdata;
  USERSQL_dbcoltype = &dbcoltype;
}
