#ifdef __VMS
#define MdsCOMPRESSIBLE MDS$_COMPRESSIBLE
#define MdsGet1Dx MDS$GET1_DX
#define MdsGet1DxS MDS$GET1_DX_S
#define MdsGet1DxA Mds$GET1_DX_A
#define MdsFree1Dx MDS$FREE1_DX
#define MdsXpand MDS$XPAND
#define MdsCopyDxXd MDS$COPY_DXXD
#define MdsPk MDS$PK
#define MdsUnpk MDS$UNPK
#define MdsCmprs MDS$CMPRS

#define MdsGetCurrentShotid MDS$GET_CURRENT_SHOTID
#pragma EXPORT extern_model save
#pragma EXPORT extern_model globalvalue
EXPORT extern MdsCOMPRESSIBLE;
#pragma EXPORT extern_model restore
#else
#define MdsCOMPRESSIBLE 3
#endif
#ifndef HAVE_VXWORKS_H
#include <config.h>
#else
#define EXPORT
#endif
#include <mdsdescrip.h>
#include <mdstypes.h>
EXPORT extern int MdsCmprs(int *nitems, struct descriptor_a *items, struct descriptor_a *pack, int *bit);
EXPORT extern int MdsCompress(struct descriptor *cimage, struct descriptor *centry, struct descriptor *in, struct descriptor_xd *out);
EXPORT extern int MdsDecompress(struct descriptor_r *rec_ptr, struct descriptor_xd *out_ptr);
EXPORT extern int MdsCopyDxXd(struct descriptor *in, struct descriptor_xd *out);
EXPORT extern int MdsCopyDxXdZ(struct descriptor *in, struct descriptor_xd *out, void **zone, int (*fixup_nid) (), void *fixup_nid_arg,
                        int (*fixup_path) (), void *fixup_path_arg);
EXPORT extern char *MdsDescrToCstring(struct descriptor *);
EXPORT extern void MdsFree(void *);
EXPORT extern int MdsGet1Dx(unsigned int *len, unsigned char *dtype, struct descriptor_xd *dsc, void **zone);
EXPORT extern int MdsGet1DxA(struct descriptor_a * in, unsigned short *len, unsigned char *dtype, struct descriptor_xd *out);
EXPORT extern int MdsGet1DxS(unsigned short *len, unsigned char *dtype, struct descriptor_xd *out);
EXPORT extern int MdsGetCurrentShotId(char *experiment);
EXPORT extern char *MdsGetMsg(int sts);
EXPORT extern int MdsFree1Dx(struct descriptor_xd *dsc, void **zone);
EXPORT extern int MdsMsg(int sts, char fmt[], ...);
EXPORT extern void MdsPk(char *nbits, int *nitems, int pack[], int items[], int *bit);
EXPORT extern int  MdsSetCurrentShotId(char *experiment,int shot);
EXPORT extern int MdsSerializeDscIn(char *in, struct descriptor_xd *out);
EXPORT extern int MdsSerializeDscOutZ(struct descriptor *in,struct descriptor_xd *out,
    int (*fixupNid)(), void *fixupNidArg, int (*fixupPath)(),
    void *fixupPathArg, int compress, int *compressible,
    unsigned int *length, unsigned int *reclen,  unsigned char *dtype,
    unsigned char *class, int  altbuflen, void *altbuf, int *data_in_altbuf);
EXPORT extern int MdsSerializeDscOut(struct descriptor *in,struct descriptor_xd *out);
EXPORT extern void MdsUnpk(char *nbits, int *nitems, int pack[], int items[], int *bit);
EXPORT extern int MdsXpand(int *nitems, struct descriptor_a *pack, struct descriptor_a *items, int *bit);
EXPORT extern int MDSEventAst(char *eventnam, void (*astadr)(void *,int,char *), void *astprm, int *eventid);
EXPORT extern int MDSEventCan(int eventid);
EXPORT extern int MDSEvent(char *evname, int num_bytes, char *data);
EXPORT extern int MDSUdpEventAst(char *eventName, void (*astadr)(void *,int,char *), void *astprm, int *eventid);
EXPORT extern int MDSUdpEvent(char *name, int bufLen, char *buf);	
EXPORT extern int MDSUdpEventCan(int id);
EXPORT extern int MDSWfevent(char *evname, int buflen, char *data, int *datlen);
EXPORT extern int MDSWfeventTimed(char *evname, int buflen, char *data, int *datlen, int timeout);
EXPORT extern char  *MdsDtypeString( int  id );
EXPORT extern char  *MdsClassString( int  id );
EXPORT extern int   MDSprintf( char *fmt , ... );
EXPORT extern char  *MdsUsageString( int  id );
EXPORT extern char *TranslateLogical(char *name);
EXPORT extern void TranslateLogicalFree(char *value);
EXPORT extern int TranslateLogicalXd(struct descriptor *in, struct descriptor_xd *out);
EXPORT extern char *MdsRelease();
EXPORT extern void MdsFloatToTime(double floatTime, _int64u *outTime);
EXPORT extern void MdsFloatToDelta(double floatTime, _int64u *outTime);
EXPORT extern void MdsTimeToFloat(_int64u inTime, float *outFloat);
EXPORT extern int MdsPutEnv(char *cmd);

#ifdef HAVE_WINDOWS_H
typedef int pthread_key_t;
typedef void *pthread_t;
EXPORT extern char *index(char *str, char c);
EXPORT extern void pthread_once(int *one_time,void (*key_alloc));
EXPORT extern void *pthread_getspecific(pthread_key_t buffer_key);
EXPORT extern void pthread_setspecific(pthread_key_t  buffer_key, void *p);
EXPORT extern void pthread_key_create(pthread_key_t *buffer_key,void *d2);
EXPORT extern int pthread_mutex_lock(void **mutex);
EXPORT extern int pthread_mutex_unlock(void **mutex);
EXPORT extern int pthread_mutex_init(void **mutex, void *);
EXPORT extern void pthread_cleanup_pop();
EXPORT extern void pthread_cleanup_push();
EXPORT extern int pthread_cond_init(void **cond,void *def);
EXPORT extern int pthread_cond_wait(void **cond, void **mutex);
EXPORT extern int pthread_create(pthread_t *thread, void *dummy, void *(*rtn)(void *), void *rtn_param);
EXPORT extern void pthread_lock_global_np();
EXPORT extern void pthread_unlock_global_np();
EXPORT extern int pthread_cond_signal(void *cond);
EXPORT extern int pthread_cond_timedwait(void **cond, void **mutex, int msec);
EXPORT extern int pthread_cond_destroy(void **cond);
EXPORT extern int pthread_mutex_destroy(void **mutex);
EXPORT extern void pthread_cancel(void *thread);
#endif
