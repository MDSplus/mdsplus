/*	Tdi1ShowVm.C
	Show virtual memory zone
		SHOW_VM([print-level], [selection-mask])

	Ken Klare, LANL CTR-7	(c)1989,1990
*/

#include "tdirefstandard.h"
#include <libroutines.h>
#include <librtl_messages.h>

extern int TdiGetLong();

TdiRefStandard(Tdi1ShowVm)
int	code, contex = 0, j = 1, mask, zone_id = 0;

	if (narg > 0 && list[0]) status = TdiGetLong(list[0], &code);
	else code = 3;
	if (narg > 1 && list[1] && status & 1) status = TdiGetLong(list[1], &mask);
	else mask = -1;
	while (status & 1) {
		status = LibFindVmZone(&contex, &zone_id);
		if (status & 1 && zone_id && j & mask) status = LibShowVmZone(&zone_id, &code);
		j <<= 1;
	}
	if (status == LibNOTFOU) status = 1;
	return status;
}
