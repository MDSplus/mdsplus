/*  CMS REPLACEMENT HISTORY, Element T.C */
/*  *3    16-OCT-1995 13:31:44 TWF "Update from msdos" */
/*  *2     5-JAN-1995 14:07:43 TWF "new definitions" */
/*  *1    28-NOV-1994 15:30:39 TWF "Test program for mdstcpip" */
/*  CMS REPLACEMENT HISTORY, Element T.C */
#include <ipdesc.h>
#include <stdio.h>
#ifdef __MSDOS__
#pragma argsused
int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpszCmdLine, int cmdShow)
{
  struct descrip ans;
  SOCKET sock;
  _InitEasyWin();
  sock = ConnectToMds("cmoda.pfc.mit.edu");
  if (sock != INVALID_SOCKET)
  {
    printf("status from MdsOpen = %d\n",MdsOpen(sock,"CMOD",950609015));
    if (MdsValue(sock,"minval(\\magnetics::ip)",&ans, EndOfArgs) & 1)
      printf("%g\n",*(float *)ans.ptr);
    else
      printf("%s\n",ans.ptr);
    if (MdsValue(sock,"42.0",&ans,EndOfArgs) & 1)
      printf("%g\n",*(float *)ans.ptr);
    else
      printf("%s\n",ans.ptr);
  }
  return 0;
}
#else
int main()
{
  struct descrip ans;
  long sock = ConnectToMds("cmoda.pfc.mit.edu");
  if (sock != -1)
  {
    printf("status from MdsOpen = %d\n",MdsOpen(sock,"CMOD",0));
    if (MdsValue(sock,"minval(\\magnetics::ip)",&ans,0) & 1)
      printf("%g\n",*(float *)ans.ptr);
    else
      printf("%s\n",ans.ptr);
    if (MdsValue(sock,"42.0",&ans,0) & 1)
      printf("%g\n",*(float *)ans.ptr);
    else
      printf("%s\n",ans.ptr);
  }
  return 1;
}
#endif
