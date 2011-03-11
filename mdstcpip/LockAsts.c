#include <config.h>
#include "mdsip_connections.h"

static int ast_mutex_initialized = 0;
static pthread_mutex_t ast_mutex;

void LockAsts() {
  if(!ast_mutex_initialized) {
    ast_mutex_initialized = 1;
    pthread_mutex_init(&ast_mutex, 0);
  }
  pthread_mutex_lock(&ast_mutex);
}

void UnlockAsts() {
  if(!ast_mutex_initialized) {
    ast_mutex_initialized = 1;
#ifndef HAVE_WINDOWS_H
    pthread_mutex_init(&ast_mutex, 0);
#endif
  }
#ifndef HAVE_WINDOWS_H
  pthread_mutex_unlock(&ast_mutex);
#endif
}
