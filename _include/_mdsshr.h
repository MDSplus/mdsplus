#ifndef __MDSSHR_H
#define __MDSSHR_H
#include <mdsshr.h>
#include <status.h>
#include <mdsmsg.h>

struct sockaddr;

#define MDSSHR_LOAD_LIBROUTINE(var, lib, method, on_error)        \
  do                                                              \
  {                                                               \
    const int status = LibFindImageSymbol_C(#lib, #method, &var); \
    if (STATUS_NOT_OK)                                            \
    {                                                             \
      MDSERR("Failed to load " #lib "->" #method "()");           \
      on_error;                                                   \
    }                                                             \
  } while (0)
#define MDSSHR_LOAD_LIBROUTINE_LOCAL(lib, method, on_error, returns, args) \
  static returns(*method) args = NULL;                                     \
  MDSSHR_LOAD_LIBROUTINE(method, lib, method, on_error)

extern int _LibGetHostAddr(const char *hostname, const char *service, struct sockaddr *sin);
#endif // ifndef __MDSSHR_H