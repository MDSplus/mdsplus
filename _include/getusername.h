#ifndef GETUSERNAME_H
#define GETUSERNAME_H
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef _WIN32
#include <io.h>
#else
#include <pwd.h>
#endif
#include <pthread_port.h>

#define GETUSERNAME(user_p)  \
  GETUSERNAME_BEGIN(user_p); \
  GETUSERNAME_END;

#define GETUSERNAME_BEGIN(user_p)                                      \
  {                                                                    \
    static pthread_mutex_t username_mutex = PTHREAD_MUTEX_INITIALIZER; \
    pthread_mutex_lock(&username_mutex);                               \
    if (!user_p)                                                       \
    {                                                                  \
      user_p = _getUserName()

#define GETUSERNAME_END                  \
  }                                      \
  pthread_mutex_unlock(&username_mutex); \
  }
static char *_getUserName()
{
  char *user_p;
#ifdef _WIN32
  static char user[128];
  DWORD bsize = 128;
  user_p = GetUserName(user, &bsize) ? user : "Windows User";
#elif __MWERKS__
  ans.pointer = "Macintosh User";
#else
  static char user[256];
  struct passwd *pwd = getpwuid(geteuid());
  if (pwd)
  {
    strcpy(user, pwd->pw_name);
    user_p = user;
  }
  else
#ifdef __APPLE__
    user_p = "Apple User";
#else
  {
    user_p = getlogin();
    if (user_p && strlen(user_p) > 0)
    {
      strcpy(user, user_p);
      user_p = user;
    }
    else
      user_p = "Linux User";
  }
#endif
#endif
  return user_p;
}

#endif // GETUSERNAME_H
