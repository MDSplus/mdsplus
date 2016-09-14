#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>
#include <treeshr.h>
#include "hv1443_gen.h"
#include "devroutines.h"

#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;

#define HV1443_K_CHANS          16

extern int hv1443___get_settings(struct descriptor *niddsc __attribute__ ((unused)), InGet_settingsStruct * setup);
extern int GenDeviceFree();

EXPORT int hv1443__get_settings(struct descriptor *niddsc_ptr __attribute__ ((unused)),
				struct descriptor *meth __attribute__ ((unused)), int max_chans,
			 int *settings)
{
  int status = 1;
  static InGet_settingsStruct setup;
  if (max_chans != HV1443_K_CHANS)
    return HV1440$_WRONG_POD_TYPE;
  status = hv1443___get_settings(niddsc_ptr, &setup);
  if (status & 1) {
    int i;
    for (i = 0; i < HV1443_K_CHANS; i++) {
      int nid = setup.head_nid + HV1443_N_VOLTAGE_01 + i;
      if (TreeIsOn(nid) & 1) {
	if ((DevLong(&nid, &settings[i]) & 1) == 0)
	  settings[i] = 0;
      } else
	settings[i] = 0;
    }
    GenDeviceFree(&setup);
  }
  return status;
}
