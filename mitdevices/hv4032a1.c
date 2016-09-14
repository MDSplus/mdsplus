#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>
#include <treeshr.h>
#include <mdsshr.h>
#include "hv4032a1_gen.h"
#include "devroutines.h"

#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;
#define HV4032A1_K_CHANS         4

extern int hv4032a1___get_settings(struct descriptor *niddsc __attribute__ ((unused)), InGet_settingsStruct * setup);
extern int GenDeviceFree();

EXPORT int hv4032a1__get_settings(struct descriptor *niddsc_ptr __attribute__ ((unused)), struct descriptor *meth __attribute__ ((unused)), int max_chans,
			   int *settings)
{
  int status = 1;
  static InGet_settingsStruct setup;
  if (max_chans != HV4032A1_K_CHANS)
    return HV4032$_WRONG_POD_TYPE;
  status = hv4032a1___get_settings(niddsc_ptr, &setup);
  if (status & 1) {
    int i;
    for (i = 0; i < HV4032A1_K_CHANS; i++) {
      int nid = setup.head_nid + HV4032A1_N_VOLTAGE_1 + i;
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
