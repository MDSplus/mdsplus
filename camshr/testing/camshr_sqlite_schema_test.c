#include "testing.h"

#include "../camac_db_sqlite.h"
#include "../common.h"

#include <sqlite3.h>
#include <stdlib.h>

int main()
{
  sqlite3 *db = 0;
  int cts_count = -1;
  int crate_count = -1;

  BEGIN_TESTING(CamShrSqliteSchema);

  TEST1(camac_db_sqlite_open(":memory:", &db) == SUCCESS);
  TEST1(camac_db_sqlite_init(db) == SUCCESS);
  TEST1(camac_db_sqlite_table_count(db, "cts_modules", &cts_count) == SUCCESS);
  TEST1(cts_count == 0);
  TEST1(camac_db_sqlite_table_count(db, "crate_map", &crate_count) == SUCCESS);
  TEST1(crate_count == 0);

  if (db)
    sqlite3_close(db);

  END_TESTING;
  return 0;
}
