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
#pragma extern_model save
#pragma extern_model globalvalue
extern MdsCOMPRESSIBLE;
#pragma extern_model restore
#else
#define MdsCOMPRESSIBLE 3
#endif

extern int MdsCmprs(int *nitems, struct descriptor_a *items, struct descriptor_a *pack, int *bit);
extern int MdsCompress(struct descriptor *cimage, struct descriptor *centry, struct descriptor *in, struct descriptor_xd *out);
extern int MdsCopyDxXd(struct descriptor *in, struct descriptor_xd *out);
extern int MdsCopyDxXdZ(struct descriptor *in, struct descriptor_xd *out, void **zone, int (*fixup_nid) (), void *fixup_nid_arg,
                        int (*fixup_path) (), void *fixup_path_arg);
extern char *MdsDescrToCstring(struct descriptor *);
extern void MdsFree(void *);
extern int MdsGet1Dx(unsigned int *len, unsigned char *dtype, struct descriptor_xd *dsc, void **zone);
extern int MdsGet1DxA(struct descriptor_a * in, unsigned short *len, unsigned char *dtype, struct descriptor_xd *out);
extern int MdsGet1DxS(unsigned short *len, unsigned char *dtype, struct descriptor_xd *out);
extern int MdsGetCurrentShotId(char *experiment);
extern char *MdsGetMsg(int sts);
extern int MdsFree1Dx(struct descriptor_xd *dsc, void **zone);
extern int MdsMsg(int sts, char fmt[], ...);
extern void MdsPk(char *nbits, int *nitems, int pack[], int items[], int *bit);
extern int  MdsSetCurrentShotId(char *experiment,int shot);
extern int MdsSerializeDscIn(char *in, struct descriptor_xd *out);
extern int MdsSerializeDscOutZ(struct descriptor *in,struct descriptor_xd *out,
    int (*fixupNid)(), void *fixupNidArg, int (*fixupPath)(),
    void *fixupPathArg, int compress, int *compressible,
    unsigned int *length, unsigned int *reclen,  unsigned char *dtype,
    unsigned char *class, int  altbuflen, void *altbuf, int *data_in_altbuf);
extern int MdsSerializeDscOut(struct descriptor *in,struct descriptor_xd *out);
extern void MdsUnpk(char *nbits, int *nitems, int pack[], int items[], int *bit);
extern int MdsXpand(int *nitems, struct descriptor_a *pack, struct descriptor_a *items, int *bit);
