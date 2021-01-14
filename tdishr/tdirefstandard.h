/*	tdirefstandard.h
        Definitions of standard Tdi1xxxx call entry:
                #include TDI$$REF_STANDARD
                TdiRefStandard(name);

        Ken Klare, LANL P-4	(c)1989,1991
*/
#include <mdsdescrip.h>
#include <stdio.h>
static struct descriptor_xd const EMPTY_XD = {0, DTYPE_DSC, CLASS_XD, 0, 0};

#include <mds_stdarg.h>
/*#include <mdsdescrip.h>*/

#define TdiRefStandard(name)                                                   \
  int name(int opcode, int narg, struct descriptor *list[],                    \
           struct descriptor_xd *out_ptr) {                                    \
    int status = 1;
