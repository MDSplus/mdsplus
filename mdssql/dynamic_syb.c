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
#ifndef ERREXIT
#define ERREXIT -1
#endif
#else
#define _LPCBYTE_DEFINED
typedef unsigned char BYTE;
typedef BYTE *LPBYTE;
typedef const LPBYTE LPCBYTE;
#include <windows.h>
#include <sqlfront.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#include <sqldb.h>
#pragma GCC diagnostic pop
#define dbloginfree dbfreelogin
#endif
#include <mdsplus/mdsconfig.h>
#include <mdsdescrip.h>
#include <status.h>
static LOGINREC *loginrec = 0;
static DBPROCESS *dbproc = 0;

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
static struct descriptor DBMSGTEXT_DSC = {0,DTYPE_T,CLASS_S,DBMSGTEXT};

static void strcatn(char *dst, const char *src, int max)
{
  int dstlen = strlen(dst);
  int srclen = strlen(src);
  if ((dstlen + srclen) < (max - 1))
    strcat(dst, src);
  else
    strncpy(&dst[dstlen], src, max - srclen - 1);
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

static void SetMsgLen() {
  DBMSGTEXT_DSC.length=strlen(DBMSGTEXT);
}

static int Err_Handler(DBPROCESS * dbproc, int severity __attribute__ ((unused)), int dberr, int oserr,
		       cnst char *dberrstr, cnst char *oserrstr)
{
#ifdef WIN32
  if (dberr != SQLEPWD) {
#else
  if (dberr != SYBERDNR) {
#endif
    strcatn(DBMSGTEXT, dberrstr, MAXMSG);
    strcatn(DBMSGTEXT, "\n", MAXMSG);
    if (oserr != DBNOERR && oserrstr) {
      strcatn(DBMSGTEXT, oserrstr, MAXMSG);
      strcatn(DBMSGTEXT, "\n", MAXMSG);
    }
    SetMsgLen();
  }
  /* if we have run out of licences then return cancel
     so we can wait and try again */
  if (DBSTATUS == 18460) {
    return INT_CANCEL;
  }
  if (!dbproc)
    return INT_CANCEL;
  if (DBDEAD(dbproc))
    return INT_CANCEL;
  return INT_CANCEL;
}

/*------------------------------MESSAGE HANDLER------------------------------*/
  static int Msg_Handler(DBPROCESS * dbproc __attribute__ ((unused)), DBINT msgno, int msgstate, int severity,
		       cnst char *msgtext, cnst char *servername, cnst char *procname,
		       DBUSMALLINT line)
{
  char msg[512];
  DBSTATUS = 1;
  if (msgno == 5701)
    return 0;			/*just a USE DATABASE notice */
  if (severity) {
    if (sizeof(msgno) == 8)
	sprintf(msg,"\nMsg %ld, Level %d, State %d\n", (long)msgno, severity, msgstate);
    else
	sprintf(msg,"\nMsg %d, Level %d, State %d\n", (int)msgno, severity, msgstate);
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
  SetMsgLen();
  DBSTATUS = msgno;
  return 0;			/* try to continue */
}

EXPORT int GetDBStatus()
{
  return DBSTATUS;
}

EXPORT struct descriptor *GetDBMsgText_dsc() {
  return &DBMSGTEXT_DSC;
} 
  
EXPORT char *GetDBMsgText()
{
  return DBMSGTEXT;
}

/*------------------------------DISCONNECT-----------------------------------*/
EXPORT void Logout_Sybase()
{
  if (loginrec)
    dbloginfree(loginrec), loginrec = 0;
  if (dbproc)
    dbexit(), dbproc = 0;
}

/*------------------------------CONNECT--------------------------------------*/
EXPORT int Login_Sybase(char *host, char *user, char *pass)
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
  if (dbinit() == FAIL)
    Fatal("%s","Login_Sybase: Can't init DB-library\n");
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
  /*//  DBSETLPACKET(loginrec, 8192);
     // DBSETOPT(dbproc, DBBUFFER, "0") ;
   */

#ifdef RETRY_CONNECTS
  for (try = 0; ((dbproc == 0) && (try < 10)); try++) {
    DBMSGTEXT[0] = 0;
    SetMsgLen();
    dbproc = dbopen(loginrec, host);
    if (!dbproc) {
      Sleep(100);
    }
  }
#else
  DBMSGTEXT[0] = 0;
  SetMsgLen();
  dbproc = dbopen(loginrec, host);
#endif
  if (!dbproc)
    return 0;
  dbgetpacket(dbproc);
  dbclropt(dbproc, DBBUFFER, "0");

  return SUCCEED;
}

/*------------------------------DYNAMIC--------------------------------------*/
EXPORT int SQL_DYNAMIC(USER_GETS, USER_PUTS, ptext, user_args, prows)
int (*USER_GETS) ();		/*routine to fill markers       */
int (*USER_PUTS) ();		/*routine to store selctions    */
char *ptext;			/*text string address           */
int *user_args;			/*value passed to GETS and PUTS */
int *prows;
{				/*output, number of rows        */

  int status = 1, nmarks = 0;
  char parsed[MAXPARSE];

  if (dbproc == 0) {
    DBSTATUS = 0;
    strcpy(DBMSGTEXT, "SET_DATABASE must preceed any DSQL calls");
    SetMsgLen();
    return 0;
  }

  DBSTATUS = 1;
  DBMSGTEXT[0] = 0;
  SetMsgLen();
  *prows = -1;
  if (strchr(ptext, '?')) {
    parsed[0] = '\0';
    status = USER_GETS(ptext, parsed, &nmarks, user_args);
  }
  if (STATUS_NOT_OK)
    return status;
  status = dbcmd(dbproc, nmarks ? parsed : ptext);
  if (status != SUCCEED)
    goto close;
  status = dbsqlexec(dbproc);
  if (status == SUCCEED) {
    int rowcount = 0, rblob = strncmp("sElEcT", nmarks ? parsed : ptext, 6);
    while ((status = dbresults(dbproc)) == SUCCEED) {
      status = USER_PUTS(dbproc, &rowcount, user_args, rblob);
      if (STATUS_OK)
	while ((status = dbnextrow(dbproc)) != NO_MORE_ROWS) {
	  ++rowcount;
	  status = USER_PUTS(dbproc, &rowcount, user_args, rblob);
	  if (STATUS_NOT_OK)
	    goto close;
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

/*
DBINT   (*USERSQL_dbconvert)() = 0;
int   (*USERSQL_dbnumcols)() = 0;
char *   (*USERSQL_dbcolname)() = 0;
DBINT   (*USERSQL_dbdatlen)() = 0;
BYTE *   (*USERSQL_dbdata)() = 0;
int   (*USERSQL_dbcoltype)() = 0;

void DBLIB_Initialize() {
  USERSQL_dbconvert = &dbconvert;
  USERSQL_dbnumcols = &dbnumcols;
  USERSQL_dbcolname = &dbcolname;
  USERSQL_dbdatlen = &dbdatlen;
  USERSQL_dbdata = &dbdata;
  USERSQL_dbcoltype = &dbcoltype;
}
*/
