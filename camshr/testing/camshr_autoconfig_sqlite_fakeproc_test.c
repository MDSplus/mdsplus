#include "testing.h"

#include "../common.h"
#include "../crate.h"
#include "../prototypes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern int CRATEdbFileIsMapped;
extern struct CRATE *CRATEdb;

extern int Autoconfig(void *ctx, char **error, char **output);

static void make_path(char *out, size_t out_len, const char *dir, const char *name)
{
  snprintf(out, out_len, "%s/%s", dir, name);
}

int main()
{
  char tmpdir[] = "/tmp/camshr-autoconfig-XXXXXX";
  char sqlite_path[1024];
  char line[CRATE_ENTRY + 1];
  int idx;
  struct Crate_ crate;
  char *error = 0;
  char *output = 0;

  BEGIN_TESTING(CamShrAutoconfigSqliteFakeProc);

  TEST1(mkdtemp(tmpdir) != 0);
  TEST1(setenv(DB_DIR, tmpdir, 1) == 0);
  TEST1(setenv("CAMSHR_DB_BACKEND", "sqlite", 1) == 0);

  CRATEdbFileIsMapped = FALSE;
  TEST1(map_data_file(CRATE_DB) == SUCCESS);
  TEST1(get_file_count(CRATE_DB) == 0);

  snprintf(line, sizeof(line), "%-.6s:%3s:%c:%c:%c\n", "GKA100", "...", '.', '0',
           '0');
  line[CRATE_ENTRY - 1] = '\n';
  TEST1(add_entry(CRATE_DB, line) == SUCCESS);
  TEST1(get_file_count(CRATE_DB) == 1);

  TEST1(Autoconfig(0, &error, &output) == SUCCESS);
  TEST1(error == 0);

  idx = lookup_entry(CRATE_DB, "GKA100");
  TEST1(idx >= 0);
  memset(&crate, 0, sizeof(crate));
  parse_crate_db(CRATEdb + idx, &crate);
  TEST1(crate.device == 0);
  TEST1(CRATEdb[idx].HwyType == '2');

  free(error);
  free(output);
  unsetenv("CAMSHR_DB_BACKEND");
  unsetenv(DB_DIR);
  make_path(sqlite_path, sizeof(sqlite_path), tmpdir, "camac.db");
  unlink(sqlite_path);
  rmdir(tmpdir);

  END_TESTING;
  return 0;
}
