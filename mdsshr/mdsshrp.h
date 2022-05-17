#ifndef _MDSSHRP_H
#define _MDSSHRP_H

#include <mdsdescrip.h>

extern int MdsCmprs(int const *nitems, struct descriptor_a const *items,
                    struct descriptor_a *pack, int *bit);
extern int MdsGetStdMsg(int status, const char **fac_out,
                        const char **msgnam_out, const char **text_out);

extern void MdsPk(signed char *nbits_ptr, int *nitems_ptr, int pack[],
                  int items[], int *bit_ptr);

extern void MdsUnpk(signed char *nbits_ptr, int *nitems_ptr, int pack[],
                    int items[], int *bit_ptr);

extern int MDSUdpEventAst(char const *eventName,
                          void (*astadr)(void *, int, char *), void *astprm,
                          int *eventid);
extern int MDSUdpEventAstMask(char const *eventName,
                              void (*astadr)(void *, int, char *), void *astprm,
                              int *eventid, unsigned int cpuMask);
extern int MDSUdpEventCan(int eventid);
extern int MDSUdpEvent(char const *eventName, int bufLen, char const *buf);
extern int LibSFree1Dd(struct descriptor *out);
#endif //indef _MDSSHRP_H