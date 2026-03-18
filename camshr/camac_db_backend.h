#ifndef CAMAC_DB_BACKEND_H
#define CAMAC_DB_BACKEND_H

#include <mdsplus/mdsconfig.h>

extern EXPORT int camac_db_backend_enabled(void);
extern EXPORT int camac_db_backend_map(int dbType);
extern EXPORT int camac_db_backend_commit(int dbType);
extern EXPORT int camac_db_backend_get_size_bytes(const char *FileName);
extern EXPORT int camac_db_backend_expand(int dbType, int numOfEntries);
extern EXPORT int camac_db_backend_contract(int dbType, int numOfEntries);

#endif
