#ifndef CAMAC_DB_SQLITE_H
#define CAMAC_DB_SQLITE_H

#include <mdsplus/mdsconfig.h>

#ifdef CAMSHR_HAVE_SQLITE3
#include <sqlite3.h>
#else
typedef struct sqlite3 sqlite3;
#endif

extern EXPORT int camac_db_sqlite_open(const char *path, sqlite3 **db);
extern EXPORT int camac_db_sqlite_init(sqlite3 *db);
extern EXPORT int camac_db_sqlite_import_legacy(sqlite3 *db, const char *db_dir);
extern EXPORT int camac_db_sqlite_table_count(sqlite3 *db, const char *table,
                                               int *count);

#endif
