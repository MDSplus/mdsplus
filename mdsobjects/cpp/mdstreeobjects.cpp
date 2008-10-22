#include "mdsobjects.h"
//#include "mdstree.h"
using namespace MDSobjects;

#ifdef HAVE_WINDOWS_H
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#define MAX_ARGS 512

extern "C" {
	void *convertToScalarDsc(int clazz, int dtype, int length, char *ptr);
	void *evaluateData(void *dscPtr, int isEvaluate);
	void freeDsc(void *dscPtr);
	void *convertFromDsc(void *dscPtr);
	char *decompileDsc(void *dscPtr);
	char *decompileDsc(void *dscPtr);
	void *compileFromExprWithArgs(char *expr, int nArgs, void *args, void *tree);
	void freeChar(void *);
	void *convertToArrayDsc(int clazz, int dtype, int length, int l_length, int nDims, int *dims, void *ptr);
	void *convertToCompoundDsc(int clazz, int dtype, int length, void *ptr, int ndescs, void **descs);
	void *convertToApdDsc(int ndescs, void **ptr);
	void *deserializeData(char *serialized, int size);

	void * convertToByte(void *dsc); 
	void * convertToShort(void *dsc); 
	void * convertToInt(void *dsc); 
	void * convertToLong(void *dsc); 
	void * convertToFloat(void *dsc); 
	void * convertToDouble(void *dsc); 
	void * convertToShape(void *dcs);


}




extern "C" void RTreeSynch();
extern "C" int TreeOpen(char *tree, int shot, int readOnly);
extern "C" int TreeClose(char *tree, int shot);
extern "C" int RTreeOpen(char *tree, int shot);
extern "C" int RTreeClose(char *tree, int shot);
extern "C" char *RTreeSetCallback(int nid, void *argument, void (*callback)(int, void *));
extern "C" char RTreeSetWarm(int nid, int warm);
extern "C" int RTreeClearCallback(int nid, char *callbackDescr);
extern "C" void RTreeConfigure(int shared, int size);
extern "C" int TreeClose(char *tree, int shot);
extern "C" int TreeFindNode(char *path, int *nid);
extern "C" int TreeFindNodeWild(char *path, int *nid, void **ctx, int mask);
extern "C" char *TreeGetPath(int nid);
extern "C" void TreeFree(void *ptr);
extern "C" int TreeGetDbi(struct dbi_itm *itmlst);
extern "C" int TreeGetNci(int nid, struct nci_itm *itmlst);
extern "C" int TreeSetNci(int nid, struct nci_itm *itmlst);
extern "C" int TreeIsOn(int nid);
extern "C" int TreeTurnOff(int nid);
extern "C" int TreeTurnOn(int nid);
extern "C" void convertTime(int *time, char *retTime);
extern "C" int TreeSetDefaultNid(int nid);
extern "C" int TreeGetDefaultNid(int *nid);
extern "C" char *TreeFindNodeTags(int nid, void *ctx);
extern "C" int TreeFindTagEnd(void *ctx);

extern "C" int getTreeData(int nid, void **data, int isCached);
extern "C" int putTreeData(int nid, void *data, int isCached, int cachePolicy);
extern "C" int deleteTreeData(int nid);
extern "C" int doTreeMethod(int nid, char *method);
extern "C" int beginTreeSegment(int nid, void *dataDsc, void *startDsc, void *endDsc, 
								void *timeDsc, int isCached, int cachePolicy);
extern "C" int putTreeSegment(int nid, void *dataDsc, int ofs, int isCached, int cachePolicy);
extern "C" int updateTreeSegment(int nid, void *startDsc, void *endDsc, 
								void *timeDsc, int isCached, int cachePolicy);
extern "C" int getTreeNumSegments(int nid, int *numSegments, int isCached); 
extern "C" int getTreeSegmentLimits(int nid, void **startDsc, void **endDsc, int isCached);
extern "C" int getTreeSegment(int nid, int segIdx, void **dataDsc, int isCached);
extern "C" int setTreeTimeContext(void *startDsc, void *endDsc, void *deltaDsc);//No cache option  
extern "C" int beginTreeTimestampedSegment(int nid, void *dataDsc, int isCached, int cachePolicy);
extern "C" int putTreeTimestampedSegment(int nid, void *dataDsc, _int64 *times, int isCached, int cachePolicy);
extern "C" int putTreeRow(int nid, void *dataDsc, _int64 *time, int isCached, int isLast, int cachePolicy);
extern "C" void * TreeSwitchDbid(void *ctx);
extern "C" int LibConvertDateString(char *asc_time, _int64 *qtime);
extern "C" int TreeSetViewDate(_int64 *date);



extern "C" int RTreeFlush(int nid);

		
Tree::Tree(char *name, int shot)
{
	this->shot = shot;
	TreeSwitchDbid((void *)0);
	int status = TreeOpen(name, shot, 0);
	if(!(status & 1))
	{
		throw new TreeException(status);
	}
	ctx=TreeSwitchDbid((void *)0);
	TreeSwitchDbid(ctx);
	this->name = new char[strlen(name) + 1];
	strcpy(this->name, name);
}

Tree::~Tree()
{
	void *currentCtx = TreeSwitchDbid(ctx);
	TreeClose(name, shot);
	if (currentCtx == ctx)
		TreeSwitchDbid((void *)0);
	else
		TreeSwitchDbid(currentCtx);
	delete [] name;
}

TreeNode *Tree::getNode(char *path)
{
	int nid, status;

	setActiveTree(this);
	status = TreeFindNode(path, &nid);
	if(!(status & 1))
	{
		throw new TreeException(status);
	}
	return new TreeNode(nid, this);
}

	
TreeNode *Tree::getNode(TreePath *path)
{
	int nid, status;
	char *pathName = path->getString();
	setActiveTree(this);
	status = TreeFindNode(pathName, &nid);
	delete[] pathName;
	if(!(status & 1))
	{
		throw new TreeException(status);
	}
	return new TreeNode(nid, this);
}

	

TreeNodeArray *Tree::getNodeWild(char *path, int usageMask)
{
	int currNid, status; 
	int numNids = 0;
	void *ctx = 0;
	setActiveTree(this);
	while ((status = TreeFindNodeWild(path,&currNid,&ctx, usageMask)) & 1)
		numNids++;


	printf("%s\n", MdsGetMsg(status));

	TreeNode **retNodes = new TreeNode *[numNids];
	ctx = 0;
	setActiveTree(this);
	for(int i = 0; i < numNids; i++)
	{
		TreeFindNodeWild(path,&currNid,&ctx, usageMask);
		retNodes[i] = new TreeNode(currNid, this);
	}
	TreeNodeArray *nodeArray = new TreeNodeArray(retNodes, numNids);
	delete [] retNodes;
	return nodeArray;
}

TreeNodeArray *Tree::getNodeWild(char *path)
{

	return getNodeWild(path, -1);
}


void Tree::setDefault(TreeNode *treeNode)
{
	setActiveTree(this);
	int status = TreeSetDefaultNid(treeNode->getNid());
	if(!(status & 1)) 
		throw new TreeException(status);
}

TreeNode *Tree::getDeault()
{
	int nid;

	setActiveTree(this);
	int status = TreeGetDefaultNid(&nid);
	if(!(status & 1)) 
		throw new TreeException(status);
	return new TreeNode(nid, this);
}

bool Tree::supportsVersions()
{
	int supports, len, status;
	struct dbi_itm dbiList[] = 
	{{sizeof(int), DbiVERSIONS_IN_PULSE, &supports, &len},
	{0, DbiEND_OF_LIST,0,0}};

	setActiveTree(this);
	status = TreeGetDbi(dbiList);
	if(!(status & 1)) 
		throw new TreeException(status);
	return (supports)?true:false;
}

void Tree::setViewDate(char *date)
{
	_int64  qtime;

	int status = LibConvertDateString(date, &qtime);
	if(!(status & 1))
		throw new TreeException("Invalid date format");
	setActiveTree(this);
	status = TreeSetViewDate(&qtime);
	if(!(status & 1)) 
		throw new TreeException(status);
}


void Tree::setTimeContext(Data *start, Data *end, Data *delta)
{
	setActiveTree(this);
	int status = setTreeTimeContext((start)?start->convertToDsc():0, (end)?end->convertToDsc():0, 
		(delta)?delta->convertToDsc():0);
	if(!(status & 1))
		throw new TreeException(status);
}


void *TreeNode::convertToDsc()
{
	setActiveTree(tree);
	return completeConversionToDsc(convertToScalarDsc(clazz, dtype, sizeof(int), (char *)&nid));
}

int TreeNode::getFlag(int flagOfs)
{
	int nciFlags;
	int nciFlagsLen = sizeof(int);
	struct nci_itm nciList[] =  {{4, NciGET_FLAGS, &nciFlags, &nciFlagsLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	setActiveTree(tree);
	int status = TreeGetNci(nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);
	return (nciFlags & flagOfs)?true:false;
}

void TreeNode::setFlag(int flagOfs, bool val)
{
	int nciFlags;
	int nciFlagsLen = sizeof(int);
	struct nci_itm nciList[] =  {{4, NciGET_FLAGS, &nciFlags, &nciFlagsLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	setActiveTree(tree);
	int status = TreeGetNci(nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);
	if(val)
		nciFlags |= flagOfs;
	else
		nciFlags &= ~flagOfs;

	status = TreeSetNci(nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);
}

TreeNode::TreeNode(int nid, Tree *tree, Data *units, Data *error, Data *help, Data *validation)
{
	if(!tree)
		throw new DataException(CLASS_S, DTYPE_NID, "A Tree instance must be defined when ceating TreeNode instances");
	this->nid = nid;
	this->tree = tree;
	clazz = CLASS_S;
	dtype = DTYPE_NID;
	setAccessory(units, error, help, validation);
}

		
char *TreeNode::getPath()
{
	setActiveTree(tree);
	char *currPath = TreeGetPath(nid);
	char *path = new char[strlen(currPath) + 1];
	strcpy(path, currPath);
	TreeFree(currPath);
	return path;
}

char *TreeNode::getFullPath()
{
	char path[1024];
	int pathLen = 1024;
	struct nci_itm nciList[] = 
		{{511, NciFULLPATH, path, &pathLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	setActiveTree(tree);
	int status = TreeGetNci(nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);
	char *retPath = new char[strlen(path)+1];
	strcpy(retPath, path);
	return retPath;
}


Data *TreeNode::getData()
{
	Data *data = 0;
	setActiveTree(tree);
	int status = getTreeData(nid, (void **)&data, isCached());
	if(!(status & 1))
	{
		throw new TreeException(status);
	}
	return data;
}

void TreeNode::putData(Data *data)
{
	setActiveTree(tree);
	int status = putTreeData(nid, (void *)data, isCached(), getCachePolicy());
	if(!(status & 1))
	{
		throw new TreeException(status);
	}
}
void TreeNode::deleteData()
{
	setActiveTree(tree);
	int status = deleteTreeData(nid);
	if(!(status & 1))
	{
		throw new TreeException(status);
	}
}

bool TreeNode::isOn()
{
	setActiveTree(tree);
	return TreeIsOn(nid)?true:false;
}
void TreeNode::setOn(bool on)
{
	setActiveTree(tree);
	if(on)
		TreeTurnOn(nid);
	else
		TreeTurnOff(nid);
}

int TreeNode::getDataSize()
{
	int length;
	int lengthLen = sizeof(int);
	struct nci_itm nciList[] = 
		{{4, NciLENGTH, &length, &lengthLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	setActiveTree(tree);
	int status = TreeGetNci(nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);
	return length;
}
int TreeNode::getCompressedDataSize()
{
	int length;
	int lengthLen = sizeof(int);
	struct nci_itm nciList[] = 
		{{4, NciRLENGTH, &length, &lengthLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	setActiveTree(tree);
	int status = TreeGetNci(nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);
	return length;
}

char *TreeNode::getInsertionDate()
{
	int timeInserted[2]; 
	int timeLen;
	struct nci_itm nciList[] = 
		{{8, NciTIME_INSERTED, timeInserted, &timeLen},
		{NciEND_OF_LIST, 0, 0, 0}};
	setActiveTree(tree);
	int status = TreeGetNci(nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);
	char ascTim[512];
	convertTime(timeInserted, ascTim);

	char *retTim = new char[strlen(ascTim) + 1];
	strcpy(retTim, ascTim);
	return retTim;
}

void TreeNode::doMethod(char *method)
{
	setActiveTree(tree);
	int status = doTreeMethod(nid, method);
	if(!(status & 1))
		throw new TreeException(status);
}

bool TreeNode::isSetup()
{
	return getFlag(NciM_SETUP_INFORMATION)?true:false;
}

void TreeNode::setSetup(bool flag)
{
	setFlag(NciM_SETUP_INFORMATION, flag);
}

bool TreeNode::isWriteOnce()
{
	return getFlag(NciM_WRITE_ONCE)?true:false;
}

void TreeNode::setWriteOnce(bool flag)
{
	setFlag(NciM_WRITE_ONCE, flag);
}

bool TreeNode::isCompressible()
{
	return getFlag(NciM_COMPRESSIBLE)?true:false;
}

void TreeNode::setCompressible(bool flag)
{
	setFlag(NciM_COMPRESSIBLE, flag);
}
bool TreeNode::isNoWriteModel()
{
	return getFlag(NciM_NO_WRITE_MODEL)?true:false;
}

void TreeNode::setNoWriteModel(bool flag)
{
	setFlag(NciM_NO_WRITE_MODEL, flag);
}

bool TreeNode::isNoWriteShot()
{
	return getFlag(NciM_NO_WRITE_SHOT)?true:false;
}

void TreeNode::setNoWriteShot(bool flag)
{
	setFlag(NciM_NO_WRITE_SHOT, flag);
}

TreeNode *TreeNode::getParent()
{

	int nidLen = 4;
	int parentNid;
	struct nci_itm nciList[] = 
		{{4, NciPARENT, &parentNid, &nidLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	setActiveTree(tree);
	int status = TreeGetNci(nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);

	return new TreeNode(parentNid, tree);
}

TreeNode **TreeNode::getChildren(int *numChildren)
{

	int numLen = 4;
	int numMembers, nChildren;
	struct nci_itm nciList[] = 
		{{4, NciNUMBER_OF_MEMBERS, &numMembers, &numLen},
		{4, NciNUMBER_OF_CHILDREN, &nChildren, &numLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	setActiveTree(tree);
	int status = TreeGetNci(nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);

	int *childrenNids = new int[numMembers+nChildren];

	int retLen = sizeof(int) * (numMembers+nChildren);
	struct nci_itm nciList1[] = 
		{{4, NciMEMBER_NIDS, &childrenNids[0], &retLen},
		{4, NciCHILDREN_NIDS, &childrenNids[numMembers], &retLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	status = TreeGetNci(nid, nciList);
	if(!(status & 1))
	{
		delete [] childrenNids;
		throw new TreeException(status);
	}
	TreeNode **retChildren = new TreeNode* [numMembers+nChildren];
	for(int i = 0; i < numMembers+nChildren; i++)
		retChildren[i] = new TreeNode(childrenNids[i], tree);

	delete[] childrenNids;
	*numChildren = numMembers+nChildren;
	return retChildren;
}

	
int TreeNode::getUsage()
{

	int usageLen = 4;
	int usage;
	struct nci_itm nciList[] = 
		{{4, NciUSAGE, &usage, &usageLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	setActiveTree(tree);
	int status = TreeGetNci(nid, nciList);
	if(!(status & 1))
		throw new TreeException(status);

	return usage;
}

char **TreeNode::getTags(int *numRetTags)
{
	const int MAX_TAGS = 128;
	
	char *tags[MAX_TAGS];
	int numTags;
	void *ctx = 0;
	setActiveTree(tree);
	for(numTags = 0; numTags < MAX_TAGS; numTags++)
	{
		tags[numTags] = TreeFindNodeTags(nid, &ctx);
		if(!tags[numTags]) break;
	}
	TreeFindTagEnd(&ctx);

	*numRetTags = numTags;
	char **retTags = new char *[numTags];
	for(int i = 0; i < numTags; i++)
	{
		retTags[i] = new char(strlen(tags[i])+1);
		strcpy(retTags[i], tags[i]);
		TreeFree(tags[i]);
	}
	return retTags;
}

void TreeNode::beginSegment(Data *start, Data *end, Data *time, Array *initialData)
{
	setActiveTree(tree);
	int status = beginTreeSegment(getNid(), start->convertToDsc(), 
		end->convertToDsc(), time->convertToDsc(), initialData->convertToDsc(), isCached(), getCachePolicy());
	if(!(status & 1))
		throw new TreeException(status);
}

void TreeNode::putSegment(Array *data, int ofs)
{
	setActiveTree(tree);
	int status = putTreeSegment(getNid(), data->convertToDsc(), ofs, isCached(), getCachePolicy());
	if(!(status & 1))
		throw new TreeException(status);
}

void TreeNode::updateSegment(Data *start, Data *end, Data *time)
{
	setActiveTree(tree);
	int status = updateTreeSegment(getNid(), start->convertToDsc(), 
		end->convertToDsc(), time->convertToDsc(), isCached(), getCachePolicy());
	if(!(status & 1))
		throw new TreeException(status);
}

int TreeNode::getNumSegments()
{
	int numSegments;

	setActiveTree(tree);
	int status = getTreeNumSegments(getNid(), &numSegments, isCached());
	if(!(status & 1))
		throw new TreeException(status);
	return  numSegments;
}

void TreeNode::getSegmentLimits(int segmentIdx, Data **start, Data **end)
{
	void *startDsc, *endDsc;
	setActiveTree(tree);
	int status = getTreeSegmentLimits(getNid(), &startDsc, &endDsc, isCached());
	if(!(status & 1))
		throw new TreeException(status);
	*start = (Data*)convertFromDsc(startDsc);
	freeDsc(startDsc);
	*end = (Data*)convertFromDsc(endDsc);
	freeDsc(endDsc);
}
	
Array *TreeNode::getSegment(int segIdx)
{
	void *dataDsc;
	
	setActiveTree(tree);
	int status = getTreeSegment(getNid(), segIdx, &dataDsc, isCached());
	if(!(status & 1))
		throw new TreeException(status);
	Array *retData = (Array *)convertFromDsc(dataDsc);
	freeDsc(dataDsc);
	return retData;
}


void TreeNode::beginTimestampedSegment(Array *initData)
{
	int status = beginTreeTimestampedSegment(getNid(), initData->convertToDsc(), isCached(), getCachePolicy());
	if(!(status & 1))
		throw new TreeException(status);
}

void TreeNode::putTimestampedSegment(Array *data, Int64Array *times)
{
	int nTimesArray;
	_int64 *timesArray = times->getLongArray(&nTimesArray);
	int status = putTreeTimestampedSegment(getNid(), data->convertToDsc(), timesArray, isCached(), getCachePolicy());
	delete [] timesArray;
	if(!(status & 1))
		throw new TreeException(status);
}

void TreeNode::putRow(Data *data, Int64 *time)
{
	_int64 time64 = time->getLong();
	int status = putTreeRow(getNid(), data->convertToDsc(), &time64, isCached(), false, getCachePolicy());
	if(!(status & 1))
		throw new TreeException(status);
}

void TreeNode::acceptSegment(Array *data, Data *start, Data *end, Data *times)
{
	int status = beginTreeSegment(getNid(), data->convertToDsc(), start->convertToDsc(), 
		end->convertToDsc(), times->convertToDsc(), isCached(), getCachePolicy());
	if(!(status & 1))
		throw new TreeException(status);
}
void TreeNode::acceptRow(Data *data, _int64 time, bool isLast)
{
	int status = putTreeRow(getNid(), data->convertToDsc(), &time, isCached(), false, getCachePolicy());
	if(!(status & 1))
		throw new TreeException(status);
}



	
	
void CachedTreeNode::flush() {RTreeFlush(getNid());}

void CachedTree::open() 
{
	int status;
	
	RTreeConfigure(1, cacheSize);
	status = RTreeOpen(name, shot);
	if(!(status & 1))
	{
		throw new TreeException(status);
	}
}

void CachedTree::close()
{
	int status = RTreeClose(name, shot);
	if(!(status & 1))
	{
		throw new TreeException(status);
	}
}

CachedTreeNode *CachedTree::getCachedNode(char *path)
{
	int nid, status;

	status = TreeFindNode(path, &nid);
	if(!(status & 1))
	{
		throw new TreeException(status);
	}
	return new CachedTreeNode(nid, this);
}
void CachedTree::synch()
{
	RTreeSynch();
}

void MDSobjects::setActiveTree(Tree *tree)
{
	TreeSwitchDbid(tree->ctx);
}

Tree *MDSobjects::getActiveTree()
{
	char name[1024];
	int shot;
	int retNameLen, retShotLen;
	
	DBI_ITM dbiItems[] = {
		{1024, DbiNAME, name, &retNameLen},
		{sizeof(int), DbiSHOTID, &shot, &retShotLen},
		{0, DbiEND_OF_LIST, 0, 0}};
	int 
		status = TreeGetDbi(dbiItems);
	if(!(status & 1))
		throw new TreeException(status);
	return new Tree(name, shot);
}

ostream &operator<<(ostream &stream, TreeNode *treeNode)
{
	return stream << treeNode->getPath();
}
