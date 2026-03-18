#include "testing.h"

#include "../common.h"
#include "../cts_p.h"
#include "../module.h"
#include "../prototypes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern int CTSdbFileIsMapped;
extern struct MODULE *CTSdb;

static void make_path(char *out, size_t out_len, const char *dir, const char *name)
{
  snprintf(out, out_len, "%s/%s", dir, name);
}

int main()
{
  char tmpdir[] = "/tmp/camshr-sqlite-backend-XXXXXX";
  char sqlite_path[1024];
  char line[MODULE_ENTRY + 1];
  struct Module_ mod;
  int idx;

  BEGIN_TESTING(CamShrSqliteBackendRoundtrip);

  TEST1(mkdtemp(tmpdir) != 0);
  TEST1(setenv(DB_DIR, tmpdir, 1) == 0);

  CTSdbFileIsMapped = FALSE;
  TEST1(map_data_file(CTS_DB) == SUCCESS);
  TEST1(get_file_count(CTS_DB) == 0);

  snprintf(line, sizeof(line), "%-32s %-10s %-40s\n", "ALPHA01", "GKA100:N01",
           "sqlite backend");
  line[MODULE_ENTRY - 1] = '\n';

  TEST1(add_entry(CTS_DB, line) == SUCCESS);
  TEST1(get_file_count(CTS_DB) == 1);

  idx = lookup_entry(CTS_DB, "ALPHA01");
  TEST1(idx >= 0);
  memset(&mod, 0, sizeof(mod));
  parse_cts_db(CTSdb + idx, &mod);
  TEST0(strcmp(mod.name, "ALPHA01"));
  TEST1(mod.adapter == 0);
  TEST1(mod.id == 1);
  TEST1(mod.crate == 0);
  TEST1(mod.slot == 1);

  CTSdbFileIsMapped = FALSE;
  TEST1(map_data_file(CTS_DB) == SUCCESS);
  TEST1(get_file_count(CTS_DB) == 1);
  TEST1(lookup_entry(CTS_DB, "ALPHA01") >= 0);

  TEST1(remove_entry(CTS_DB, 0) == SUCCESS);
  TEST1(get_file_count(CTS_DB) == 0);

  unsetenv(DB_DIR);
  make_path(sqlite_path, sizeof(sqlite_path), tmpdir, "camac.db");
  unlink(sqlite_path);
  rmdir(tmpdir);

  END_TESTING;
  return 0;
}
