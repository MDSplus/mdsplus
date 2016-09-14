#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include "b3224_gen.h"
#include "devroutines.h"

static int one = 1;
#define pio(f,a,d)  return_on_error(DevCamChk(CamPiow(setup->name, a, f, d, 24, 0), &one, 0),status)
#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;

EXPORT int b3224___init(struct descriptor *niddsc_ptr __attribute__ ((unused)), InInitStruct * setup)
{
  int status;

  pio(16, 0, &setup->output_1);	/* Write register 1 */
  pio(25, 0, 0);		/* Strobe pulse on register 1 */
  pio(16, 1, &setup->output_2);	/* Write register 2 */
  pio(25, 1, 0);		/* Strobe pulse on register 2 */
  return status;
}
