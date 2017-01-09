#include <config.h>
#include <mdsdescrip.h>
#include <config.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#if defined(WIN32)
#include <io.h>
#include <windows.h>
#else
#include <pwd.h>
#endif
EXPORT struct descriptor *whoami()
{
  static struct descriptor ans = { 0, DTYPE_T, CLASS_S, 0 };
  if (ans.pointer == 0) {
#ifdef _WIN32
    static char user[128];
    DWORD bsize = 128;
    ans.pointer = GetUserName(user, &bsize) ? user : "Windows User";
#elif __MWERKS__
    ans.pointer = "Macintosh User";
#elif __APPLE__
    struct passwd *pwd;
    pwd = getpwuid(geteuid());
    ans.pointer = pwd->pw_name;
#else
    struct passwd *passStruct = getpwuid(geteuid());
    if (!passStruct)
      ans.pointer = "Linux";
    else
      ans.pointer = passStruct->pw_name;
#endif
    ans.length = (unsigned short)strlen(ans.pointer);
  }
  return &ans;
}
