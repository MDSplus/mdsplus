/**********************************************************************
* CCLDEF.H --
*
* Local "include" file for CCL ...
*
***********************************************************************/

#ifndef __CCLDEF_H
#define __CCLDEF_H   1

#ifdef __vms
#ifndef vms
#define vms    1
#endif
#endif

#include        "mdsdcldef.h"
#include        "clisysdef.h"

#define CCL_STS(N)    (CCL_FACILITY<<16)+N

#define CCL_STS_SUCCESS     CCL_STS(1)

#endif		/* __CCLDEF_H	*/

#ifdef CREATE_STS_TEXT
static struct stsText  ccl_stsText[] = {
        STS_TEXT(CCL_STS_SUCCESS,"Success")
       };
#endif
