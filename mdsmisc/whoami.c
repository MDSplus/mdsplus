#include <config.h>
#include <mdsdescrip.h>
#include <config.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef WIN32
#include <io.h>
#include <windows.h>
#else
#include <pwd.h>
#endif
#define LOAD_GETUSERNAME
#include <pthread_port.h>

EXPORT struct descriptor *whoami()
{
  static struct descriptor ans = { 0 , DTYPE_T, CLASS_S, 0 };
  GETUSERNAME_BEGIN(ans.pointer);
  ans.length  = (unsigned short)strlen(ans.pointer);
  GETUSERNAME_END;
  return &ans;
}
