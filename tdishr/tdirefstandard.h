/*	tdirefstandard.h
	Definitions of standard Tdi1xxxx call entry:
		#include TDI$$REF_STANDARD
		TdiRefStandard(name);

	Ken Klare, LANL P-4	(c)1989,1991
*/
#ifdef HAVE_VXWORKS_H
#include <stdio.h>
/*Just a workaround for a gnu c compiler bug in checking definitions */
#endif
#include <mdsdescrip.h>
#include <mds_stdarg.h>
/*#include <mdsdescrip.h>*/

static struct descriptor_xd const EMPTY_XD = {0,DTYPE_DSC,CLASS_XD,0,0};

#define TdiRefStandard(name) int name(\
int				opcode,\
int				narg,\
struct descriptor		*list[],\
struct descriptor_xd	*out_ptr)\
{\
int				status = 1;
