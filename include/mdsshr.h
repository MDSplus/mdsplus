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

extern char *MdsDescrToCstring(struct descriptor *);

extern void MdsFree(void *);

extern int MdsGet1Dx();
extern int MdsGet1DxS();
extern int MdsGet1DxA();

extern int MdsGetCurrentShotid();
extern int MdsFree1Dx();
extern int MdsXpand();
extern int MdsCopyDxXd();
extern int MdsCmprs();
extern int MdsPk();
extern int MdsUnpk();
extern int MdsCompress();
