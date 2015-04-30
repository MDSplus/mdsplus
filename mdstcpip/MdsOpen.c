#include <STATICdef.h>
#include "mdsip_connections.h"
#include <ipdesc.h>
#include <stdlib.h>


///
/// Opens a MDSplus tree parsefile identified by tree name and shot number.
/// The actual implementation relies on the MdsValue() of the TDI expression 
/// "TreeOpen($1,$2) with name and shot arguments. If 
/// 
/// \param id the id of the connection to use
/// \param tree the tree name to be opened
/// \param shot the shot id
/// \return status if evaluation was succesfull this is the exit status 
/// of the TreeOpen command converted to int, the status of MdsValue() is returned 
/// otherwise.
///

int MdsOpen(int id, char *tree, int shot)
{
  struct descrip treearg;
  struct descrip shotarg;
  struct descrip ansarg;
  STATIC_CONSTANT char *expression = "TreeOpen($,$)";
  int status =
      MdsValue(id, expression, MakeDescrip((struct descrip *)&treearg, DTYPE_CSTRING, 0, 0, tree),
	       MakeDescrip((struct descrip *)&shotarg, DTYPE_LONG, 0, 0, &shot),
	       (struct descrip *)&ansarg, (struct descrip *)NULL);
  if ((status & 1) && (ansarg.dtype == DTYPE_LONG))
    status = *(int *)ansarg.ptr;
  if (ansarg.ptr)
    free(ansarg.ptr);
  return status;
}
