#ifndef CAMAC_HW_IFACE_H
#define CAMAC_HW_IFACE_H

#include <mdsplus/mdsconfig.h>
#include "mytypes.h"
#include "ScsiCamac.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*camac_cam_piow_fn_t)(char *Name, BYTE A, BYTE F, void *Data,
                                   BYTE Mem, TranslatedIosb *iosb);

extern EXPORT int camac_hw_cam_piow(char *Name, BYTE A, BYTE F, void *Data,
                                    BYTE Mem, TranslatedIosb *iosb);
extern EXPORT void camac_hw_set_cam_piow(camac_cam_piow_fn_t fn);
extern EXPORT void camac_hw_set_proc_file(const char *path);
extern EXPORT const char *camac_hw_get_proc_file(void);
extern EXPORT FILE *camac_hw_open_proc_scsi(void);
extern EXPORT void camac_hw_reset(void);

#ifdef __cplusplus
}
#endif

#endif
