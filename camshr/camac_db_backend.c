#include "camac_db_backend.h"

#include "camac_db_sqlite.h"
#include "common.h"
#include "crate.h"
#include "module.h"
#include "prototypes.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

extern int CTSdbFileIsMapped;
extern int CRATEdbFileIsMapped;
extern struct MODULE *CTSdb;
extern struct CRATE *CRATEdb;

#ifdef CAMSHR_HAVE_SQLITE3

static sqlite3 *g_db = 0;
static int g_cts_capacity = 0;
static int g_crate_capacity = 0;
static int g_cts_owned = FALSE;
static int g_crate_owned = FALSE;
static int g_warned_mmap_fallback = FALSE;
static int g_warned_unknown_backend = FALSE;

static int round_capacity(int count, int incr)
{
  if (incr <= 0)
    incr = 1;
  if (count < 0)
    count = 0;
  return ((count / incr) + 1) * incr;
}

static int decode_hwy_char(char c)
{
  if (c == '.')
    return TYPE_UNKNOWN;
  if (isdigit((unsigned char)c))
    return c - '0';
  return (unsigned char)c;
}

static char encode_hwy_char(int type)
{
  if (type >= 0 && type <= 9)
    return (char)('0' + type);
  if (type == TYPE_UNKNOWN)
    return '.';
  return (char)type;
}

static int snapshot_count(int dbType)
{
  int i;
  int capacity;
  int entry_size;
  char *p;

  switch (dbType)
  {
  case CTS_DB:
    p = (char *)CTSdb;
    capacity = g_cts_capacity;
    entry_size = MODULE_ENTRY;
    break;
  case CRATE_DB:
    p = (char *)CRATEdb;
    capacity = g_crate_capacity;
    entry_size = CRATE_ENTRY;
    break;
  default:
    return 0;
  }
  if (!p || capacity <= 0)
    return 0;
  for (i = 0; i < capacity; ++i)
    if (*(p + (i * entry_size)) == ' ')
      return i;
  return capacity;
}

static int ensure_open(void)
{
  const char *db_dir;
  int cts_count = 0;
  int crate_count = 0;

  if (g_db)
    return SUCCESS;

  if (camac_db_sqlite_open(get_file_name("camac.db"), &g_db) != SUCCESS)
    return FILE_ERROR;
  if (camac_db_sqlite_init(g_db) != SUCCESS)
    return ERROR;

  if (camac_db_sqlite_table_count(g_db, "cts_modules", &cts_count) != SUCCESS)
    return ERROR;
  if (camac_db_sqlite_table_count(g_db, "crate_map", &crate_count) != SUCCESS)
    return ERROR;

  if (cts_count == 0 && crate_count == 0)
  {
    db_dir = getenv(DB_DIR);
    if (camac_db_sqlite_import_legacy(g_db, db_dir ? db_dir : ".") != SUCCESS)
      return ERROR;
  }

  return SUCCESS;
}

static int load_cts_snapshot(void)
{
  sqlite3_stmt *count_stmt = 0;
  sqlite3_stmt *stmt = 0;
  struct MODULE *buf = 0;
  int count = 0;
  int idx = 0;
  int cap;

  if (sqlite3_prepare_v2(g_db, "SELECT COUNT(*) FROM cts_modules", -1, &count_stmt,
                         0) != SQLITE_OK)
    return ERROR;
  if (sqlite3_step(count_stmt) == SQLITE_ROW)
    count = sqlite3_column_int(count_stmt, 0);
  sqlite3_finalize(count_stmt);

  cap = round_capacity(count, CTS_DB_INCREMENT);
  buf = malloc((size_t)cap * sizeof(struct MODULE));
  if (!buf)
    return NO_MEMORY;
  memset(buf, ' ', (size_t)cap * sizeof(struct MODULE));

  if (sqlite3_prepare_v2(
          g_db,
          "SELECT logical_name,adapter,scsi_id,crate,slot,comment "
          "FROM cts_modules ORDER BY logical_name COLLATE NOCASE",
          -1, &stmt, 0) != SQLITE_OK)
  {
    free(buf);
    return ERROR;
  }

  while (sqlite3_step(stmt) == SQLITE_ROW && idx < count)
  {
    const char *name = (const char *)sqlite3_column_text(stmt, 0);
    int adapter = sqlite3_column_int(stmt, 1);
    int scsi_id = sqlite3_column_int(stmt, 2);
    int crate = sqlite3_column_int(stmt, 3);
    int slot = sqlite3_column_int(stmt, 4);
    const char *comment = (const char *)sqlite3_column_text(stmt, 5);
    char line[MODULE_ENTRY + 1];
    char phys[16];

    snprintf(phys, sizeof(phys), "GK%c%d%02d:N%d", 'A' + adapter, scsi_id, crate,
             slot);
    memset(line, ' ', sizeof(line));
    snprintf(line, sizeof(line), "%-32s %-10s %-40s\n", name ? name : "",
             phys, comment ? comment : "");
    memcpy(&buf[idx], line, MODULE_ENTRY);
    idx++;
  }
  sqlite3_finalize(stmt);

  if (g_cts_owned && CTSdb)
    free(CTSdb);
  CTSdb = buf;
  g_cts_capacity = cap;
  g_cts_owned = TRUE;
  CTSdbFileIsMapped = TRUE;
  return SUCCESS;
}

static int load_crate_snapshot(void)
{
  sqlite3_stmt *count_stmt = 0;
  sqlite3_stmt *stmt = 0;
  struct CRATE *buf = 0;
  int count = 0;
  int idx = 0;
  int cap;

  if (sqlite3_prepare_v2(g_db, "SELECT COUNT(*) FROM crate_map", -1, &count_stmt,
                         0) != SQLITE_OK)
    return ERROR;
  if (sqlite3_step(count_stmt) == SQLITE_ROW)
    count = sqlite3_column_int(count_stmt, 0);
  sqlite3_finalize(count_stmt);

  cap = round_capacity(count, CRATE_DB_INCREMENT);
  buf = malloc((size_t)cap * sizeof(struct CRATE));
  if (!buf)
    return NO_MEMORY;
  memset(buf, ' ', (size_t)cap * sizeof(struct CRATE));

  if (sqlite3_prepare_v2(
          g_db,
          "SELECT crate_name,device_num,highway_type,enhanced,online "
          "FROM crate_map ORDER BY crate_name COLLATE NOCASE",
          -1, &stmt, 0) != SQLITE_OK)
  {
    free(buf);
    return ERROR;
  }

  while (sqlite3_step(stmt) == SQLITE_ROW && idx < count)
  {
    const char *crate_name = (const char *)sqlite3_column_text(stmt, 0);
    int device_num = sqlite3_column_int(stmt, 1);
    int highway_type = sqlite3_column_int(stmt, 2);
    int enhanced = sqlite3_column_int(stmt, 3);
    int online = sqlite3_column_int(stmt, 4);
    char line[CRATE_ENTRY + 1];
    char dsf[4];
    char hwy_char = encode_hwy_char(highway_type);

    if (device_num >= 0)
      snprintf(dsf, sizeof(dsf), "%03d", device_num);
    else
      snprintf(dsf, sizeof(dsf), "...");
    memset(line, ' ', sizeof(line));
    snprintf(line, sizeof(line), "%-.6s:%3s:%c:%c:%c\n",
             crate_name ? crate_name : "", dsf, hwy_char,
             enhanced ? '1' : '0', online ? '1' : '0');
    memcpy(&buf[idx], line, CRATE_ENTRY);
    idx++;
  }
  sqlite3_finalize(stmt);

  if (g_crate_owned && CRATEdb)
    free(CRATEdb);
  CRATEdb = buf;
  g_crate_capacity = cap;
  g_crate_owned = TRUE;
  CRATEdbFileIsMapped = TRUE;
  return SUCCESS;
}

static int commit_cts(void)
{
  sqlite3_stmt *stmt = 0;
  int i;
  int count = snapshot_count(CTS_DB);

  if (sqlite3_exec(g_db, "DELETE FROM cts_modules", 0, 0, 0) != SQLITE_OK)
    return ERROR;

  if (sqlite3_prepare_v2(
          g_db,
          "INSERT INTO cts_modules"
          "(logical_name,adapter,scsi_id,crate,slot,comment) VALUES(?,?,?,?,?,?)",
          -1, &stmt, 0) != SQLITE_OK)
    return ERROR;

  for (i = 0; i < count; ++i)
  {
    struct Module_ mod;
    parse_cts_db(CTSdb + i, &mod);
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
      sqlite3_finalize(stmt);
      return ERROR;
    }
  }
  sqlite3_finalize(stmt);
  return SUCCESS;
}

static int commit_crate(void)
{
  sqlite3_stmt *stmt = 0;
  int i;
  int count = snapshot_count(CRATE_DB);

  if (sqlite3_exec(g_db, "DELETE FROM crate_map", 0, 0, 0) != SQLITE_OK)
    return ERROR;

  if (sqlite3_prepare_v2(
          g_db,
          "INSERT INTO crate_map"
          "(crate_name,device_num,highway_type,enhanced,online) VALUES(?,?,?,?,?)",
          -1, &stmt, 0) != SQLITE_OK)
    return ERROR;

  for (i = 0; i < count; ++i)
  {
    struct Crate_ crate;
    parse_crate_db(CRATEdb + i, &crate);
    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);
    sqlite3_bind_text(stmt, 1, crate.name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, crate.device);
    sqlite3_bind_int(stmt, 3, decode_hwy_char(CRATEdb[i].HwyType));
    sqlite3_bind_int(stmt, 4, crate.enhanced);
    sqlite3_bind_int(stmt, 5, crate.online);
    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
      sqlite3_finalize(stmt);
      return ERROR;
    }
  }
  sqlite3_finalize(stmt);
  return SUCCESS;
}

int camac_db_backend_enabled(void)
{
  const char *v = getenv("CAMSHR_DB_BACKEND");
  if (!v || *v == '\0')
    return TRUE; // sqlite is the default backend

  if ((strcasecmp(v, "mmap") == 0) || (strcasecmp(v, "legacy") == 0) ||
      (strcasecmp(v, "file") == 0))
  {
    if (!g_warned_mmap_fallback)
    {
      fprintf(stderr,
              "camshr: CAMSHR_DB_BACKEND=%s enables legacy mmap backend "
              "(sqlite is default)\n",
              v);
      g_warned_mmap_fallback = TRUE;
    }
    return FALSE;
  }

  if ((strcasecmp(v, "sqlite") == 0) || (strcasecmp(v, "sql") == 0) ||
      (strcasecmp(v, "auto") == 0))
    return TRUE;

  if (!g_warned_unknown_backend)
  {
    fprintf(stderr,
            "camshr: unknown CAMSHR_DB_BACKEND=%s, defaulting to sqlite\n", v);
    g_warned_unknown_backend = TRUE;
  }

  return TRUE;
}

int camac_db_backend_map(int dbType)
{
  if (ensure_open() != SUCCESS)
    return MAP_ERROR;

  switch (dbType)
  {
  case CTS_DB:
    return load_cts_snapshot();
  case CRATE_DB:
    return load_crate_snapshot();
  default:
    return ERROR;
  }
}

int camac_db_backend_commit(int dbType)
{
  int rc = ERROR;
  if (ensure_open() != SUCCESS)
    return COMMIT_ERROR;

  if (sqlite3_exec(g_db, "BEGIN IMMEDIATE TRANSACTION", 0, 0, 0) != SQLITE_OK)
    return COMMIT_ERROR;

  switch (dbType)
  {
  case CTS_DB:
    rc = commit_cts();
    break;
  case CRATE_DB:
    rc = commit_crate();
    break;
  default:
    rc = ERROR;
    break;
  }

  if (rc == SUCCESS)
  {
    if (sqlite3_exec(g_db, "COMMIT", 0, 0, 0) != SQLITE_OK)
      rc = ERROR;
  }
  else
    sqlite3_exec(g_db, "ROLLBACK", 0, 0, 0);

  return rc == SUCCESS ? SUCCESS : COMMIT_ERROR;
}

int camac_db_backend_get_size_bytes(const char *FileName)
{
  if (!FileName)
    return ERROR;
  if (strcmp(FileName, CTS_DB_FILE) == 0)
  {
    if (!CTSdbFileIsMapped && camac_db_backend_map(CTS_DB) != SUCCESS)
      return ERROR;
    return g_cts_capacity * MODULE_ENTRY;
  }
  if (strcmp(FileName, CRATE_DB_FILE) == 0)
  {
    if (!CRATEdbFileIsMapped && camac_db_backend_map(CRATE_DB) != SUCCESS)
      return ERROR;
    return g_crate_capacity * CRATE_ENTRY;
  }
  return ERROR;
}

int camac_db_backend_expand(int dbType, int numOfEntries)
{
  int newcap;
  void *p;
  int oldcap;
  int entry;

  switch (dbType)
  {
  case CTS_DB:
    oldcap = g_cts_capacity;
    entry = MODULE_ENTRY;
    newcap = round_capacity(numOfEntries, CTS_DB_INCREMENT);
    if (newcap <= oldcap)
      return SUCCESS;
    p = realloc(CTSdb, (size_t)newcap * entry);
    if (!p)
      return NO_MEMORY;
    memset((char *)p + ((size_t)oldcap * entry), ' ',
           (size_t)(newcap - oldcap) * entry);
    CTSdb = (struct MODULE *)p;
    g_cts_capacity = newcap;
    g_cts_owned = TRUE;
    return SUCCESS;
  case CRATE_DB:
    oldcap = g_crate_capacity;
    entry = CRATE_ENTRY;
    newcap = round_capacity(numOfEntries, CRATE_DB_INCREMENT);
    if (newcap <= oldcap)
      return SUCCESS;
    p = realloc(CRATEdb, (size_t)newcap * entry);
    if (!p)
      return NO_MEMORY;
    memset((char *)p + ((size_t)oldcap * entry), ' ',
           (size_t)(newcap - oldcap) * entry);
    CRATEdb = (struct CRATE *)p;
    g_crate_capacity = newcap;
    g_crate_owned = TRUE;
    return SUCCESS;
  default:
    return ERROR;
  }
}

int camac_db_backend_contract(int dbType, int numOfEntries)
{
  (void)dbType;
  (void)numOfEntries;
  return SUCCESS;
}

#else

int camac_db_backend_enabled(void) { return FALSE; }
int camac_db_backend_map(int dbType)
{
  (void)dbType;
  return MAP_ERROR;
}
int camac_db_backend_commit(int dbType)
{
  (void)dbType;
  return COMMIT_ERROR;
}
int camac_db_backend_get_size_bytes(const char *FileName)
{
  (void)FileName;
  return ERROR;
}
int camac_db_backend_expand(int dbType, int numOfEntries)
{
  (void)dbType;
  (void)numOfEntries;
  return ERROR;
}
int camac_db_backend_contract(int dbType, int numOfEntries)
{
  (void)dbType;
  (void)numOfEntries;
  return ERROR;
}

#endif
