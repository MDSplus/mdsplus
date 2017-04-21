#include <Xm/Xm.h>

XmString GetNciString(int nid __attribute__ ((unused)))
{
  char *cstring = "GETNCI is not yet implemented";
  XmString ans = XmStringCreateLtoR(cstring, XmSTRING_DEFAULT_CHARSET);
  return ans;
}
