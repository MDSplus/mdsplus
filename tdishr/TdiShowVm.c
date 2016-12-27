/*      Tdi1ShowVm.C
        Show virtual memory zone
                SHOW_VM([print-level], [selection-mask])

        Ken Klare, LANL CTR-7   (c)1989,1990
*/

#include <STATICdef.h>
#include "tdirefstandard.h"
#include <libroutines.h>
#include <mdsshr_messages.h>



extern int TdiGetLong();

int Tdi1ShowVm()
{
  /*
  INIT_STATUS;
  int code, contex = 0, j = 1, mask, zone_id = 0;

  if (narg > 0 && list[0])
    status = TdiGetLong(list[0], &code);
  else
    code = 3;
  if (narg > 1 && list[1] && STATUS_OK)
    status = TdiGetLong(list[1], &mask);
  else
    mask = -1;
  while STATUS_OK {
    status = LibFindVmZone(&contex, &zone_id);
    if (STATUS_OK && zone_id && j & mask)
      status = LibShowVmZone(&zone_id, &code);
    j <<= 1;
  }
  if (status == LibNOTFOU)
    status = MDSplusSUCCESS;
  return status;
  */
  return 0;
}
