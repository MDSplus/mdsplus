
#include <STATICdef.h>
#include "mdsip_connections.h"
#include <stdlib.h>

///
/// Sets the defauld node from which the relative path computation is performed.
/// The TreeSetDefault($) expression is evaluated via MdsValue() and the relative
/// exit status is returned.
/// 
/// \param id of the connection to use
/// \param node the node name of the remote tree to be set as the default node
/// \return id if the expression evaluation was succesfull returns the exit status,
/// the MdsValue() status is returned othewise.
///
int MdsSetDefault(int id, char *node)
{
  struct descrip nodearg;
  struct descrip ansarg;
  STATIC_CONSTANT char *expression = "TreeSetDefault($)";
  int status =
      MdsValue(id, expression, MakeDescrip(&nodearg, DTYPE_CSTRING, 0, 0, node), &ansarg, NULL);
  if ((status & 1) && (ansarg.dtype == DTYPE_LONG))
    status = *(int *)ansarg.ptr;
  if (ansarg.ptr)
    free(ansarg.ptr);
  return status;
}
