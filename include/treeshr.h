#ifndef __TREESHR
#define __TREESHR
#ifdef HAVE_VXWORKS_H
//typedef long long  _int64;
#else
#include <config.h>
#endif
#ifdef HAVE_WINDOWS_H
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif
#include <mdstypes.h>
extern int treeshr_errno;
extern int TREE_BLOCKID;

#ifdef __VMS
#pragma extern_model save
#pragma extern_model globalvalue
#define TreeALREADY_OFF TREE$_ALREADY_OFF
extern TreeALREADY_OFF;
#define TreeALREADY_ON TREE$_ALREADY_ON
extern TreeALREADY_ON;
#define TreeALREADY_OPEN TREE$_ALREADY_OPEN
extern TreeALREADY_OPEN;
#define TreeALREADY_THERE TREE$_ALREADY_THERE
extern TreeALREADY_THERE;
#define TreeBADRECORD TREE$_BADRECORD
extern TreeBADRECORD;
#define TreeBLOCKID TREE$_BLOCKID
extern TreeBLOCKID;
#define TreeBOTH_OFF TREE$_BOTH_OFF
extern TreeBOTH_OFF;
#define TreeBUFFEROVF TREE$_BUFFEROVF
extern TreeBUFFEROVF;
#define TreeCONGLOMFULL TREE$_CONGLOMFULL
extern TreeCONGLOMFULL;
#define TreeCONGLOM_NOT_FULL TREE$_CONGLOM_NOT_FULL
extern TreeCONGLOM_NOT_FULL;
#define TreeCONTINUING TREE$_CONTINUING
extern TreeCONTINUING;
#define TreeDUPTAG TREE$_DUPTAG
extern TreeDUPTAG;
#define TreeEDITTING TREE$_EDITTING
extern TreeEDITTING;
#define TreeILLEGAL_ITEM TREE$_ILLEGAL_ITEM
extern TreeILLEGAL_ITEM;
#define TreeILLPAGCNT TREE$_ILLPAGCNT
extern TreeILLPAGCNT;
#define TreeINVDFFCLASS TREE$_INVDFFCLASS
extern TreeINVDFFCLASS;
#define TreeINVDTPUSG TREE$_INVDTPUSG
extern TreeINVDTPUSG;
#define TreeINVPATH TREE$_INVPATH
extern TreeINVPATH;
#define TreeINVRECTYP TREE$_INVRECTYP
extern TreeINVRECTYP;
#define TreeINVTREE TREE$_INVTREE
extern TreeINVTREE;
#define TreeMAXOPENEDIT TREE$_MAXOPENEDIT
extern TreeMAXOPENEDIT;
#define TreeNEW TREE$_NEW
extern TreeNEW;
#define TreeNMN TREE$_NMN
extern TreeNMN;
#define TreeNMT TREE$_NMT
extern TreeNMT;
#define TreeNNF TREE$_NNF
extern TreeNNF;
#define TreeNODATA TREE$_NODATA
extern TreeNODATA;
#define TreeNODNAMLEN TREE$_NODNAMLEN
extern TreeNODNAMLEN;
#define TreeNOEDIT TREE$_NOEDIT
extern TreeNOEDIT;
#define TreeNOLOG TREE$_NOLOG
extern TreeNOLOG;
#define TreeNOMETHOD TREE$_NOMETHOD
extern TreeNOMETHOD;
#define TreeNOOVERWRITE TREE$_NOOVERWRITE
extern TreeNOOVERWRITE;
#define TreeNORMAL TREE$_NORMAL
extern TreeNORMAL;
#define TreeNOTALLSUBS TREE$_NOTALLSUBS
extern TreeNOTALLSUBS;
#define TreeNOTCHILDLESS TREE$_NOTCHILDLESS
extern TreeNOTCHILDLESS;
#define TreeNOT_IN_LIST TREE$_NOT_IN_LIST
extern TreeNOT_IN_LIST;
#define TreeNOTMEMBERLESS TREE$_NOTMEMBERLESS
extern TreeNOTMEMBERLESS;
#define TreeNOTOPEN TREE$_NOTOPEN
extern TreeNOTOPEN;
#define TreeNOTSON TREE$_NOTSON
extern TreeNOTSON;
#define TreeNOT_CONGLOM TREE$_NOT_CONGLOM
extern TreeNOT_CONGLOM;
#define TreeNOT_OPEN TREE$_NOT_OPEN
extern TreeNOT_OPEN;
#define TreeNOWRITEMODEL TREE$_NOWRITEMODEL
extern TreeNOWRITEMODEL;
#define TreeNOWRITESHOT TREE$_NOWRITESHOT
extern TreeNOWRITESHOT;
#define TreeNO_CONTEXT TREE$_NO_CONTEXT
extern TreeNO_CONTEXT;
#define TreeOFF TREE$_OFF
extern TreeOFF;
#define TreeON TREE$_ON
extern TreeON;
#define TreeOPEN TREE$_OPEN
extern TreeOPEN;
#define TreeOPEN_EDIT TREE$_OPEN_EDIT
extern TreeOPEN_EDIT;
#define TreePARENT_OFF TREE$_PARENT_OFF
extern TreePARENT_OFF;
#define TreeREADERR TREE$_READERR
extern TreeREADERR;
#define TreeREADONLY TREE$_READONLY
extern TreeREADONLY;
#define TreeRESOLVED TREE$_RESOLVED
extern TreeRESOLVED;
#define TreeTAGNAMLEN TREE$_TAGNAMLEN
extern TreeTAGNAMLEN;
#define TreeTNF TREE$_TNF
extern TreeTNF;
#define TreeTREENF TREE$_TREENF
extern TreeTREENF;
#define TreeUNRESOLVED TREE$_UNRESOLVED
extern TreeUNRESOLVED;
#define TreeUNSPRTCLASS TREE$_UNSPRTCLASS
extern TreeUNSPRTCLASS;
#define TreeUNSUPARRDTYPE TREE$_UNSUPARRDTYPE
extern TreeUNSUPARRDTYPE;
#define TreeWRITEFIRST TREE$_WRITEFIRST
extern TreeWRITEFIRST;
#define TreeSUCCESS TREE$_SUCCESS
extern TreeSUCCESS;
#define TreeFAILURE TREE$_FAILURE
extern TreeFAILURE;
#define TreeFILE_NOT_FOUND TREE$_FILE_NOT_FOUND
extern TreeFILE_NOT_FOUND;
#define TreeCANCEL TREE$_CANCEL
extern TreeCANCEL;
#pragma extern_model restore

#else

#define TreeALREADY_OFF 0xfd1802b
#define TreeALREADY_ON 0xfd18033
#define TreeALREADY_OPEN 0xfd1803b
#define TreeALREADY_THERE 0xfd18088
#define TreeBADRECORD 0xfd180ba
#define TreeBLOCKID 0x3ade68b1
#define TreeBOTH_OFF 0xfd18098
#define TreeBUFFEROVF 0xfd18112
#define TreeCONGLOMFULL 0xfd18122
#define TreeCONGLOM_NOT_FULL 0xfd1812a
#define TreeCONTINUING 0xfd18963
#define TreeDUPTAG 0xfd180ca
#define TreeEDITTING 0xfd18192
#define TreeILLEGAL_ITEM 0xfd1810a
#define TreeILLPAGCNT 0xfd180d2
#define TreeINVDFFCLASS 0xfd1813a
#define TreeINVDTPUSG 0xfd1818a
#define TreeINVPATH 0xfd18102
#define TreeINVRECTYP 0xfd18142
#define TreeINVTREE 0xfd180c2
#define TreeMAXOPENEDIT 0xfd180da
#define TreeNEW 0xfd1801b
#define TreeNMN 0xfd18060
#define TreeNMT 0xfd18068
#define TreeNNF 0xfd18070
#define TreeNODATA 0xfd180e2
#define TreeNODNAMLEN 0xfd1814a
#define TreeNOEDIT 0xfd180f2
#define TreeNOLOG 0xfd181aa
#define TreeNOMETHOD 0xfd180b0
#define TreeNOOVERWRITE 0xfd18182
#define TreeNORMAL 0xfd18009
#define TreeNOTALLSUBS 0xfd18023
#define TreeNOTCHILDLESS 0xfd180fa
#define TreeNOT_IN_LIST 0xfd181c2
#define TreeNOTMEMBERLESS 0xfd18172
#define TreeNOTOPEN 0xfd180ea
#define TreeNOTSON 0xfd1817a
#define TreeNOT_CONGLOM 0xfd18162
#define TreeNOT_OPEN 0xfd180a8
#define TreeNOWRITEMODEL 0xfd1819a
#define TreeNOWRITESHOT 0xfd181a2
#define TreeNO_CONTEXT 0xfd18043
#define TreeOFF 0xfd180a0
#define TreeON 0xfd1804b
#define TreeOPEN 0xfd18053
#define TreeOPEN_EDIT 0xfd1805b
#define TreePARENT_OFF 0xfd18090
#define TreeREADERR 0xfd181ba
#define TreeREADONLY 0xfd181b2
#define TreeRESOLVED 0xfd18011
#define TreeSUCCESS 0xfd18641
#define TreeTAGNAMLEN 0xfd18152
#define TreeTNF 0xfd18078
#define TreeTREENF 0xfd18080
#define TreeUNRESOLVED 0xfd18132
#define TreeUNSPRTCLASS 0xfd1811a
#define TreeUNSUPARRDTYPE 0xfd1816a
#define TreeWRITEFIRST 0xfd1815a
#define TreeFAILURE 0xfd18fa2
#define TreeLOCK_FAILURE 0xfd18fb2
#define TreeFILE_NOT_FOUND 0xfd18faa
#define TreeCANCEL 0xfd18c80
#define TreeNOSEGMENTS  0xfd18fba
#define TreeINVDTYPE 0xfd18fc2
#define TreeINVSHAPE 0xfd18fca

#endif

#ifndef MDSDESCRIP_H_DEFINED
struct descriptor;
struct descriptor_a;
struct descriptor_r;
struct descriptor_xd;
#endif

#ifndef DBIDEF_H
struct dbi_itm;
#endif

#ifndef NCIDEF_H
struct nci_itm;
#endif

#ifdef CREATE_STS_TEXT
#include        "facility_list.h"

static struct stsText  treeshr_stsText[] = {
    STS_TEXT(TreeALREADY_OFF,"Node is already OFF")
   ,STS_TEXT(TreeALREADY_ON,"Node is already ON")
   ,STS_TEXT(TreeALREADY_OPEN,"Tree is already OPEN")
   ,STS_TEXT(TreeALREADY_THERE,"Node is already in the tree")
   ,STS_TEXT(TreeBADRECORD,"Data corrupted: cannot read record")
   ,STS_TEXT(TreeBLOCKID,"xxx")
   ,STS_TEXT(TreeBOTH_OFF,"Both this node and its parent are off")
   ,STS_TEXT(TreeBUFFEROVF,"Output buffer overflow")
   ,STS_TEXT(TreeCONGLOMFULL,"Current conglomerate is full")
   ,STS_TEXT(TreeCONGLOM_NOT_FULL,"Current conglomerate is not yet full")
   ,STS_TEXT(TreeCONTINUING,"Operation continuing: note following error")
   ,STS_TEXT(TreeDUPTAG,"Tag name already in use")
   ,STS_TEXT(TreeEDITTING,"Tree file open for edit: operation not permitted")
   ,STS_TEXT(TreeILLEGAL_ITEM,"Invalid item code or part number specified")
   ,STS_TEXT(TreeILLPAGCNT,"Illegal page count, error mapping tree file")
   ,STS_TEXT(TreeINVDFFCLASS,"Invalid data fmt: only CLASS_S can have data in NCI")
   ,STS_TEXT(TreeINVDTPUSG,"Attempt to store datatype which conflicts\n\
			with the designated usage of this node")
   ,STS_TEXT(TreeINVPATH,"Invalid tree pathname specified")
   ,STS_TEXT(TreeINVRECTYP,"Record type invalid for requested operation")
   ,STS_TEXT(TreeINVTREE,"Invalid tree identification structure")
   ,STS_TEXT(TreeMAXOPENEDIT,"Too many files open for edit")
   ,STS_TEXT(TreeNEW,"New tree created")
   ,STS_TEXT(TreeNMN,"No More Nodes")
   ,STS_TEXT(TreeNMT,"No More Tags")
   ,STS_TEXT(TreeNNF,"Node Not Found")
   ,STS_TEXT(TreeNODATA,"No data available for this node")
   ,STS_TEXT(TreeNODNAMLEN,"Node name too long (12 chars max)")
   ,STS_TEXT(TreeNOEDIT,"Tree file is not open for edit")
   ,STS_TEXT(TreeNOLOG,"Experiment pathname (xxx_path) not defined")
   ,STS_TEXT(TreeNOMETHOD,"Method not available for this object")
   ,STS_TEXT(TreeNOOVERWRITE,"Write-once node: overwrite not permitted")
   ,STS_TEXT(TreeNORMAL,"Normal successful completion")
   ,STS_TEXT(TreeNOTALLSUBS,"Main tree opened but not all subtrees found/or connected")
   ,STS_TEXT(TreeNOTCHILDLESS,"Node must be childless to become subtree reference")
   ,STS_TEXT(TreeNOT_IN_LIST,"Tree being opened was not in the list")
   ,STS_TEXT(TreeNOTMEMBERLESS,"Subtree reference can not have members")
   ,STS_TEXT(TreeNOTOPEN,"No tree file currently open")
   ,STS_TEXT(TreeNOTSON,"Subtree reference cannot be a member")
   ,STS_TEXT(TreeNOT_CONGLOM,"Head node of conglomerate does not contain a DTYPE_CONGLOM record")
   ,STS_TEXT(TreeNOT_OPEN,"Tree not currently open")
   ,STS_TEXT(TreeNOWRITEMODEL,"Data for this node can not be written into the MODEL file")
   ,STS_TEXT(TreeNOWRITESHOT,"Data for this node can not be written into the SHOT file")
   ,STS_TEXT(TreeNO_CONTEXT,"There is no active search to end")
   ,STS_TEXT(TreeOFF,"Node is OFF")
   ,STS_TEXT(TreeON,"Node is ON")
   ,STS_TEXT(TreeOPEN,"Tree is OPEN (no edit)")
   ,STS_TEXT(TreeOPEN_EDIT,"Tree is OPEN for edit")
   ,STS_TEXT(TreePARENT_OFF,"Parent of this node is OFF")
   ,STS_TEXT(TreeREADERR,"Error reading record for node")
   ,STS_TEXT(TreeREADONLY,"Tree was opened with readonly access")
   ,STS_TEXT(TreeRESOLVED,"Indirect reference successfully resolved")
   ,STS_TEXT(TreeSUCCESS,"Operation successful")
   ,STS_TEXT(TreeTAGNAMLEN,"Tagname too long (max 24 chars)")
   ,STS_TEXT(TreeTNF,"Tag Not Found")
   ,STS_TEXT(TreeTREENF,"Tree Not Found")
   ,STS_TEXT(TreeUNRESOLVED,"Not an indirect node reference: No action taken")
   ,STS_TEXT(TreeUNSPRTCLASS,"Unsupported descriptor class")
   ,STS_TEXT(TreeUNSUPARRDTYPE,"Complex data types not supported as members of arrays")
   ,STS_TEXT(TreeWRITEFIRST,"Tree has been modified:  write or quit first")
   ,STS_TEXT(TreeFAILURE,"Operation NOT successful")
   ,STS_TEXT(TreeLOCK_FAILURE,"Error locking file, perhaps NFSLOCKING not enabled on this system")
   ,STS_TEXT(TreeFILE_NOT_FOUND,"File or Directory Not Found")
   ,STS_TEXT(TreeCANCEL,"User canceled operation")
    ,STS_TEXT(TreeNOSEGMENTS,"No segments exist in this node")
    ,STS_TEXT(TreeINVDTYPE,"Invalid datatype for data segment")
    ,STS_TEXT(TreeINVSHAPE,"Invalid shape for this data segment")
   };
#endif

/******************* Prototypes for TREESHR routines *********************/

#ifdef _MDS_NO_PROTO

extern EXPORT char* TreeAbsPath();           /********** Use TreeFree(result) *****/
extern EXPORT char* _TreeAbsPath();          /********** Use TreeFree(result) *****/
extern EXPORT int TreeAddConglom();
extern EXPORT int _TreeAddConglom();
/* obsolete */	extern int TreeAddMethodImage();
extern EXPORT int TreeAddNode();
extern EXPORT int _TreeAddNode();
extern EXPORT int TreeAddTag();
extern EXPORT int _TreeAddTag();
extern EXPORT int TreeCleanDatafile();
extern EXPORT int _TreeCleanDatafile();
/* obsolete */	extern EXPORT int TreeClearMethodImageS();
extern EXPORT int TreeClose();
extern EXPORT int _TreeClose();
extern EXPORT int TreeCompressDatafile();
extern EXPORT int _TreeCompressDatafile();
extern EXPORT int TreeCreatePulseFile();
extern EXPORT int _TreeCreatePulseFile();
extern EXPORT int  TreeCreateTreeFiles();
extern EXPORT void TreeDeleteNodeExecute();
extern EXPORT int TreeDeleteNodeGetNid();
extern EXPORT int TreeDeleteNodeInitialize();
extern EXPORT int TreeDeletePulseFile();
extern EXPORT int _TreeDeletePulseFile();
extern EXPORT int TreeDoMethod();
extern EXPORT int _TreeDoMethod();
/* obsolete */	extern EXPORT int TreeDoMethodHandler();
extern EXPORT int TreeEditing();
extern EXPORT int _TreeEditing();
extern EXPORT int TreeEndConglomerate();
extern EXPORT int _TreeEndConglomerate();
extern EXPORT int TreeFindNode();
extern EXPORT int _TreeFindNode();
extern EXPORT int TreeFindNodeEnd();
extern EXPORT int _TreeFindNodeEnd();
extern EXPORT char *TreeFindNodeTags();   /********** Use TreeFree(result) *****/
extern EXPORT char *_TreeFindNodeTags();  /********** Use TreeFree(result) *****/
extern EXPORT int TreeFindNodeWild();
extern EXPORT int _TreeFindNodeWild();
extern EXPORT void TreeFindTagEnd();
extern EXPORT char *TreeFindTagWild();
extern EXPORT char *_TreeFindTagWild();
extern EXPORT int TreeFlushOff();
extern EXPORT int TreeFlushReset();
extern EXPORT int TreeFree();
extern EXPORT int TreeGetDbi();
extern EXPORT int _TreeGetDbi();
extern EXPORT int TreeGetNci();
extern EXPORT int _TreeGetNci();
extern EXPORT int TreeGetDefaultNid();
extern EXPORT int _TreeGetDefaultNid();
extern EXPORT char *TreeGetMinimumPath();  /********** Use TreeFree(result) *****/
extern EXPORT char *_TreeGetMinimumPath(); /********** Use TreeFree(result) *****/
extern EXPORT char *TreeGetPath(); /********** Use TreeFree(result) *****/
extern EXPORT char *_TreeGetPath();/********** Use TreeFree(result) *****/
extern EXPORT int TreeGetRecord();
extern EXPORT int _TreeGetRecord();
extern EXPORT int TreeGetStackSize();
extern EXPORT int _TreeGetStackSize();
extern EXPORT int TreeIsOn();
extern EXPORT int _TreeIsOn();
extern EXPORT int TreeIsOpen();
extern EXPORT int _TreeIsOpen();
	extern EXPORT int TreeLink();
	extern EXPORT int TreeMarkIncludes();
extern EXPORT int TreeOpen();
extern EXPORT int _TreeOpen();
extern EXPORT int TreeOpenEdit();
extern EXPORT int _TreeOpenEdit();
extern EXPORT int TreeOpenNew();
extern EXPORT int _TreeOpenNew();
extern EXPORT int TreePutRecord();
extern EXPORT int _TreePutRecord();
extern EXPORT int TreeQuitTree();
extern EXPORT int _TreeQuitTree();
extern EXPORT int TreeRemoveNodesTags();
extern EXPORT int _TreeRemoveNodesTags();
extern EXPORT int TreeRemoveTag();
extern EXPORT int _TreeRemoveTag();
extern EXPORT int TreeRenameNode();
extern EXPORT int _TreeRenameNode();
extern EXPORT void TreeRestoreContext();
extern EXPORT void _TreeRestoreContext();
	extern EXPORT int TreeRundownTree();
extern EXPORT void *TreeSaveContext();
extern EXPORT void *_TreeSaveContext();
extern EXPORT int TreeSetNci();
extern EXPORT int _TreeSetNci();
extern EXPORT int TreeSetDefault();
extern EXPORT int _TreeSetDefault();
extern EXPORT int TreeSetDefaultNid();
extern EXPORT int _TreeSetDefaultNid();
extern EXPORT int TreeSetNoSubtree();
extern EXPORT int _TreeSetNoSubtree();
extern EXPORT int TreeSetStackSize();
extern EXPORT int _TreeSetStackSize();
extern EXPORT int TreeSetSubtree();
extern EXPORT int _TreeSetSubtree();
extern EXPORT int TreeSetUsage();
extern EXPORT int _TreeSetUsage();
extern EXPORT int TreeStartConglomerate();
extern EXPORT int _TreeStartConglomerate();
extern EXPORT void *TreeSwitchDbid();
extern EXPORT int TreeTurnOff();
extern EXPORT int _TreeTurnOff();
extern EXPORT int TreeTurnOn();
extern EXPORT int _TreeTurnOn();
extern EXPORT int TreeVerify();
extern EXPORT int _TreeVerify();
	extern EXPORT int TreeWait();
extern EXPORT int TreeWriteTree();
extern EXPORT int _TreeWriteTree();
extern EXPORT int TreeGetCurrentShotId();
extern EXPORT int TreeSetCurrentShotId();
//Segments
extern EXPORT int TreeBeginSegment();
extern EXPORT int _TreeBeginSegment();
extern EXPORT int TreePutSegment();
extern EXPORT int _TreePutSegment();
extern EXPORT int TreeUpdateSegment();
extern EXPORT int _TreeUpdateSegment();
extern EXPORT int TreeBeginTimestampedSegment();
extern EXPORT int _TreeBeginTimestampedSegment();
extern EXPORT int TreePutTimestampedSegment();
extern EXPORT int _TreePutTimestampedSegment();
extern EXPORT int TreePutRow();
extern EXPORT int _TreePutRow();
extern EXPORT int TreeSetTimeContext();
extern EXPORT int _TreeSetTimeContext();
extern EXPORT int TreeGetNumSegments();
extern EXPORT int TreeGetSegmentLimits();
extern EXPORT int _TreeGetSegmentLimits();
extern EXPORT int TreeGetSegment();
extern EXPORT int _TreeGetSegment();
extern EXPORT int TreeGetSegmentInfo();
extern EXPORT int _TreeGetSegmentInfo();

extern EXPORT int TreeGetXNci();
extern EXPORT int _TreeGetXNci();
extern EXPORT int TreeSetXNci();
extern EXPORT int _TreeSetXNci();
extern EXPORT int TreeSetViewDate();
extern EXPORT int _TreeSetViewDate();
extern EXPORT int TreeSetCurrentShotId();
extern EXPORT int TreeGetCurrentShotId();
extern EXPORT int TreeSetDbiItm();


#else

extern EXPORT char *TreeAbsPath(char *in);             /********** Use TreeFree(result) *****/
extern EXPORT char *_TreeAbsPath( void *dbid, char *in);  /********** Use TreeFree(result) *****/
extern EXPORT int TreeAddConglom(char *path, char *congtype, int *nid);
extern EXPORT int _TreeAddConglom(void *dbid, char *path, char *congtype, int *nid);
extern EXPORT int TreeAddNode(char *name, int *nid_ret, char usage);
extern EXPORT int _TreeAddNode(void *dbid, char *name, int *nid_ret, char usage);
extern EXPORT int TreeAddTag(int nid, char *tagnam);
extern EXPORT int _TreeAddTag(void *dbid, int nid, char *tagnam);
extern EXPORT int TreeCleanDatafile(char *tree, int shot);
extern EXPORT int _TreeCleanDatafile(void **dbid, char *tree, int shot);
extern EXPORT int TreeClose(char *tree, int shot);
extern EXPORT int _TreeClose(void **dbid, char *tree, int shot);
extern EXPORT int TreeCompressDatafile(char *tree, int shot);
extern EXPORT int _TreeCompressDatafile(void **dbid, char *tree, int shot);
extern EXPORT int TreeCreatePulseFile( int shot, int numnids, int *nids);
extern EXPORT int _TreeCreatePulseFile(void *dbid, int shot, int numnids, int *nids);
extern EXPORT int  TreeCreateTreeFiles(char *tree, int shot, int source_shot);
extern EXPORT void TreeDeleteNodeExecute(void);
extern EXPORT void _TreeDeleteNodeExecute(void *dbid);
extern EXPORT int TreeDeleteNodeGetNid(int *nid);
extern EXPORT int _TreeDeleteNodeGetNid(void *dbid, int *nid);
extern EXPORT int TreeDeleteNodeInitialize(int nid, int *count, int reset);
extern EXPORT int _TreeDeleteNodeInitialize(void *dbid, int nid, int *count, int reset);
extern EXPORT int TreeDeletePulseFile(int shotid, int all_versions);
extern EXPORT int _TreeDeletePulseFile(void *dbid, int shotid, int all_versions);
extern EXPORT int TreeDoMethod(struct descriptor *nid, struct descriptor *method, ...);
extern EXPORT int _TreeDoMethod(void *dbid, struct descriptor *nid, struct descriptor *method, ...);
extern EXPORT int TreeDoMethod_HANDLER(int *sig_args, int *mech_args);
extern EXPORT int TreeEditing();
extern EXPORT int _TreeEditing(void *dbid);
extern EXPORT int TreeEndConglomerate();
extern EXPORT int _TreeEndConglomerate(void *dbid);
extern EXPORT int TreeFindNode(char *path, int *nid);
extern EXPORT int _TreeFindNode(void *dbid, char *path, int *nid);
extern EXPORT int TreeFindNodeEnd(void **ctx);
extern EXPORT int _TreeFindNodeEnd(void *dbid, void **ctx);
extern EXPORT char *TreeFindNodeTags(int nid, void **ctx);   /********** Use TreeFree(result) *****/
extern EXPORT char *_TreeFindNodeTags(void *dbid, int nid, void **ctx);  /********** Use TreeFree(result) *****/
extern EXPORT int TreeFindNodeWild(char *path, int *nid, void **ctx, int usage_mask);
extern EXPORT int _TreeFindNodeWild(void *dbid, char *path, int *nid, void **ctx, int usage_mask);
extern EXPORT void TreeFindTagEnd(void **ctx);
extern EXPORT char *TreeFindTagWild(char *wild, int *nidout, void **search_ctx);
extern EXPORT char *_TreeFindTagWild(void *dbid, char *wild, int *nidout, void **ctx);
extern EXPORT int TreeFlushOff(int nid);
extern EXPORT int _TreeFlushOff(void *dbid, int nid);
extern EXPORT int TreeFlushReset(int nid);
extern EXPORT int _TreeFlushReset(void *dbid, int nid);
extern EXPORT void TreeFree(void *);
extern EXPORT int TreeGetDbi(struct dbi_itm *itmlst);
extern EXPORT int _TreeGetDbi(void *dbid, struct dbi_itm *itmlst);
extern EXPORT int TreeGetNci(int nid, struct nci_itm *itmlst);
extern EXPORT int _TreeGetNci(void *dbid, int nid, struct nci_itm *itmlst);
extern EXPORT int TreeGetDefaultNid(int *nid);
extern EXPORT int _TreeGetDefaultNid(void *dbid, int *nid);
extern EXPORT char *TreeGetMinimumPath(int *def_nid, int nid); /********** Use TreeFree(result) *****/
extern EXPORT char *_TreeGetMinimumPath(void *dbid, int *def_nid, int nid); /********** Use TreeFree(result) *****/
extern EXPORT char *TreeGetPath(int nid); /********** Use TreeFree(result) *****/
extern EXPORT char *_TreeGetPath(void *dbid, int nid); /********** Use TreeFree(result) *****/
extern EXPORT int TreeGetRecord(int nid, struct descriptor_xd *dsc_ptr);
extern EXPORT int _TreeGetRecord(void *dbid, int nid, struct descriptor_xd *dsc_ptr);
extern EXPORT int TreeGetStackSize();
extern EXPORT int _TreeGetStackSize(void *dbid);
extern EXPORT int TreeGetViewDate(_int64 *date);
extern EXPORT int TreeIsOn(int nid);
extern EXPORT int _TreeIsOn(void *dbid, int nid);
extern EXPORT int TreeIsOpen();
extern EXPORT int _TreeIsOpen(void *dbid);
extern EXPORT int TreeLink(struct descriptor *intree, struct descriptor *outtree);
extern EXPORT int TreeMarkIncludes();
extern EXPORT int TreeNodePresent(int *nid);
extern EXPORT int TreeOpen(char *tree, int shot, int flags);
extern EXPORT int _TreeOpen(void **dbid,char *tree, int shot, int flags);
extern EXPORT int TreeOpenEdit(char *tree, int shot);
extern EXPORT int _TreeOpenEdit(void **dbid, char *tree, int shot);
extern EXPORT int TreeOpenNew(char *tree, int shot);
extern EXPORT int _TreeOpenNew(void **dbid, char *tree, int shot);
extern EXPORT int TreePutRecord(int nid, struct descriptor *descriptor_ptr, int utility_update);
extern EXPORT int _TreePutRecord(void *dbid, int nid, struct descriptor *descriptor, int utility_update);
extern EXPORT int TreeQuitTree(char *tree, int shot);
extern EXPORT int _TreeQuitTree(void **dbid, char *tree, int shot);
extern EXPORT int TreeRemoveNodesTags(int nid);
extern EXPORT int _TreeRemoveNodesTags(void *dbid, int nid);
extern EXPORT int TreeRemoveTag(char *tagnam);
extern EXPORT int _TreeRemoveTag(void *dbid, char *tagnam);
extern EXPORT int TreeRenameNode(int nid, char *newnamedsc_ptr);
extern EXPORT int _TreeRenameNode(void *dbid, int nid, char *newname);
extern EXPORT void TreeRestoreContext(void *ctx);
extern EXPORT void _TreeRestoreContext(void **dbid, void *ctx);
extern EXPORT int TreeRundownTree(struct descriptor *treenam, int *shotid, int single_tree, int force_exit);
extern EXPORT void *TreeSaveContext();
extern EXPORT void *_TreeSaveContext(void *dbid);
extern EXPORT int TreeSetDbi(struct dbi_itm *itmlst);
extern EXPORT int _TreeSetDbi(void *dbid, struct dbi_itm *itmlst);
extern EXPORT int TreeSetNci(int nid, struct nci_itm *itmlst);
extern EXPORT int _TreeSetNci(void *dbid, int nid, struct nci_itm *itmlst);
extern EXPORT int TreeSetDefault(char *path, int *nid);
extern EXPORT int _TreeSetDefault(void *dbid, char *path, int *nid);
extern EXPORT int TreeSetDefaultNid(int nid);
extern EXPORT int _TreeSetDefaultNid(void *dbid, int nid);
extern EXPORT int TreeSetNoSubtree(int nid);
extern EXPORT int _TreeSetNoSubtree(void *dbid, int nid);
extern EXPORT int TreeSetStackSize(int new_size);
extern EXPORT int _TreeSetStackSize(void **dbid, int new_size);
extern EXPORT int TreeSetSubtree(int nid);
extern EXPORT int _TreeSetSubtree(void *dbid, int nid);
extern EXPORT int TreeSetUsage(int nid, unsigned char usage);
extern EXPORT int _TreeSetUsage(void *dbid, int nid, unsigned char usage);
extern EXPORT int TreeStartConglomerate(int numnodes);
extern EXPORT int _TreeStartConglomerate(void *dbid, int numnodes);
extern EXPORT void *TreeSwitchDbid(void *dbid);
extern EXPORT int TreeTurnOff(int nid);
extern EXPORT int _TreeTurnOff(void *dbid, int nid);
extern EXPORT int TreeTurnOn(int nid);
extern EXPORT int _TreeTurnOn(void *dbid, int nid);
extern EXPORT int TreeVerify();
extern EXPORT int _TreeVerify(void *dbid);
extern EXPORT int TreeWait();
extern EXPORT int TreeWriteTree(char *tree, int shot);
extern EXPORT int _TreeWriteTree(void **dbid, char *tree, int shot);
extern EXPORT int TreeGetCurrentShotId(char *experiment);
extern EXPORT int TreeSetCurrentShotId(char *experiment, int shot);

//Segments
extern EXPORT int TreeBeginSegment(int nid, struct descriptor *start, struct descriptor *end, 
							struct descriptor *dim, struct descriptor_a *initialData, int idx);
extern EXPORT int _TreeBeginSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end, 
							struct descriptor *dim, struct descriptor_a *initialData, int idx);
extern EXPORT int TreePutSegment(int nid, int rowidx, struct descriptor_a *data);
extern EXPORT int _TreePutSegment(void *dbid, int nid, int rowidx, struct descriptor_a *data);
extern EXPORT int TreeUpdateSegment(int nid, struct descriptor *start, struct descriptor *end, struct descriptor *dim, int idx);
extern EXPORT int _TreeUpdateSegment(void *dbid, int nid, struct descriptor *start, struct descriptor *end, struct descriptor *dim, int idx);
extern EXPORT int TreeBeginTimestampedSegment(int nid, struct descriptor_a *initialValue, int idx);
extern EXPORT int _TreeBeginTimestampedSegment(void *dbid, int nid, struct descriptor_a *initialValue, int idx);
extern EXPORT int TreePutTimestampedSegment(int nid, _int64 *timestamp, struct descriptor_a *rowdata);
extern EXPORT int _TreePutTimestampedSegment(void *dbid, int nid, _int64 *timestamp, struct descriptor_a *rowdata);
extern EXPORT int TreePutRow(int nid, int bufsize, _int64 *timestamp, struct descriptor_a *rowdata);
extern EXPORT int _TreePutRow(void *dbid, int nid, int bufsize, _int64 *timestamp, struct descriptor_a *rowdata);
extern EXPORT int TreeSetTimeContext( struct descriptor *start, struct descriptor *end, struct descriptor *delta);
extern EXPORT int _TreeSetTimeContext(void *dbid,  struct descriptor *start, struct descriptor *end, struct descriptor *delta);
extern EXPORT int TreeGetNumSegments(int nid, int *num);
extern EXPORT int _TreeGetNumSegments(void *dbid, int nid, int *num);
extern EXPORT int TreeGetSegmentLimits(int nid, int segidx, struct descriptor_xd *start, struct descriptor_xd *end);
extern EXPORT int _TreeGetSegmentLimits(void *dbid, int nid, int segidx, struct descriptor_xd *start, struct descriptor_xd *end);
extern EXPORT int TreeGetSegment(int nid, int segidx, struct descriptor_xd *data, struct descriptor_xd *dim);
extern EXPORT int _TreeGetSegment(void *dbid, int nid, int segidx, struct descriptor_xd *data, struct descriptor_xd *dim);
extern EXPORT int TreeGetSegmentInfo(int nid, int idx, char *dtype, char *dimct, int *dims, int *next_row);
extern EXPORT int _TreeGetSegmentInfo(void *dbid, int nid, int idx, char *dtype, char *dimct, int *dims, int *next_row);


extern EXPORT int TreeGetXNci(int nid, char *xnciname, struct descriptor_xd *value);
extern EXPORT int _TreeGetXNci(void *dbid, int nid, char *xnciname, struct descriptor_xd *value);
extern EXPORT int TreeSetXNci(int nid, char *xnciname, struct descriptor *value);
extern EXPORT int _TreeSetXNci(void *dbid, int nid, char *xnciname, struct descriptor *value);
extern EXPORT int TreeSetViewDate(_int64 *date);
extern EXPORT int _TreeSetViewDate(void *dbid, _int64 *date);
extern EXPORT int TreeSetCurrentShotId(char *experiment, int shot);
extern EXPORT int TreeGetCurrentShotId(char *experiment);
extern EXPORT int TreeSetDbiItm(int code, int value);

#endif

#endif
