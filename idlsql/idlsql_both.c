#include <stdio.h>
#include "export.h"
#include <descrip.h>
#include <lib$routines.h>
static $DESCRIPTOR(rdb_image, "IDLSQL_RDB");
static $DESCRIPTOR(syb_image, "IDLSQL_SYB");
static $DESCRIPTOR(dyn_routine, "IDLSQL_DYNAMIC");
static $DESCRIPTOR(int_routine, "IDLSQL_INTERACTIVE");
static $DESCRIPTOR(logout_routine, "IDLSQL_LOGOUT");
static $DESCRIPTOR(login_routine, "IDLSQL_LOGIN");
static IDL_VPTR (*CURRENT_DYNAMIC)() = 0;
static IDL_VPTR (*CURRENT_INTERACTIVE)() = 0;
static IDL_VPTR (*CURRENT_LOGOUT)() = 0;
static IDL_VPTR (*CURRENT_LOGIN)() = 0;


void IDLSQL_SETDBTYPE(int argc, IDL_VPTR argv_in[], char *argk)
{
  static int sybase = 0;
  static int rdb = 0;
  static IDL_KW_PAR kw_list[] = {
	{"RDB",		IDL_TYP_LONG,	1,IDL_KW_ZERO,		0,	IDL_CHARA(rdb)},
	{"SYBASE",	IDL_TYP_LONG,	1,IDL_KW_ZERO,		0,	IDL_CHARA(sybase)},
	{NULL}};

  IDL_VPTR		argv[IDL_MAXPARAMS+1];
	IDL_KWCleanup(IDL_KW_MARK);
	IDL_KWGetParams(argc, argv_in, argk, kw_list, argv, 1);

  if (sybase && rdb) 
    IDL_MessageErrno(IDL_M_NAMED_GENERIC, 0, IDL_MSG_RET, "Can not use both RDB and SYBASE at the same time");

  if (!sybase && !rdb) 
    IDL_MessageErrno(IDL_M_NAMED_GENERIC, 0, IDL_MSG_RET, "Must use at least one of RDB or SYBASE");

  lib$establish(lib$sig_to_ret);
  if (sybase) {
    lib$find_image_symbol(&syb_image, &dyn_routine, &CURRENT_DYNAMIC); 
    lib$find_image_symbol(&syb_image, &int_routine, &CURRENT_INTERACTIVE); 
    lib$find_image_symbol(&syb_image, &logout_routine, &CURRENT_LOGOUT); 
    lib$find_image_symbol(&syb_image, &login_routine, &CURRENT_LOGIN); 
  }
  else if (rdb) {
    lib$find_image_symbol(&rdb_image, &dyn_routine, &CURRENT_DYNAMIC); 
    lib$find_image_symbol(&rdb_image, &int_routine, &CURRENT_INTERACTIVE); 
    lib$find_image_symbol(&rdb_image, &logout_routine, &CURRENT_LOGOUT); 
    lib$find_image_symbol(&rdb_image, &login_routine, &CURRENT_LOGIN); 
  }
}

/*********************************************************/
IDL_VPTR	IDLSQL_DYNAMIC(argc, argv_in, argk)
int	argc;
IDL_VPTR	argv_in[];
char	*argk;
{
  IDL_VPTR status = 0;
  if (CURRENT_DYNAMIC != 0) {
    status = (*CURRENT_DYNAMIC)(argc, argv_in, argk);
  }
  return status;
}
/*********************************************************/
IDL_VPTR	IDLSQL_INTERACTIVE(argc, argv_in, argk)
int	argc;
IDL_VPTR	argv_in[];
char	*argk;
{
  IDL_VPTR status = 0;
  if (CURRENT_INTERACTIVE != 0) {
    status =  (*CURRENT_INTERACTIVE)(argc, argv_in, argk);
  }
  return status;
}

/*********************************************************/
IDL_VPTR	IDLSQL_LOGOUT(argc, argv_in, argk)
int	argc;
IDL_VPTR	argv_in[];
char	*argk;
{
  IDL_VPTR status = 0;
  if (CURRENT_LOGOUT != 0) {
    status =  (*CURRENT_LOGOUT)(argc, argv_in, argk);
  }
  return status;
}
/*********************************************************/
IDL_VPTR	IDLSQL_LOGIN(argc, argv_in, argk)
int	argc;
IDL_VPTR	argv_in[];
char	*argk;
{
  IDL_VPTR status = 0;
  if (CURRENT_LOGOUT != 0) {
    status =  (*CURRENT_LOGIN)(argc, argv_in, argk);
  }
  return status;
}
