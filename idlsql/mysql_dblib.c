#include <config.h>
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif
#ifdef _MYSQL_
#include <mysql.h>
#endif
#include <stdio.h>
static int ResultToggle = 0;
#ifdef _MYSQL_
static MYSQL *MySQL = 0;
static void *MySQL_HANDLE = 0;
static MYSQL_RES *Result = 0;
static MYSQL_ROW Row = 0;
#else
#include <iodbc.h>
#include <isqlext.h>
#include <isql.h>
#ifdef HAVE_WINDOWS_H
#include <sqlfront.h>
#include <sqldb.h>
#else
typedef short SQLSMALLINT;
typedef void *SQLHENV;
typedef int INT;
typedef int DBINT;
typedef void *DBPROCESS;
typedef const char *LPCSTR;
typedef unsigned short DBUSMALLINT;
typedef void *LOGINREC;
#define SUCCEED (int)1
#define SQLAPI
#define SQLCHAR   0x2F 
#define SQLTEXT   0x23
#define SQLFLT8   0x3E
#define SQLFLT4   0x3B
#define SQLINT4   0x38
#define SQLINT2   0x34
#define SQLINT1   0x30
#define SQLDATETIME 0x3D
#define SQL_DESC_COUNT SQL_COLUMN_COUNT
#define SQL_TYPE_TIMESTAMP SQL_TIMESTAMP
#define SQL_DATETIME 42
#define SQL_DESC_CONCISE_TYPE SQL_COLUMN_TYPE
#define SQL_DESC_LABEL SQL_COLUMN_LABEL
#define SQL_NO_DATA SQL_NO_DATA_FOUND
#define NO_MORE_RESULTS 2
#define NO_MORE_ROWS -2
typedef void *(*DBMSGHANDLE_PROC)();
typedef void *(*DBERRHANDLE_PROC)();
#endif
static SQLHENV henv1,hdbc1,hstmt1;
static void *DATA = 0;
static int LAST_IDX = -1;
#endif
static DBMSGHANDLE_PROC MsgProc;

void dbexit()
{
#ifdef _MYSQL_
  if (MySQL)
    mysql_close(MySQL); 
  MySQL = 0;
#endif
}

void dbloginfree(LOGINREC *loginrec)
{
#ifdef _MYSQL_
  if (MySQL) 
    mysql_close(MySQL); 
  MySQL = 0;
#endif
}

DBPROCESS *dbopen(LOGINREC *loginrec, char  *host)
{
#ifdef _MYSQL_
  char *error;
  MySQL = mysql_init(NULL);
  MySQL_HANDLE = mysql_real_connect(MySQL,host,
    /* char *user */0,/* char *password */0,
    /* char *database */0,/* unsigned int port */0,
    /* char *unix_socket */NULL,/* unsigned int client_flag */0); 
  error = mysql_error(MySQL); 
  return (DBPROCESS *)MySQL_HANDLE;
#else
  int status;
  SQLAllocEnv(&henv1);
  /*  status = SQLSetEnvAttr(henv1, SQL_ATTR_ODBC_VERSION, (char *)SQL_OV_ODBC3, 0); */
  SQLAllocConnect(henv1, &hdbc1);
  status = SQLConnect(hdbc1, (unsigned char *)host, (SQLSMALLINT)strlen(host),0,0,0,0);
  SQLAllocStmt(hdbc1, &hstmt1);
  return (void *)status;
#endif

}

LOGINREC *dblogin()
{
  return 0;
}

DBERRHANDLE_PROC  dberrhandle(DBERRHANDLE_PROC handler)
{
  return 0;
}
DBMSGHANDLE_PROC  dbmsghandle(DBMSGHANDLE_PROC handler)
{
  DBMSGHANDLE_PROC oldproc = MsgProc;
  MsgProc = handler;
  return oldproc;
}
LPCSTR  dbinit()
{
  return (LPCSTR)1;
}
int dbdead(DBPROCESS  *dbproc)
{
  return 0;
}
int  dbcancel(DBPROCESS  *dbproc)
{
#ifdef _MYSQL_
  if (Result)
  {
    mysql_free_result(Result);
    Result = 0;
  }
  return 0;
#else
   return SQLCancel(hstmt1);
#endif
}

int  dbnextrow(DBPROCESS  *dbproc)
{
#ifdef _MYSQL_
  if (Result)
  {
    Row = mysql_fetch_row(Result);
    return (Row != NULL) ? SUCCEED : NO_MORE_ROWS;
  }
  return NO_MORE_ROWS;
#else
  int status = SQLFetch(hstmt1);
  switch(status)
  {
    case SQL_SUCCESS:
    case SQL_SUCCESS_WITH_INFO: status = SUCCEED; break;
    case SQL_NO_DATA: status = NO_MORE_ROWS; break;
    default: status = NO_MORE_ROWS; break;
  }
  return status;
#endif
}

int  dbresults(DBPROCESS  *dbproc)
{
  int status = ResultToggle == 1 ? SUCCEED : NO_MORE_RESULTS;
  ResultToggle = 0;
  return status;
}

int  dbsqlexec(DBPROCESS  *dbproc)
{
#ifdef _MYSQL_
  int status = 0;
  Result = mysql_store_result(MySQL); 
  if (mysql_errno(MySQL) == 0)
  {
    status = SUCCEED;
    ResultToggle = 1;
  }
  else
  {
    MsgProc(0,0,0,0,mysql_error(MySQL),"","",0);
  }
  return status;
#else
  return SUCCEED;
#endif
}
int  dbcmd(DBPROCESS  *dbproc,char  *text)
{
#ifdef _MYSQL_
  int status =(mysql_real_query(MySQL,text,strlen(text))==0)?SUCCEED:0;
  if (!status)
    MsgProc(0,0,0,0,mysql_error(MySQL),"","",0);
  return status;
#else
  int status = SQLExecDirect(hstmt1,(unsigned char *)text,(int)strlen(text));
  switch (status)
  {
    case SQL_SUCCESS:
    case SQL_SUCCESS_WITH_INFO: status = SUCCEED; break;
    case SQL_NO_DATA: status = SUCCEED; break;
    default: status = 0; break;
  }
  ResultToggle = (status == SUCCEED);
  LAST_IDX = -1;
  return status;
#endif
}
char  *  dbcolname(DBPROCESS  *dbproc,int idx)
{
#ifdef _MYSQL_
  if (Result)
  {
    MYSQL_FIELD *field = mysql_fetch_field_direct(Result,idx-1);
    return field->name;
  }
  else
    return "";
#else
  static char colname[128];
  SQLSMALLINT length;
  if (SQLColAttributes (hstmt1, (SQLSMALLINT)idx, SQL_DESC_LABEL, colname, sizeof(colname),&length,0) == SQL_SUCCESS)
  {
    return colname;
  }
  else
    return 0;
 #endif
}

DBINT  dbconvert(DBPROCESS *dbproc,int itype, char  *in, DBINT inlen, int otype, char  *out, DBINT outlen)
{
#ifdef _MYSQL_
  return 0;
#else
  if (itype == SQLDATETIME)
  {
    int i;
    for (i=0;i<inlen && i<outlen; i++) out[i] = in[i];
    return i;
  }
  else
    return 0;
#endif
}
int dbcoltype(DBPROCESS *dbproc, int idx)
{
#ifdef _MYSQL_
  int ans = 999;
  if (Result)
  {
    MYSQL_FIELD *field = mysql_fetch_field_direct(Result,idx-1);
    switch (field->type)
    {
    case FIELD_TYPE_TINY: ans = SQLINT1; break;
    case FIELD_TYPE_SHORT: ans = SQLINT2; break;
    case FIELD_TYPE_LONG: ans = SQLINT4; break;
    case FIELD_TYPE_FLOAT: ans = SQLFLT4; break;
    case FIELD_TYPE_DOUBLE: ans = SQLFLT8; break;
    case FIELD_TYPE_TIMESTAMP: ans = SQLDATETIME; break;
    case FIELD_TYPE_VAR_STRING: ans = SQLTEXT; break;
    case FIELD_TYPE_STRING: ans = SQLTEXT; break;
    case FIELD_TYPE_DECIMAL:
    case FIELD_TYPE_LONGLONG:
    case FIELD_TYPE_INT24:
    case FIELD_TYPE_DATE:
    case FIELD_TYPE_TIME:
    case FIELD_TYPE_DATETIME:
    case FIELD_TYPE_YEAR:
    case FIELD_TYPE_NEWDATE:
    case FIELD_TYPE_ENUM:
    case FIELD_TYPE_SET:
    case FIELD_TYPE_TINY_BLOB:
    case FIELD_TYPE_MEDIUM_BLOB:
    case FIELD_TYPE_LONG_BLOB:
    case FIELD_TYPE_BLOB:
    case FIELD_TYPE_NULL:
    default: break;
    }
  }
  return ans;
#else
  int sqltype = 0;
  if (SQLColAttributes (hstmt1, (SQLSMALLINT)idx, SQL_DESC_CONCISE_TYPE, 0, 0,0,&sqltype) == SQL_SUCCESS)
  {
    switch (sqltype)
    {
    case SQL_CHAR: sqltype = SQLINT1; break;
    case SQL_SMALLINT: sqltype = SQLINT2; break;
    case SQL_INTEGER: sqltype = SQLINT4; break;
    case SQL_REAL: sqltype = SQLFLT4; break;
    case SQL_DOUBLE: sqltype = SQLFLT8; break;
    case SQL_DATETIME:
    case SQL_TYPE_TIMESTAMP: sqltype = SQLDATETIME; break;
    case SQL_VARCHAR: sqltype = SQLTEXT; break;
    case -1:
    case -9: sqltype = SQLTEXT; break;
    }
  }
  return sqltype;
#endif
}

char *dbdata(DBPROCESS *dbproc, int idx)
{
#ifdef _MYSQL_
  char *ans = 0;
  static union { char _tiny;
          short _short;
          int _long;
          float _float;
          double _double;
  } converted;
  if (Result && Row && Row[idx-1])
  {
    MYSQL_FIELD *field = mysql_fetch_field_direct(Result,idx-1);
    switch (field->type)
    {
    case FIELD_TYPE_TINY: converted._tiny = (char)atoi(Row[idx-1]); ans = (char *)&converted; break;
    case FIELD_TYPE_SHORT: converted._short = (short)atoi(Row[idx-1]); ans = (char *)&converted; break;
    case FIELD_TYPE_LONG: converted._long = (int)atoi(Row[idx-1]); ans = (char *)&converted; break;
    case FIELD_TYPE_FLOAT: sscanf(Row[idx-1],"%g",&converted._float); ans = (char *)&converted; break;
    case FIELD_TYPE_DOUBLE: sscanf(Row[idx-1],"%lg",&converted._double); ans = (char *)&converted; break;
    case FIELD_TYPE_TIMESTAMP: ans = (char *)Row[idx-1]; break;
    case FIELD_TYPE_VAR_STRING: ans = (char *)Row[idx-1]; break;
    case FIELD_TYPE_STRING: ans = (char *)Row[idx-1]; break;
    case FIELD_TYPE_DECIMAL:
    case FIELD_TYPE_LONGLONG:
    case FIELD_TYPE_INT24:
    case FIELD_TYPE_DATE:
    case FIELD_TYPE_TIME:
    case FIELD_TYPE_DATETIME:
    case FIELD_TYPE_YEAR:
    case FIELD_TYPE_NEWDATE:
    case FIELD_TYPE_ENUM:
    case FIELD_TYPE_SET:
    case FIELD_TYPE_TINY_BLOB:
    case FIELD_TYPE_MEDIUM_BLOB:
    case FIELD_TYPE_LONG_BLOB:
    case FIELD_TYPE_BLOB:
    case FIELD_TYPE_NULL:
    default: break;
    }
  }
  return ans;
#else
  int len;
  if (LAST_IDX != idx)
    len = dbdatlen(dbproc,idx);
  return DATA;
#endif
}

DBINT dbdatlen(DBPROCESS *dbproc, int idx)
{
#ifdef _MYSQL_
  DBINT ans=0;
  if (Result && Row && Row[idx-1])
  {
    MYSQL_FIELD *field = mysql_fetch_field_direct(Result,idx-1);
    switch (field->type)
    {
    case FIELD_TYPE_TINY: ans=1; break;
    case FIELD_TYPE_SHORT: ans=2; break;
    case FIELD_TYPE_LONG: ans=4; break;
    case FIELD_TYPE_FLOAT: ans=4; break;
    case FIELD_TYPE_DOUBLE: ans=8; break;
    case FIELD_TYPE_TIMESTAMP: ans=strlen(Row[idx-1]); break;
    case FIELD_TYPE_VAR_STRING: ans=strlen(Row[idx-1]); break;
    case FIELD_TYPE_STRING: ans=strlen(Row[idx-1]); break;
    case FIELD_TYPE_DECIMAL:
    case FIELD_TYPE_LONGLONG:
    case FIELD_TYPE_INT24:
    case FIELD_TYPE_DATE:
    case FIELD_TYPE_TIME:
    case FIELD_TYPE_DATETIME:
    case FIELD_TYPE_YEAR:
    case FIELD_TYPE_NEWDATE:
    case FIELD_TYPE_ENUM:
    case FIELD_TYPE_SET:
    case FIELD_TYPE_TINY_BLOB:
    case FIELD_TYPE_MEDIUM_BLOB:
    case FIELD_TYPE_LONG_BLOB:
    case FIELD_TYPE_BLOB:
    case FIELD_TYPE_NULL:
    default: break;
    }
  }
  return ans;
#else
  static void *buffer = 0;
  static int buflen = 0;
  int status;
  long len = 0;
  static char cbuffer[128];
  long newlen = 0;
  int coltype;
  LAST_IDX = idx;
  switch (coltype = dbcoltype(dbproc,idx))
  {
    case SQLINT1:     newlen = 1; break;
    case SQLINT2:     newlen = 2; break;
    case SQLINT4:     newlen = 4; break;
    case SQLFLT4:     newlen = 4; break;
    case SQLFLT8:     newlen = 8; break;
    case SQLDATETIME: newlen = 23; break;
    case SQLTEXT:     newlen = 0;
                      status = SQLGetData(hstmt1,(SQLSMALLINT)idx,
                                  (SQLSMALLINT)SQL_C_CHAR,cbuffer,1,&newlen);
                      if (status == SQL_SUCCESS_WITH_INFO)
                        newlen++;
                      break;
  }
  if (newlen > 0)
  {
    if (newlen > sizeof(cbuffer))
    {
      if (newlen != buflen)
      {
        if (buffer)
          free(buffer);
        buffer = malloc(newlen);
        buflen = newlen;
      }
     DATA = buffer;
    }
    else
      DATA = cbuffer;
    status = SQLGetData(hstmt1,(SQLSMALLINT)idx,
       (SQLSMALLINT)((dbcoltype(dbproc,idx) == SQLDATETIME) ? SQL_C_CHAR : SQL_C_DEFAULT),DATA,newlen,&len);
    if ((status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) || len <= 0)
    {
      DATA = 0;
      len = 0;
    }
  }
  return len;
#endif
}

int dbnumcols(DBPROCESS *dbproc)
{
#ifdef _MYSQL_
  return Result ? mysql_num_fields(Result) : 0;
#else
  int ncols = 0;
  if (SQLColAttributes (hstmt1, 0, SQL_DESC_COUNT, 0, 0,0,&ncols) != SQL_SUCCESS)
    ncols = 0;
  return ncols;
#endif
}

DBINT dbcollen(DBPROCESS *dbproc, int idx)
{
#ifdef _MYSQL_
  return mysql_fetch_lengths(Result)[idx-1];
#else
  long newlen = 0;
  int status;
  char cbuffer[1];
  switch (dbcoltype(dbproc,idx))
  {
    case SQLINT1:     newlen = 1; break;
    case SQLINT2:     newlen = 2; break;
    case SQLINT4:     newlen = 4; break;
    case SQLFLT4:     newlen = 4; break;
    case SQLFLT8:     newlen = 8; break;
    case SQLDATETIME: newlen = 23; break;
    case SQLTEXT:     newlen = 0;
                      status = SQLGetData(hstmt1,(SQLSMALLINT)idx,
                                  (SQLSMALLINT)SQL_C_CHAR,cbuffer,1,&newlen);
                      break;
  }
  return newlen;
#endif
}

int  dbsetlname (LOGINREC  *loginrec, char  *whatever, INT what2)
{
  return 0;
}
