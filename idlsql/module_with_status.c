/*  CMS REPLACEMENT HISTORY, Element MODULE_WITH_STATUS.C */
/*  *3     5-MAR-1996 10:01:52 TWF "Change psect of rdbmessage vector" */
/*  *2    13-OCT-1993 10:55:02 TWF "Add module_with_status for AXP" */
/*  *1    13-OCT-1993 10:24:35 TWF "AXP equivalent of MODULE.SQLMOD for use with module_with_status.c to return correct stats" */
/*  CMS REPLACEMENT HISTORY, Element MODULE_WITH_STATUS.C */
#include <descrip.h>
#include <stdarg.h>
#include <varargs.h>
#include <lib$routines.h>

#include "SQL$INCLUDE.H"
#pragma extern_model save
#pragma extern_model common_block noshr
extern struct rdb$message_vector RDB$MESSAGE_VECTOR;
#pragma extern_model restore

#define define_call(name) \
void SQL__##name();\
unsigned int SQL_##name(va_alist)\
va_dcl\
{\
  int i;\
  int arglist[256];\
  va_list ap;\
  va_start(ap);\
  va_count(arglist[0]);\
  for (i=0;i<arglist[0];i++)\
    arglist[i+1] = va_arg(ap, int);\
  lib$callg(arglist,SQL__##name);\
  return RDB$MESSAGE_VECTOR.RDB$LU_STATUS;\
}
  
define_call(PREPARE)
define_call(DESCRIBE_SELECT)
define_call(DESCRIBE_PARM)
define_call(EXECUTE)
define_call(IMMEDIATE)
define_call(RELEASE)
define_call(COMMIT)
define_call(ROLLBACK)
define_call(FINISH)
define_call(TABLE_READ)
define_call(TABLE_READ_WRITE)
define_call(TABLE_WRITE)
define_call(OPEN_CODE)
define_call(FETCH)
define_call(CLOSE)
define_call(LIST_READ)
define_call(LIST_WRITE)
define_call(EXECUTE_CA)
define_call(IMMEDIATE_CA)
define_call(OPEN_CA)
define_call(FETCH_CA)
