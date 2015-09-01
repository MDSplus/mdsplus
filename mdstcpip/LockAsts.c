#include <config.h>
#include <mdsshr.h>
#include "mdsip_connections.h"

static int ast_mutex_initialized = 0;
static pthread_mutex_t ast_mutex;

////////////////////////////////////////////////////////////////////////////////
//  LockAsts  //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void LockAsts()
{
  if (!ast_mutex_initialized) {
    ast_mutex_initialized = 1;
    pthread_mutex_init(&ast_mutex, 0);
  }
  pthread_mutex_lock(&ast_mutex);
}

///
/// Unlocks AST mutex
///
void UnlockAsts()
{
  if (!ast_mutex_initialized) {
    ast_mutex_initialized = 1;
#ifndef _WIN32
    pthread_mutex_init(&ast_mutex, 0);
#endif
  }
#ifndef _WIN32
  pthread_mutex_unlock(&ast_mutex);
#endif
}
