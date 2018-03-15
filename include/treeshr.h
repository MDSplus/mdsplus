#pragma once

/********** define this in case user code is checking it ************/

#include <treeshr_messages.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <mdsplus/mdsconfig.h>
#include <mdstypes.h>

#define TreeBLOCKID 0x3ade68b1

extern int treeshr_errno;
extern int TREE_BLOCKID;




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


  extern EXPORT void* TreeSavePrivateCtx(void* dbid);
  extern EXPORT void* TreeRestorePrivateCtx(void* pctx);
  extern EXPORT char *TreeAbsPath(char const *in);	     /********** Use TreeFree(result) *****/
  extern EXPORT char *_TreeAbsPath(void *dbid, char const *in);/********** Use TreeFree(result) *****/
  extern EXPORT int TreeAddConglom(char const *path, char const *congtype, int *nid);
  extern EXPORT int _TreeAddConglom(void *dbid, char const *path, char const *congtype, int *nid);
  extern EXPORT int TreeAddNode(char const *name, int *nid_ret, char usage);
  extern EXPORT int _TreeAddNode(void *dbid, char const *name, int *nid_ret, char usage);
  extern EXPORT int TreeAddTag(int nid, char const *tagnam);
  extern EXPORT int _TreeAddTag(void *dbid, int nid, char const *tagnam);
  extern EXPORT int TreeCleanDatafile(char *tree, int shot);
  extern EXPORT int _TreeCleanDatafile(void **dbid, char *tree, int shot);
  extern EXPORT int TreeClose(char const *tree, int shot);
  extern EXPORT int _TreeClose(void **dbid, char const *tree, int shot);
  extern EXPORT int TreeCompressDatafile(char *tree, int shot);
  extern EXPORT int _TreeCompressDatafile(void **dbid, char *tree, int shot);
  extern EXPORT int TreeCreatePulseFile(int shot, int numnids, int *nids);
  extern EXPORT int _TreeCreatePulseFile(void *dbid, int shot, int numnids, int *nids);
  extern EXPORT int TreeCreateTreeFiles(char *tree, int shot, int source_shot);
  extern EXPORT void *TreeDbid();
  extern EXPORT void *_TreeDbid(void **dbid);
  extern EXPORT void TreeDeleteNodeExecute(void);
  extern EXPORT void _TreeDeleteNodeExecute(void *dbid);
  extern EXPORT int TreeDeleteNodeGetNid(int *nid);
  extern EXPORT int _TreeDeleteNodeGetNid(void *dbid, int *nid);
  extern EXPORT int TreeDeleteNodeInitialize(int nid, int *count, int reset);
  extern EXPORT int _TreeDeleteNodeInitialize(void *dbid, int nid, int *count, int reset);
  extern EXPORT int TreeDeletePulseFile(int shotid, int all_versions);
  extern EXPORT int _TreeDeletePulseFile(void *dbid, int shotid, int all_versions);
  extern EXPORT int TreeDoMethod(struct descriptor *nid, struct descriptor *method, ...);
  extern EXPORT int _TreeDoMethod(void *dbid, struct descriptor *nid, struct descriptor *method,
				  ...);
  extern EXPORT int TreeDoMethod_HANDLER(int *sig_args, int *mech_args);
  extern EXPORT int TreeEditing();
  extern EXPORT int _TreeEditing(void *dbid);
  extern EXPORT int TreeEndConglomerate();
  extern EXPORT int _TreeEndConglomerate(void *dbid);
  extern EXPORT int TreeFindNode(char const *path, int *nid);
  extern EXPORT int _TreeFindNode(void *dbid, char const *path, int *nid);
  extern EXPORT int TreeFindNodeEnd(void **ctx);
  extern EXPORT int _TreeFindNodeEnd(void *dbid, void **ctx);
  extern EXPORT char *TreeFindNodeTags(int nid, void **ctx); /********** Use TreeFree(result) *****/
  extern EXPORT char *_TreeFindNodeTags(void *dbid, int nid, void **ctx);/********** Use TreeFree(result) *****/
  extern EXPORT int TreeFindNodeWild(char const *path, int *nid, void **ctx, int usage_mask);
  extern EXPORT int _TreeFindNodeWild(void *dbid, char const *path, int *nid, void **ctx,
				      int usage_mask);
  extern EXPORT void TreeFindTagEnd(void **ctx);
  extern EXPORT char *TreeFindTagWild(char *wild, int *nidout, void **search_ctx);
  extern EXPORT char *_TreeFindTagWild(void *dbid, char *wild, int *nidout, void **ctx);
  extern EXPORT int TreeFlushOff(int nid);
  extern EXPORT int _TreeFlushOff(void *dbid, int nid);
  extern EXPORT int TreeFlushReset(int nid);
  extern EXPORT int _TreeFlushReset(void *dbid, int nid);
  extern EXPORT void TreeFree(void *);
  extern EXPORT void TreeFreeDbid(void *);
  extern EXPORT int _TreeNewDbid(void **dbid);
  extern EXPORT int TreeGetDbi(struct dbi_itm *itmlst);
  extern EXPORT int _TreeGetDbi(void *dbid, struct dbi_itm *itmlst);
  extern EXPORT int TreeGetNci(int nid, struct nci_itm *itmlst);
  extern EXPORT int _TreeGetNci(void *dbid, int nid, struct nci_itm *itmlst);
  extern EXPORT int TreeGetDefaultNid(int *nid);
  extern EXPORT int _TreeGetDefaultNid(void *dbid, int *nid);
  extern EXPORT char *TreeGetMinimumPath(int *def_nid, int nid);
							       /********** Use TreeFree(result) *****/
  extern EXPORT char *_TreeGetMinimumPath(void *dbid, int *def_nid, int nid);
									    /********** Use TreeFree(result) *****/
  extern EXPORT char *TreeGetPath(int nid);
					  /********** Use TreeFree(result) *****/
  extern EXPORT char *_TreeGetPath(void *dbid, int nid);
						       /********** Use TreeFree(result) *****/
  extern EXPORT int TreeGetRecord(int nid, struct descriptor_xd *dsc_ptr);
  extern EXPORT int _TreeGetRecord(void *dbid, int nid, struct descriptor_xd *dsc_ptr);
  extern EXPORT int TreeGetStackSize();
  extern EXPORT int _TreeGetStackSize(void *dbid);
  extern EXPORT int TreeGetViewDate(int64_t * date);
  extern EXPORT int TreeIsOn(int nid);
  extern EXPORT int _TreeIsOn(void *dbid, int nid);
  extern EXPORT int TreeIsOpen();
  extern EXPORT int _TreeIsOpen(void *dbid);
  extern EXPORT int TreeLink(struct descriptor *intree, struct descriptor *outtree);
  extern EXPORT int TreeMarkIncludes();
  extern EXPORT int TreeNodePresent(int *nid);
  extern EXPORT int TreeOpen(char const *tree, int shot, int flags);
  extern EXPORT int _TreeOpen(void **dbid, char const *tree, int shot, int flags);
  extern EXPORT int TreeOpenEdit(char const *tree, int shot);
  extern EXPORT int _TreeOpenEdit(void **dbid, char const *tree, int shot);
  extern EXPORT int TreeOpenNew(char const *tree, int shot);
  extern EXPORT int _TreeOpenNew(void **dbid, char const *tree, int shot);
  extern EXPORT int TreePutRecord(int nid, struct descriptor *descriptor_ptr, int utility_update);
  extern EXPORT int _TreePutRecord(void *dbid, int nid, struct descriptor *descriptor,
				   int utility_update);
  extern EXPORT int TreeQuitTree(char const *tree, int shot);
  extern EXPORT int _TreeQuitTree(void **dbid, char const *tree, int shot);
  extern EXPORT int TreeRemoveNodesTags(int nid);
  extern EXPORT int _TreeRemoveNodesTags(void *dbid, int nid);
  extern EXPORT int TreeRemoveTag(char const *tagname);
  extern EXPORT int _TreeRemoveTag(void *dbid, char const *tagname);
  extern EXPORT int TreeRenameNode(int nid, char const *newname);
  extern EXPORT int _TreeRenameNode(void *dbid, int nid, char const *newname);
  extern EXPORT void TreeRestoreContext(void *ctx);
  extern EXPORT void _TreeRestoreContext(void **dbid, void *ctx);
  extern EXPORT int TreeRundownTree(struct descriptor *treenam, int *shotid, int single_tree,
				    int force_exit);
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
  extern EXPORT int TreeUsePrivateCtx(int onoff);
  extern EXPORT int TreeUsingPrivateCtx();
  extern EXPORT int TreeVerify();
  extern EXPORT int _TreeVerify(void *dbid);
  extern EXPORT int TreeWait();
  extern EXPORT int TreeWriteTree(char const *tree, int shot);
  extern EXPORT int _TreeWriteTree(void **dbid, char const *tree, int shot);

//Segments
  extern EXPORT int TreeBeginSegment(int nid, struct descriptor *start, struct descriptor *end,
				     struct descriptor *dim, struct descriptor_a *initialData,
				     int idx);
  extern EXPORT int _TreeBeginSegment(void *dbid, int nid, struct descriptor *start,
				      struct descriptor *end, struct descriptor *dim,
				      struct descriptor_a *initialData, int idx);
  extern EXPORT int TreeMakeSegment(int nid, struct descriptor *start, struct descriptor *end,
				    struct descriptor *dim, struct descriptor_a *initialData,
				    int idx, int filled);
  extern EXPORT int _TreeMakeSegment(void *dbid, int nid, struct descriptor *start,
				     struct descriptor *end, struct descriptor *dim,
				     struct descriptor_a *initialData, int idx, int filled);
  extern EXPORT int TreePutSegment(int nid, const int rowidx, struct descriptor_a *data);
  extern EXPORT int _TreePutSegment(void *dbid, int nid, const int rowidx, struct descriptor_a *data);
  extern EXPORT int TreeUpdateSegment(int nid, struct descriptor *start, struct descriptor *end,
				      struct descriptor *dim, int idx);
  extern EXPORT int _TreeUpdateSegment(void *dbid, int nid, struct descriptor *start,
				       struct descriptor *end, struct descriptor *dim, int idx);
  extern EXPORT int TreeBeginTimestampedSegment(int nid, struct descriptor_a *initialValue,
						int idx);
  extern EXPORT int _TreeBeginTimestampedSegment(void *dbid, int nid,
						 struct descriptor_a *initialValue, int idx);
  extern EXPORT int TreePutTimestampedSegment(int nid, int64_t * timestamp,
					      struct descriptor_a *rowdata);
  extern EXPORT int _TreePutTimestampedSegment(void *dbid, int nid, int64_t * timestamp,
					       struct descriptor_a *rowdata);
  extern EXPORT int TreeMakeTimestampedSegment(int nid, int64_t * timestamp,
					       struct descriptor_a *rowdata, int idx, int filled);
  extern EXPORT int _TreeMakeTimestampedSegment(void *dbid, int nid, int64_t * timestamp,
						struct descriptor_a *rowdata, int idx, int filled);
  extern EXPORT int TreePutRow(int nid, int bufsize, int64_t * timestamp,
			       struct descriptor_a *rowdata);
  extern EXPORT int _TreePutRow(void *dbid, int nid, int bufsize, int64_t * timestamp,
				struct descriptor_a *rowdata);
  extern EXPORT int TreeSetTimeContext(struct descriptor *start, struct descriptor *end,
				       struct descriptor *delta);
  extern EXPORT int _TreeSetTimeContext(void *dbid, struct descriptor *start,
					struct descriptor *end, struct descriptor *delta);
  extern EXPORT int TreeGetTimeContext(struct descriptor_xd *start, struct descriptor_xd *end,
				       struct descriptor_xd *delta);
  extern EXPORT int _TreeGetTimeContext(void *dbid, struct descriptor_xd *start,
					struct descriptor_xd *end, struct descriptor_xd *delta);
  extern EXPORT int TreeGetNumSegments(int nid, int *num);
  extern EXPORT int _TreeGetNumSegments(void *dbid, int nid, int *num);
  extern EXPORT int TreeGetSegmentLimits(int nid, int segidx, struct descriptor_xd *start,
					 struct descriptor_xd *end);
  extern EXPORT int _TreeGetSegmentLimits(void *dbid, int nid, int segidx,
					  struct descriptor_xd *start, struct descriptor_xd *end);
  extern EXPORT int TreeGetSegment(int nid, int segidx, struct descriptor_xd *data,
				   struct descriptor_xd *dim);
  extern EXPORT int _TreeGetSegment(void *dbid, int nid, int segidx, struct descriptor_xd *data,
				    struct descriptor_xd *dim);
  extern EXPORT int TreeGetSegments(int nid, struct descriptor *start, struct descriptor *end,
				    struct descriptor_xd *retSegments);
  extern EXPORT int _TreeGetSegments(void *dbid, int nid, struct descriptor *start,
				     struct descriptor *end, struct descriptor_xd *retSegments);
  extern EXPORT int TreeGetSegmentInfo(int nid, int idx, char *dtype, char *dimct, int *dims,
				       int *next_row);
  extern EXPORT int _TreeGetSegmentInfo(void *dbid, int nid, int idx, char *dtype, char *dimct,
					int *dims, int *next_row);
  extern EXPORT int TreeGetSegments(int nid, struct descriptor *start, struct descriptor *end,
				    struct descriptor_xd *out);
  extern EXPORT int _TreeGetSegments(void *dbid, int nid, struct descriptor *start,
				     struct descriptor *end, struct descriptor_xd *out);
  extern EXPORT int TreeGetSegmentTimes(int nid, int *numsegs, int64_t ** times);
  extern EXPORT int _TreeGetSegmentTimes(void *dbid, int nid, int *numsegs, int64_t ** times);
  extern EXPORT int TreeGetSegmentTimesXd(int nid, int *numsegs, struct descriptor_xd *start_list,
					  struct descriptor_xd *end_list);
  extern EXPORT int _TreeGetSegmentTimesXd(void *dbid, int nid, int *numsegs,
					   struct descriptor_xd *start_list,
					   struct descriptor_xd *end_list);

  extern EXPORT int TreeGetXNci(int nid, const char *xnciname, struct descriptor_xd *value);
  extern EXPORT int _TreeGetXNci(void *dbid, int nid, const char *xnciname, struct descriptor_xd *value);
  extern EXPORT int TreeSetXNci(int nid, const char *xnciname, struct descriptor *value);
  extern EXPORT int _TreeSetXNci(void *dbid, int nid, const char *xnciname, struct descriptor *value);
  extern EXPORT int TreeSetViewDate(int64_t * date);
  extern EXPORT int _TreeSetViewDate(void *dbid, int64_t * date);
  extern EXPORT int TreeSetCurrentShotId(char const *experiment, int shot);
  extern EXPORT int TreeGetCurrentShotId(char const *experiment);
  extern EXPORT int TreeSetDbiItm(int code, int value);
  extern EXPORT int64_t TreeGetDatafileSize();
  extern EXPORT int64_t _TreeGetDatafileSize(void *dbid);

  extern EXPORT int TreeFindNodeTagsDsc(int nid_in, void **ctx_ptr, struct descriptor *tag);
  extern EXPORT int TreeFindTagWildDsc(char *wild, int *nidout, void **ctx_inout,
				       struct descriptor_xd *name);

  extern EXPORT int _TreeGetSegmentScale(void *dbid, int nid, struct descriptor_xd *value);
  extern EXPORT int TreeGetSegmentScale(int nid, struct descriptor_xd *value);
  extern EXPORT int _TreeSetSegmentScale(void *dbid, int nid, struct descriptor *value);
  extern EXPORT int TreeSetSegmentScale(int nid, struct descriptor *value);

#ifdef __cplusplus
}
#endif
