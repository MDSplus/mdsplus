#ifndef __TREESHR
#define __TREESHR

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
#define TreeFILE_NOT_FOUND 0xfd18faa
#define TreeCANCEL 0xfd18c80

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
   ,STS_TEXT(TreeFILE_NOT_FOUND,"File or Directory Not Found")
   ,STS_TEXT(TreeCANCEL,"User canceled operation")
   };
#endif

/******************* Prototypes for TREESHR routines *********************/

#ifdef _MDS_NO_PROTO

extern char* TreeAbsPath();           /********** Use TreeFree(result) *****/
extern char* _TreeAbsPath();          /********** Use TreeFree(result) *****/
extern int TreeAddConglom();
extern int _TreeAddConglom();
/* obsolete */	extern int TreeAddMethodImage();
extern int TreeAddNode();
extern int TreeAddTag();
extern int _TreeAddTag();
extern int TreeCleanDatafile();
extern int _TreeCleanDatafile();
/* obsolete */	extern int TreeClearMethodImageS();
extern int TreeClose();
extern int _TreeClose();
extern int TreeCompressDatafile();
extern int _TreeCompressDatafile();
extern int TreeCreatePulseFile();
extern int _TreeCreatePulseFile();
extern void TreeDeleteNodeExecute();
extern int TreeDeleteNodeGetNid();
extern int TreeDeleteNodeInitialize();
extern int TreeDeletePulseFile();
extern int _TreeDeletePulseFile();
extern int TreeDoMethod();
extern int _TreeDoMethod();
/* obsolete */	extern int TreeDoMethodHandler();
extern int TreeEditing();
extern int _TreeEditing();
extern int TreeEndConglomerate();
extern int _TreeEndConglomerate();
extern int TreeFindNode();
extern int _TreeFindNode();
extern int TreeFindNodeEnd();
extern int _TreeFindNodeEnd();
extern char *TreeFindNodeTags();   /********** Use TreeFree(result) *****/
extern char *_TreeFindNodeTags();  /********** Use TreeFree(result) *****/
extern int TreeFindNodeWild();
extern int _TreeFindNodeWild();
extern void TreeFindTagEnd();
extern char *TreeFindTagWild();
extern char *_TreeFindTagWild();
extern int TreeFlushOff();
extern int TreeFlushReset();
extern int TreeFree();
extern int TreeGetDbi();
extern int _TreeGetDbi();
extern int TreeGetNci();
extern int _TreeGetNci();
extern int TreeGetDefaultNid();
extern int _TreeGetDefaultNid();
extern char *TreeGetMinimumPath();  /********** Use TreeFree(result) *****/
extern char *_TreeGetMinimumPath(); /********** Use TreeFree(result) *****/
extern char *TreeGetPath(); /********** Use TreeFree(result) *****/
extern char *_TreeGetPath();/********** Use TreeFree(result) *****/
extern int TreeGetRecord();
extern int _TreeGetRecord();
extern int TreeGetStackSize();
extern int _TreeGetStackSize();
extern int TreeIsOn();
extern int _TreeIsOn();
extern int TreeIsOpen();
extern int _TreeIsOpen();
	extern int TreeLink();
	extern int TreeMarkIncludes();
extern int TreeOpen();
extern int TreeOpenEdit();
extern int _TreeOpenEdit();
extern int TreeOpenNew();
extern int _TreeOpenNew();
extern int TreePutRecord();
extern int _TreePutRecord();
extern int TreeQuitTree();
extern int _TreeQuitTree();
extern int TreeRemoveNodesTags();
extern int TreeRemoveTag();
extern int TreeRenameNode();
extern void TreeRestoreContext();
extern void _TreeRestoreContext();
	extern int TreeRundownTree();
extern void *TreeSaveContext();
extern void *_TreeSaveContext();
extern int TreeSetNci();
extern int _TreeSetNci();
extern int TreeSetDefault();
extern int _TreeSetDefault();
extern int TreeSetDefaultNid();
extern int _TreeSetDefaultNid();
extern int TreeSetNoSubtree();
extern int _TreeSetNoSubtree();
extern int TreeSetStackSize();
extern int _TreeSetStackSize();
extern int TreeSetSubtree();
extern int _TreeSetSubtree();
extern int TreeStartConglomerate();
extern int _TreeStartConglomerate();
extern void *TreeSwitchDbid();
extern int TreeTurnOff();
extern int _TreeTurnOff();
extern int TreeTurnOn();
extern int _TreeTurnOn();
extern int TreeVerify();
extern int _TreeVerify();
	extern int TreeWait();
extern int TreeWriteTree();
extern int _TreeWriteTree();

#else

extern char *TreeAbsPath(char *in);             /********** Use TreeFree(result) *****/
extern char *_TreeAbsPath( void *dbid, char *in);  /********** Use TreeFree(result) *****/
extern int TreeAddConglom(char *path, char *congtype, int *nid);
extern int _TreeAddConglom(void *dbid, char *path, char *congtype, int *nid);
extern int TreeAddNode(char *name, int *nid_ret, char usage);
extern int _TreeAddNode(void *dbid, char *name, int *nid_ret, char usage);
extern int TreeAddTag(int nid, char *tagnam);
extern int _TreeAddTag(void *dbid, int nid, char *tagnam);
extern int TreeCleanDatafile(char *tree, int shot);
extern int _TreeCleanDatafile(void **dbid, char *tree, int shot);
extern int TreeClose(char *tree, int shot);
extern int _TreeClose(void **dbid, char *tree, int shot);
extern int TreeCompressDatafile(char *tree, int shot);
extern int _TreeCompressDatafile(void **dbid, char *tree, int shot);
extern int TreeCreatePulseFile( int shot, int numnids, int *nids);
extern int _TreeCreatePulseFile(void *dbid, int shot, int numnids, int *nids);
extern void TreeDeleteNodeExecute(void);
extern void _TreeDeleteNodeExecute(void *dbid);
extern int TreeDeleteNodeGetNid(int *nid);
extern int _TreeDeleteNodeGetNid(void *dbid, int *nid);
extern int TreeDeleteNodeInitialize(int nid, int *count, int reset);
extern int _TreeDeleteNodeInitialize(void *dbid, int nid, int *count, int reset);
extern int TreeDeletePulseFile(int shotid, int all_versions);
extern int _TreeDeletePulseFile(void *dbid, int shotid, int all_versions);
extern int TreeDoMethod(struct descriptor *nid, struct descriptor *method, ...);
extern int _TreeDoMethod(void *dbid, struct descriptor *nid, struct descriptor *method, ...);
extern int TreeDoMethod_HANDLER(int *sig_args, int *mech_args);
extern int TreeEditing();
extern int _TreeEditing(void *dbid);
extern int TreeEndConglomerate();
extern int _TreeEndConglomerate(void *dbid);
extern int TreeFindNode(char *path, int *nid);
extern int _TreeFindNode(void *dbid, char *path, int *nid);
extern int TreeFindNodeEnd(void **ctx);
extern int _TreeFindNodeEnd(void *dbid, void **ctx);
extern char *TreeFindNodeTags(int nid, void **ctx);   /********** Use TreeFree(result) *****/
extern char *_TreeFindNodeTags(void *dbid, int nid, void **ctx);  /********** Use TreeFree(result) *****/
extern int TreeFindNodeWild(char *path, int *nid, void **ctx, int usage_mask);
extern int _TreeFindNodeWild(void *dbid, char *path, int *nid, void **ctx, int usage_mask);
extern void TreeFindTagEnd(void **ctx);
extern char *TreeFindTagWild(char *wild, int *nidout, void **search_ctx);
extern char *_TreeFindTagWild(void *dbid, char *wild, int *nidout, void **ctx);
extern int TreeFlushOff(int nid);
extern int _TreeFlushOff(void *dbid, int nid);
extern int TreeFlushReset(int nid);
extern int _TreeFlushReset(void *dbid, int nid);
extern void TreeFree(void *);
extern int TreeGetDbi(struct dbi_itm *itmlst);
extern int _TreeGetDbi(void *dbid, struct dbi_itm *itmlst);
extern int TreeGetNci(int nid, struct nci_itm *itmlst);
extern int _TreeGetNci(void *dbid, int nid, struct nci_itm *itmlst);
extern int TreeGetDefaultNid(int *nid);
extern int _TreeGetDefaultNid(void *dbid, int *nid);
extern char *TreeGetMinimumPath(int *def_nid, int nid); /********** Use TreeFree(result) *****/
extern char *_TreeGetMinimumPath(void *dbid, int *def_nid, int nid); /********** Use TreeFree(result) *****/
extern char *TreeGetPath(int nid); /********** Use TreeFree(result) *****/
extern char *_TreeGetPath(void *dbid, int nid); /********** Use TreeFree(result) *****/
extern int TreeGetRecord(int nid, struct descriptor_xd *dsc_ptr);
extern int _TreeGetRecord(void *dbid, int nid, struct descriptor_xd *dsc_ptr);
extern int TreeGetStackSize();
extern int _TreeGetStackSize(void *dbid);
extern int TreeIsOn(int nid);
extern int _TreeIsOn(void *dbid, int nid);
extern int TreeIsOpen();
extern int _TreeIsOpen(void *dbid);
extern int TreeLink(struct descriptor *intree, struct descriptor *outtree);
extern int TreeMarkIncludes();
extern int TreeNodePresent(int *nid);
extern int TreeOpen(char *tree, int shot, int flags);
extern int _TreeOpen(void **dbid,char *tree, int shot, int flags);
extern int TreeOpenEdit(char *tree, int shot);
extern int _TreeOpenEdit(void **dbid, char *tree, int shot);
extern int TreeOpenNew(char *tree, int shot);
extern int _TreeOpenNew(void **dbid, char *tree, int shot);
extern int TreePutRecord(int nid, struct descriptor *descriptor_ptr, int utility_update);
extern int _TreePutRecord(void *dbid, int nid, struct descriptor *descriptor, int utility_update);
extern int TreeQuitTree(char *tree, int shot);
extern int _TreeQuitTree(void **dbid, char *tree, int shot);
extern int TreeRemoveNodesTags(int nid);
extern int TreeRemoveTag(char *tagnam);
extern int _TreeRemoveTag(void *dbid, char *tagnam);
extern int TreeRenameNode(int nid, char *newnamedsc_ptr);
extern int _TreeRenameNode(void *dbid, int nid, char *newname);
extern void TreeRestoreContext(void *ctx);
extern void _TreeRestoreContext(void **dbid, void *ctx);
extern int TreeRundownTree(struct descriptor *treenam, int *shotid, int single_tree, int force_exit);
extern void *TreeSaveContext();
extern void *_TreeSaveContext(void *dbid);
extern int TreeSetNci(int nid, struct nci_itm *itmlst);
extern int _TreeSetNci(void *dbid, int nid, struct nci_itm *itmlst);
extern int TreeSetDefault(char *path, int *nid);
extern int _TreeSetDefault(void *dbid, char *path, int *nid);
extern int TreeSetDefaultNid(int nid);
extern int _TreeSetDefaultNid(void *dbid, int nid);
extern int TreeSetNoSubtree(int nid);
extern int _TreeSetNoSubtree(void *dbid, int nid);
extern int TreeSetStackSize(int new_size);
extern int _TreeSetStackSize(void **dbid, int new_size);
extern int TreeSetSubtree(int nid);
extern int _TreeSetSubtree(void *dbid, int nid);
extern int TreeStartConglomerate(int numnodes);
extern int _TreeStartConglomerate(void *dbid, int numnodes);
extern void *TreeSwitchDbid(void *dbid);
extern int TreeTurnOff(int nid);
extern int _TreeTurnOff(void *dbid, int nid);
extern int TreeTurnOn(int nid);
extern int _TreeTurnOn(void *dbid, int nid);
extern int TreeVerify();
extern int _TreeVerify(void *dbid);
extern int TreeWait();
extern int TreeWriteTree(char *tree, int shot);
extern int _TreeWriteTree(void **dbid, char *tree, int shot);

#endif

#endif
