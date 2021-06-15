/*	tdirefstandard.h
        Definitions of standard Tdi1xxxx call entry:
                #include TDI$$REF_STANDARD
                TdiRefStandard(name);

        Ken Klare, LANL P-4	(c)1989,1991
*/
#include <mdsdescrip.h>
#include <mds_stdarg.h>

static struct descriptor_xd const EMPTY_XD = MDSDSC_XD_INITIALIZER;

#define TdiRefStandard(name)                                \
  int name(int opcode, int narg, struct descriptor *list[], \
           struct descriptor_xd *out_ptr)                   \
  {                                                         \
    int status = 1;
