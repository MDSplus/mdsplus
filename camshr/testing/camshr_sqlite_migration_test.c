#include "testing.h"

#include "../camac_db_sqlite.h"
#include "../common.h"
#include "../crate.h"
#include "../module.h"

#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void make_path(char *out, size_t out_len, const char *dir, const char *name)
{
  snprintf(out, out_len, "%s/%s", dir, name);
}

static int write_record(const char *path, const char *rec, size_t rec_size)
{
  FILE *fp = fopen(path, "wb");
  if (!fp)
    return ERROR;
  if (fwrite(rec, 1, rec_size, fp) != rec_size)
  {
    fclose(fp);
    return ERROR;
  }
  fclose(fp);
  return SUCCESS;
}

int main()
{
  sqlite3 *db = 0;
  char tmpdir[] = "/tmp/camshr-sqlite-migration-XXXXXX";
  char cts_path[1024];
  char crate_path[1024];
  char sqlite_path[1024];
  char cts_rec[MODULE_ENTRY];
  char crate_rec[CRATE_ENTRY];
  int cts_count = -1;
  int crate_count = -1;
  sqlite3_stmt *stmt = 0;

  BEGIN_TESTING(CamShrSqliteMigration);

  TEST1(mkdtemp(tmpdir) != 0);

  make_path(cts_path, sizeof(cts_path), tmpdir, CTS_DB_FILE);
  make_path(crate_path, sizeof(crate_path), tmpdir, CRATE_DB_FILE);
  make_path(sqlite_path, sizeof(sqlite_path), tmpdir, "camac.db");

  memset(cts_rec, ' ', sizeof(cts_rec));
  snprintf(cts_rec, sizeof(cts_rec), "%-32s %-10s %-40s\n", "ALPHA01",
           "GKA100:N01", "First migration record");
  {
    size_t i;
    for (i = 0; i < sizeof(cts_rec); ++i)
      if (cts_rec[i] == '\0')
        cts_rec[i] = ' ';
  }
  cts_rec[sizeof(cts_rec) - 1] = '\n';

  memset(crate_rec, ' ', sizeof(crate_rec));
  snprintf(crate_rec, sizeof(crate_rec), "%-.6s:%3s:%c:%c:%c\n", "GKA100",
           "001", '2', '1', '1');
  {
    size_t i;
    for (i = 0; i < sizeof(crate_rec); ++i)
      if (crate_rec[i] == '\0')
        crate_rec[i] = ' ';
  }
  crate_rec[sizeof(crate_rec) - 1] = '\n';

  TEST1(write_record(cts_path, cts_rec, sizeof(cts_rec)) == SUCCESS);
  TEST1(write_record(crate_path, crate_rec, sizeof(crate_rec)) == SUCCESS);

  TEST1(camac_db_sqlite_open(sqlite_path, &db) == SUCCESS);
  TEST1(camac_db_sqlite_init(db) == SUCCESS);
  TEST1(camac_db_sqlite_import_legacy(db, tmpdir) == SUCCESS);

  TEST1(camac_db_sqlite_table_count(db, "cts_modules", &cts_count) == SUCCESS);
  TEST1(cts_count == 1);
  TEST1(camac_db_sqlite_table_count(db, "crate_map", &crate_count) == SUCCESS);
  TEST1(crate_count == 1);

  TEST1(sqlite3_prepare_v2(
            db, "SELECT logical_name,crate,slot FROM cts_modules", -1, &stmt,
            0) == SQLITE_OK);
  TEST1(sqlite3_step(stmt) == SQLITE_ROW);
  TEST0(strcmp((const char *)sqlite3_column_text(stmt, 0), "ALPHA01"));
  TEST1(sqlite3_column_int(stmt, 1) == 0);
  TEST1(sqlite3_column_int(stmt, 2) == 1);
  sqlite3_finalize(stmt);
  stmt = 0;

  TEST1(sqlite3_prepare_v2(
            db, "SELECT crate_name,device_num,enhanced,online FROM crate_map",
            -1, &stmt, 0) == SQLITE_OK);
  TEST1(sqlite3_step(stmt) == SQLITE_ROW);
  TEST0(strcmp((const char *)sqlite3_column_text(stmt, 0), "GKA100"));
  TEST1(sqlite3_column_int(stmt, 1) == 1);
  TEST1(sqlite3_column_int(stmt, 2) == 1);
  TEST1(sqlite3_column_int(stmt, 3) == 1);
  sqlite3_finalize(stmt);
  stmt = 0;

  if (db)
    sqlite3_close(db);
  unlink(sqlite_path);
  unlink(cts_path);
  unlink(crate_path);
  rmdir(tmpdir);

  END_TESTING;
  return 0;
}
