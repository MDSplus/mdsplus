/*	SQL$INCLUDE.H
	Definitions for the descriptor areas and codes.

	Ken Klare, LANL P-4	(c)1991
*/
	/*******************
	Values of SQLCODE
	Positive is warning.
	Negative is error.
	*******************/
#define SQL_SUCCESS	0	/*command completed successfully	*/
#define SQL_EOS		100	/*SELECT or cursor got to end of stream */
#define SQL_RDBERR	-1	/*Rdb returned a general error		*/
#define SQL_OUTOFRAN	-304	/*Value out of range for a host variable*/
#define SQL_NULLNOIND	-305	/*Tried store NULL and no indicator	*/
#define SQL_CURNOTOPE	-501	/*Cursor isn't open.			*/
#define SQL_CURALROPE	-502	/*Cursor is already open		*/
#define SQL_UDCURNOPE	-507	/*Cursor in update or delete is not open*/
#define SQL_UDCURNPOS	-508	/*Cursor is not position on a record	*/
#define SQL_NO_DUP	-803	/*Update would duplicate unique index	*/
#define SQL_SELMORVAL	-811	/*Singleton select returned more than 1	*/
#define SQL_ROTXN	-817	/*Attempt to update from read-only txn	*/
#define SQL_DEADLOCK	-913	/*Request failed for resource deadlock	*/
#define SQL_INTEG_FAIL	-1001	/*Constraint failed			*/
#define SQL_NOT_VALID	-1002	/*Valid-if failed			*/
#define SQL_LOCK_CONFLICT -1003	/*NO WAIT failed because resource locked*/
#define SQL_BAD_TXN_STATE -1004	/*Invalid transaction state, started	*/
#define SQL_NO_TXN_HANDLE -1005	/*No transaction outstanding		*/
#define SQL_BAD_VERSION	-1006	/*Base system doesn't support feature	*/
/*----------------------------------------------------------*/
struct SQLCA {
	char	SQLCAID[8];	/*identifier of this block	*/
	int	SQLCABC;	/*byte count of this block	*/
	int	SQLCODE;	/*error code			*/
	struct {
		short	SQLERRML;
		char	SQLERRMC[70];
	}	SQLERRM;	/*error message			*/
	int	SQLERRD[6];	/*[0] unused			*/
				/*[1] longest segment		*/
				/*[2] number of rows		*/
				/*[3] number of segments	*/
				/*[4-5] quadword total bytes	*/
	struct {
		char	SQLWARN0,SQLWARN1,SQLWARN2,SQLWARN3;
		char	SQLWARN4,SQLWARN5,SQLWARN6,SQLWARN7;
	}	SQLWARN;	/*unused DB2 compatibility	*/
	char	SQLEXT[8];	/*unknown extension		*/
};
#define SQLCA_INIT	{"SQLCA   ",128,0,{0,""},{0,0,0,0,0,0},\
		{'\0','\0','\0','\0','\0','\0','\0','\0'},"        "}
struct rdb$message_vector {
	int RDB$LU_NUM_ARGUMENTS;/*Number of arguments in vector*/
	int RDB$LU_STATUS;	/*Returned VMS status		*/
	int RDB$LU_ARGUMENTS[18];/*FAO arguments		*/
};
/*----------------------------------------------------------*/
#define SQL_TYPE_VARCHAR	448 /*short counted string	*/
#define SQL_TYPE_CHAR		452 /*fixed length string	*/
#define SQL_TYPE_FLOAT		480 /*REAL* or REAL*8		*/
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
