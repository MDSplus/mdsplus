#include <Xm/Xm.h>

XmString GetNciString(int nid)
{
    char *cstring = "GETNCI is not yet implemented";
    XmString ans = XmStringCreateLtoR(cstring, XmSTRING_DEFAULT_CHARSET);
    XtFree (cstring);
    return ans;
}
