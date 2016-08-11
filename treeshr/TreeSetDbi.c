/*------------------------------------------------------------------------------

		Name: TreeSetDbi

		Type:   C function

		Author:	Tom Fredian
			MIT Plasma Fusion Center

		Date:   25-SEP-2006

		Purpose: Set the Characteristics of a tree.

------------------------------------------------------------------------------

	Call sequence:

  int TreeSetDbi(DBI_ITM *dbi_itm_ptr)

------------------------------------------------------------------------------
   Copyright (c) 1988
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

+-----------------------------------------------------------------------------*/
#include <config.h>
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <treeshr.h>
#include "treeshrp.h"
#include <dbidef.h>

extern void **TreeCtx();

extern int SetDbiRemote();

int TreeSetDbi(DBI_ITM * dbi_itm_ptr)
{
  return _TreeSetDbi(*TreeCtx(), dbi_itm_ptr);
}

int _TreeSetDbi(void *dbid, DBI_ITM * dbi_itm_ptr)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  int status = TreeNORMAL;
  DBI_ITM *itm_ptr;
/*------------------------------------------------------------------------------

 Executable:
*/
  if (!(IS_OPEN_FOR_EDIT(dblist)))
    return TreeNOEDIT;
  if (dblist->remote)
    return SetDbiRemote(dbid, dbi_itm_ptr);
  for (itm_ptr = dbi_itm_ptr; itm_ptr->code != NciEND_OF_LIST && status & 1; itm_ptr++) {
    switch (itm_ptr->code) {
    case DbiVERSIONS_IN_MODEL:
      dblist->tree_info->header->versions_in_model = (*(unsigned int *)itm_ptr->pointer) != 0;
      dblist->modified = 1;
      break;
    case DbiVERSIONS_IN_PULSE:
      dblist->tree_info->header->versions_in_pulse = (*(unsigned int *)itm_ptr->pointer) != 0;
      dblist->modified = 1;
      break;
    case DbiREADONLY:
      dblist->tree_info->header->readonly = (*(unsigned int *)itm_ptr->pointer) != 0;
      dblist->modified = 1;
      break;
    default:
      status = TreeILLEGAL_ITEM;
      break;
    }
  }
  return status;
}

int TreeSetDbiItm(int code, int value)
{
  DBI_ITM itm[] = { {0, 0, 0, 0}, {0, 0, 0, 0} };
  itm[0].buffer_length = (short)sizeof(int);
  itm[0].code = (short)code;
  itm[0].pointer = (void *)&value;
  return TreeSetDbi(itm);
}
