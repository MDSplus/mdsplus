/*  CMS REPLACEMENT HISTORY, Element T.C */
/*  *3    16-OCT-1995 13:31:44 TWF "Update from msdos" */
/*  *2     5-JAN-1995 14:07:43 TWF "new definitions" */
/*  *1    28-NOV-1994 15:30:39 TWF "Test program for mdstcpip" */
/*  CMS REPLACEMENT HISTORY, Element T.C */
#include <ipdesc.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char **argv)
{
  int status;
  struct descrip ans;
  float val = 9876;
  struct descrip vald = { DTYPE_FLOAT, 0, {0}, 0, 0 };
  long sock = ConnectToMds((argc > 1) ? argv[1] : "lost.pfc.mit.edu:9000");
  if (sock != -1) {
    printf("status from MdsOpen = %d\n", MdsOpen(sock, "main", -1));
    ans.ptr = 0;
    if (MdsValue(sock, "f_float(member)", &ans, 0) & 1) {
      printf("%g\n", *(float *)ans.ptr);
      val = *(float *)ans.ptr;
      val = val + (float)1.;
    } else
      printf("%s\n", (char *)ans.ptr);
    if (ans.ptr) {
      free(ans.ptr);
      ans.ptr = 0;
    }
    vald.ptr = (void *)&val;
    status = MdsPut(sock, "member", "$", &vald, 0);
    if (!(status & 1))
      printf("Error during put %d\n", status);
    if (MdsValue(sock, "42.0", &ans, 0) & 1)
      printf("%g\n", *(float *)ans.ptr);
    else
      printf("%s\n", (char *)ans.ptr);
    if (ans.ptr)
      free(ans.ptr);
  }
  return 1;
}


