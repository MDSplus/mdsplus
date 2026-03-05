#include "camac_db_sqlite.h"

#include "common.h"
#include "crate.h"
#include "module.h"
#include "prototypes.h"

#ifdef CAMSHR_HAVE_SQLITE3

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int exec_sql(sqlite3 *db, const char *sql)
{
  int rc;
  char *errmsg = 0;
  rc = sqlite3_exec(db, sql, 0, 0, &errmsg);
  if (rc != SQLITE_OK)
  {
    if (errmsg)
      sqlite3_free(errmsg);
    return ERROR;
  }
  return SUCCESS;
}

static int import_cts(sqlite3 *db, const char *path)
{
  FILE *fp;
  sqlite3_stmt *stmt = 0;
  int status = SUCCESS;

  fp = fopen(path, "rb");
  if (!fp)
    return SUCCESS;

  if (sqlite3_prepare_v2(
          db,
          "INSERT OR REPLACE INTO cts_modules"
          "(logical_name,adapter,scsi_id,crate,slot,comment) "
          "VALUES(?,?,?,?,?,?)",
          -1, &stmt, 0) != SQLITE_OK)
  {
    fclose(fp);
    return ERROR;
  }

  for (;;)
  {
    struct MODULE rec;
    struct Module_ mod;
    size_t got = fread(&rec, 1, sizeof(rec), fp);
    if (got != sizeof(rec))
      break;
    if (rec.Name[0] == ' ')
      continue;
    memset(&mod, 0, sizeof(mod));
    parse_cts_db(&rec, &mod);
    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);
    sqlite3_bind_text(stmt, 1, mod.name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, mod.adapter);
    sqlite3_bind_int(stmt, 3, mod.id);
    sqlite3_bind_int(stmt, 4, mod.crate);
    sqlite3_bind_int(stmt, 5, mod.slot);
    sqlite3_bind_text(stmt, 6, mod.comment, -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
      status = ERROR;
      break;
    }
  }

  sqlite3_finalize(stmt);
  fclose(fp);
  return status;
}

static int decode_hwy_type(char c)
{
  if (c == '.')
    return TYPE_UNKNOWN;
  if (isdigit((unsigned char)c))
    return c - '0';
  return (unsigned char)c;
}

static int import_crate(sqlite3 *db, const char *path)
{
  FILE *fp;
  sqlite3_stmt *stmt = 0;
  int status = SUCCESS;

  fp = fopen(path, "rb");
  if (!fp)
    return SUCCESS;

  if (sqlite3_prepare_v2(
          db,
          "INSERT OR REPLACE INTO crate_map"
          "(crate_name,device_num,highway_type,enhanced,online) "
          "VALUES(?,?,?,?,?)",
          -1, &stmt, 0) != SQLITE_OK)
  {
    fclose(fp);
    return ERROR;
  }

  for (;;)
  {
    struct CRATE rec;
    struct Crate_ crate;
    size_t got = fread(&rec, 1, sizeof(rec), fp);
    if (got != sizeof(rec))
      break;
    if (rec.Phys_Name.prefix[0] == ' ')
      continue;
    memset(&crate, 0, sizeof(crate));
    parse_crate_db(&rec, &crate);
    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);
    sqlite3_bind_text(stmt, 1, crate.name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, crate.device);
    sqlite3_bind_int(stmt, 3, decode_hwy_type(rec.HwyType));
    sqlite3_bind_int(stmt, 4, crate.enhanced);
    sqlite3_bind_int(stmt, 5, crate.online);
    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
      status = ERROR;
      break;
    }
  }

  sqlite3_finalize(stmt);
  fclose(fp);
  return status;
}

int camac_db_sqlite_open(const char *path, sqlite3 **db)
{
  if (!path || !db)
    return ERROR;
  if (sqlite3_open(path, db) != SQLITE_OK)
    return FILE_ERROR;
  return SUCCESS;
}

int camac_db_sqlite_init(sqlite3 *db)
{
  static const char *schema_sql =
      "CREATE TABLE IF NOT EXISTS cts_modules ("
      " logical_name TEXT PRIMARY KEY COLLATE NOCASE,"
      " adapter INTEGER NOT NULL,"
      " scsi_id INTEGER NOT NULL,"
      " crate INTEGER NOT NULL,"
      " slot INTEGER NOT NULL,"
      " comment TEXT NOT NULL DEFAULT ''"
      ");"
      "CREATE TABLE IF NOT EXISTS crate_map ("
      " crate_name TEXT PRIMARY KEY COLLATE NOCASE,"
      " device_num INTEGER NOT NULL DEFAULT -1,"
      " highway_type INTEGER NOT NULL DEFAULT 0,"
      " enhanced INTEGER NOT NULL DEFAULT 0,"
      " online INTEGER NOT NULL DEFAULT 0"
      ");"
      "CREATE INDEX IF NOT EXISTS idx_cts_phy "
      "ON cts_modules(adapter,scsi_id,crate,slot);";

  if (!db)
    return ERROR;
  if (exec_sql(db, "BEGIN IMMEDIATE TRANSACTION;") != SUCCESS)
    return ERROR;
  if (exec_sql(db, schema_sql) != SUCCESS)
  {
    exec_sql(db, "ROLLBACK;");
    return ERROR;
  }
  if (exec_sql(db, "COMMIT;") != SUCCESS)
    return ERROR;
  return SUCCESS;
}

int camac_db_sqlite_import_legacy(sqlite3 *db, const char *db_dir)
{
  char cts_path[1024];
  char crate_path[1024];
  const char *dir = (db_dir && *db_dir) ? db_dir : ".";

  if (!db)
    return ERROR;

  snprintf(cts_path, sizeof(cts_path), "%s/%s", dir, CTS_DB_FILE);
  snprintf(crate_path, sizeof(crate_path), "%s/%s", dir, CRATE_DB_FILE);

  if (exec_sql(db, "BEGIN IMMEDIATE TRANSACTION;") != SUCCESS)
    return ERROR;
  if (import_cts(db, cts_path) != SUCCESS || import_crate(db, crate_path) != SUCCESS)
  {
    exec_sql(db, "ROLLBACK;");
    return ERROR;
  }
  if (exec_sql(db, "COMMIT;") != SUCCESS)
    return ERROR;
  return SUCCESS;
}

int camac_db_sqlite_table_count(sqlite3 *db, const char *table, int *count)
{
  char sql[128];
  sqlite3_stmt *stmt = 0;
  int rc;
  if (!db || !table || !count)
    return ERROR;
  snprintf(sql, sizeof(sql), "SELECT COUNT(*) FROM %s", table);
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
  if (rc != SQLITE_OK)
    return ERROR;
  rc = sqlite3_step(stmt);
  if (rc != SQLITE_ROW)
  {
    sqlite3_finalize(stmt);
    return ERROR;
  }
  *count = sqlite3_column_int(stmt, 0);
  sqlite3_finalize(stmt);
  return SUCCESS;
}

#else

int camac_db_sqlite_open(const char *path, sqlite3 **db)
{
  (void)path;
  (void)db;
  return FILE_ERROR;
}

int camac_db_sqlite_init(sqlite3 *db)
{
  (void)db;
  return FILE_ERROR;
}

int camac_db_sqlite_import_legacy(sqlite3 *db, const char *db_dir)
{
  (void)db;
  (void)db_dir;
  return FILE_ERROR;
}

int camac_db_sqlite_table_count(sqlite3 *db, const char *table, int *count)
{
  (void)db;
  (void)table;
  (void)count;
  return FILE_ERROR;
}

#endif
