#include "camac_hw_iface.h"

#include "common.h"
#include "prototypes.h"

#include <stdlib.h>

static camac_cam_piow_fn_t g_cam_piow = 0;
static char g_proc_file_path[1024];

void camac_hw_set_cam_piow(camac_cam_piow_fn_t fn) { g_cam_piow = fn; }

void camac_hw_set_proc_file(const char *path)
{
  if (path && *path)
    snprintf(g_proc_file_path, sizeof(g_proc_file_path), "%s", path);
  else
    g_proc_file_path[0] = '\0';
}

const char *camac_hw_get_proc_file(void)
{
  const char *env_path;
  if (g_proc_file_path[0] != '\0')
    return g_proc_file_path;
  env_path = getenv("CAMSHR_PROC_FILE");
  if (env_path && *env_path)
    return env_path;
  return PROC_FILE;
}

FILE *camac_hw_open_proc_scsi(void) { return fopen(camac_hw_get_proc_file(), "r"); }

int camac_hw_cam_piow(char *Name, BYTE A, BYTE F, void *Data, BYTE Mem,
                      TranslatedIosb *iosb)
{
  if (g_cam_piow)
    return g_cam_piow(Name, A, F, Data, Mem, iosb);
  return CamPiow(Name, A, F, Data, Mem, iosb);
}

void camac_hw_reset(void)
{
  g_cam_piow = 0;
  g_proc_file_path[0] = '\0';
}
