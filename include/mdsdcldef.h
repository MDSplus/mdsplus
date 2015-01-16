
/**********************************************************************
* MDSDCLDEF.H --
*
* Messages for MDSDCL ...
*
***********************************************************************/

#ifndef __MDSDCLDEF
#define __MDSDCLDEF   1

#include <facility_list.h>

	/****************************************************************
	 * Status codes:
	 ****************************************************************/
#define MDSDCL_STS(N)   (MDSDCL_FACILITY<<16)+N

#define MDSDCL_STS_SUCCESS         MDSDCL_STS(1)
#define MDSDCL_STS_INDIRECT_DONE   MDSDCL_STS(0x103)

#define MDSDCL_STS_ERROR           MDSDCL_STS(0x202)
#define MDSDCL_STS_INDIRECT_ERROR  MDSDCL_STS(0x304)
#define MDSDCL_STS_INDIRECT_EOF    MDSDCL_STS(0x406)

#ifdef CREATE_STS_TEXT
static struct stsText mdsdcl_stsText[] = {
  STS_TEXT(MDSDCL_STS_SUCCESS, "Normal successful completion")
      , STS_TEXT(MDSDCL_STS_INDIRECT_DONE, "Success from indirect command")

      , STS_TEXT(MDSDCL_STS_ERROR, "Generic MDSDCL Error")
      , STS_TEXT(MDSDCL_STS_INDIRECT_ERROR, "Error from indirect command")
      , STS_TEXT(MDSDCL_STS_INDIRECT_EOF, "End-Of-File in indirect command")
};
#endif

#endif				/* __MDSDCLDEF  */
