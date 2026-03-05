#include "testing.h"

#include "../camac_hw_iface.h"
#include "../common.h"
#include "../crate.h"
#include "../prototypes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern int CRATEdbFileIsMapped;
extern struct CRATE *CRATEdb;

static short g_fake_status_word = 0;
static int g_fake_piow_calls = 0;

static int fake_cam_piow(char *Name, BYTE A, BYTE F, void *Data, BYTE Mem,
                         TranslatedIosb *iosb)
{
  (void)Name;
  (void)A;
  (void)Mem;
  (void)iosb;
  ++g_fake_piow_calls;
  if (F == 1 && Data)
    *(short *)Data = g_fake_status_word;
  return SUCCESS;
}

static void make_path(char *out, size_t out_len, const char *dir, const char *name)
{
  snprintf(out, out_len, "%s/%s", dir, name);
}

int main()
{
  char tmpdir[] = "/tmp/camshr-fakehw-XXXXXX";
  char crate_path[1024];
  char rec[CRATE_ENTRY];
  FILE *fp;
  int idx;
  char crate_on[] = "gka100";
  char crate_off[] = "GKA100";

  BEGIN_TESTING(CamShrFakeCrateState);

  TEST1(mkdtemp(tmpdir) != 0);
  make_path(crate_path, sizeof(crate_path), tmpdir, CRATE_DB_FILE);

  memset(rec, ' ', sizeof(rec));
  snprintf(rec, sizeof(rec), "%-.6s:%3s:%c:%c:%c\n", "GKA100", "001", '2', '0',
           '0');
  {
    size_t i;
    for (i = 0; i < sizeof(rec); ++i)
      if (rec[i] == '\0')
        rec[i] = ' ';
  }
  rec[sizeof(rec) - 1] = '\n';

  fp = fopen(crate_path, "wb");
  TEST1(fp != 0);
  TEST1(fwrite(rec, 1, sizeof(rec), fp) == sizeof(rec));
  fclose(fp);

  TEST1(setenv(DB_DIR, tmpdir, 1) == 0);

  CRATEdbFileIsMapped = FALSE;
  camac_hw_reset();
  camac_hw_set_cam_piow(fake_cam_piow);

  TEST1(map_data_file(CRATE_DB) == SUCCESS);
  idx = lookup_entry(CRATE_DB, "GKA100");
  TEST1(idx >= 0);

  g_fake_status_word = 0x4000;
  TEST1(turn_crate_on_off_line(crate_on, ON) == SUCCESS);
  TEST1(CRATEdb[idx].online == '1');
  TEST1(CRATEdb[idx].enhanced == '1');

  g_fake_status_word = 0x1000;
  TEST1(turn_crate_on_off_line(crate_off, OFF) == SUCCESS);
  TEST1(CRATEdb[idx].online == '0');
  TEST1(CRATEdb[idx].enhanced == '0');

  TEST1(g_fake_piow_calls >= 6);

  camac_hw_reset();
  unsetenv(DB_DIR);
  unlink(crate_path);
  rmdir(tmpdir);

  END_TESTING;
  return 0;
}
