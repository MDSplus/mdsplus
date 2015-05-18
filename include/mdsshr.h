#ifndef MDS_MDSSHR_H_
#define MDS_MDSSHR_H_

#define MdsCOMPRESSIBLE 3
#include <config.h>
#include <mdsdescrip.h>
#include <mdstypes.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

  EXPORT extern int MdsCmprs(int *nitems, struct descriptor_a *items, struct descriptor_a *pack,
			     int *bit);
  EXPORT extern int MdsCompress(struct descriptor *cimage, struct descriptor *centry,
				struct descriptor *in, struct descriptor_xd *out);
  EXPORT extern int MdsDecompress(struct descriptor_r *rec_ptr, struct descriptor_xd *out_ptr);
  EXPORT extern int MdsCopyDxXd(struct descriptor *in, struct descriptor_xd *out);
  EXPORT extern int MdsCopyDxXdZ(struct descriptor *in, struct descriptor_xd *out, void **zone,
				 int (*fixup_nid) (), void *fixup_nid_arg, int (*fixup_path) (),
				 void *fixup_path_arg);
  EXPORT extern char *MdsDescrToCstring(struct descriptor *);
  EXPORT extern int MDSfprintf(FILE * fp, char *fmt, ...);
  EXPORT extern void MdsFree(void *);
  EXPORT extern int MdsGet1Dx(unsigned int *len, unsigned char *dtype, struct descriptor_xd *dsc,
			      void **zone);
  EXPORT extern int MdsGet1DxA(struct descriptor_a *in, unsigned short *len, unsigned char *dtype,
			       struct descriptor_xd *out);
  EXPORT extern int MdsGet1DxS(unsigned short *len, unsigned char *dtype,
			       struct descriptor_xd *out);
  EXPORT extern int MdsGetCurrentShotId(char *experiment);
  EXPORT extern char *MdsGetMsg(int sts);
  EXPORT extern int MdsFree1Dx(struct descriptor_xd *dsc, void **zone);
  EXPORT extern int MdsMsg(int sts, char fmt[], ...);
  EXPORT extern void MdsPk(char *nbits, int *nitems, int pack[], int items[], int *bit);
  EXPORT extern int MDSQueueEvent(char *event, int *eventid);
  EXPORT extern int MDSGetEventQueue(int eventid, int timeout, int *data_len, char **data);
  EXPORT extern int MdsSetCurrentShotId(char *experiment, int shot);
  EXPORT extern int MdsSerializeDscIn(char const *in, struct descriptor_xd *out);
  EXPORT extern int MdsSerializeDscOutZ(struct descriptor *in, struct descriptor_xd *out,
					int (*fixupNid) (), void *fixupNidArg, int (*fixupPath) (),
					void *fixupPathArg, int compress, int *compressible,
					unsigned int *length, unsigned int *reclen,
					unsigned char *dtype, unsigned char *classType,
					int altbuflen, void *altbuf, int *data_in_altbuf);
  EXPORT extern int MdsSerializeDscOut(struct descriptor *in, struct descriptor_xd *out);
  EXPORT extern void MdsUnpk(char *nbits, int *nitems, int pack[], int items[], int *bit);
  EXPORT extern int MdsXpand(int *nitems, struct descriptor_a *pack, struct descriptor_a *items,
			     int *bit);
  EXPORT extern int MDSEvent(char const *eventName, int num_bytes, char *data);
  EXPORT extern int MDSEventAst(char const *eventName, void (*astadr) (void *, int, char *),
				void *astprm, int *eventid);
  EXPORT extern int MDSEventCan(int eventid);
  EXPORT extern int MDSUdpEvent(char const *eventName, int bufLen, char const *buf);
  EXPORT extern int MDSUdpEventAst(char const *eventName, void (*astadr) (void *, int, char *),
				   void *astprm, int *eventid);
  EXPORT extern int MDSUdpEventCan(int id);
  EXPORT extern int MDSWfevent(char *evname, int buflen, char *data, int *datlen);
  EXPORT extern int MDSWfeventTimed(char *evname, int buflen, char *data, int *datlen, int timeout);
  EXPORT extern char *MdsDtypeString(int id);
  EXPORT extern char *MdsClassString(int id);
  EXPORT extern int MDSprintf(char *fmt, ...);
  EXPORT extern char *MdsUsageString(int id);
  EXPORT extern char *TranslateLogical(char *name);
  EXPORT extern void TranslateLogicalFree(char *value);
  EXPORT extern int TranslateLogicalXd(struct descriptor *in, struct descriptor_xd *out);
  EXPORT extern const char *MdsRelease();
  EXPORT extern void MdsFloatToTime(double floatTime, uint64_t * outTime);
  EXPORT extern void MdsFloatToDelta(double floatTime, uint64_t * outTime);
  EXPORT extern void MdsTimeToFloat(uint64_t inTime, float *outFloat);
  EXPORT extern void MdsTimeToDouble(uint64_t inTime, double *outFloat);
  EXPORT extern int MdsPutEnv(char *cmd);
  EXPORT extern void MdsGlobalLock();
  EXPORT extern void MdsGlobalUnlock();
  EXPORT extern int MdsGetStdMsg(int status, const char **fac_out, const char **msgnam_out, const char **text_out);


#ifdef __cplusplus
}
#endif
#endif				/* MDS_MDSSHR_H_ */
