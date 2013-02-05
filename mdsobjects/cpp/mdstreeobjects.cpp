#include <mdsobjects.h>
#include <usagedef.h>

//#include "mdstree.h"
using namespace MDSplus;

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

static int convertUsage(char *usage)
{
	char usageCode;
	if(!strcmp(usage, "ACTION"))
		usageCode = TreeUSAGE_ACTION;
	else if(!strcmp(usage, "ANY"))
		usageCode = TreeUSAGE_ANY;
	else if(!strcmp(usage, "AXIS"))
		usageCode = TreeUSAGE_AXIS;
	else if(!strcmp(usage, "COMPOUND_DATA"))
		usageCode = TreeUSAGE_COMPOUND_DATA;
	else if(!strcmp(usage, "DEVICE"))
		usageCode = TreeUSAGE_DEVICE;
	else if(!strcmp(usage, "DISPATCH"))
		usageCode = TreeUSAGE_DISPATCH;
	else if(!strcmp(usage, "STRUCTURE"))
		usageCode = TreeUSAGE_STRUCTURE;
	else if(!strcmp(usage, "NUMERIC"))
		usageCode = TreeUSAGE_NUMERIC;
	else if(!strcmp(usage, "SIGNAL"))
		usageCode = TreeUSAGE_SIGNAL;
	else if(!strcmp(usage, "SUBTREE"))
		usageCode = TreeUSAGE_SUBTREE;
	else if(!strcmp(usage, "TASK"))
		usageCode = TreeUSAGE_TASK;
	else if(!strcmp(usage, "TEXT"))
		usageCode = TreeUSAGE_TEXT;
	else if(!strcmp(usage, "WINDOW"))
		usageCode = TreeUSAGE_WINDOW;
	else
		usageCode = TreeUSAGE_ANY;

	return usageCode;
}



extern "C" void RTreeSynch();
extern "C" int _TreeOpen(void *dbid, char *tree, int shot, int readOnly);
extern "C" int _TreeClose(void *dbid, char *tree, int shot);
extern "C" int _RTreeOpen(void *dbid, char *tree, int shot);
extern "C" int _RTreeClose(void *dbid, char *tree, int shot);
extern "C" char *_RTreeSetCallback(void *dbid, int nid, void *argument, void (*callback)(int, void *));
extern "C" char _RTreeSetWarm(void *dbid, int nid, int warm);
extern "C" int _RTreeClearCallback(void *dbid, int nid, char *callbackDescr);
extern "C" int _TreeClose(void *dbid, char *tree, int shot);
extern "C" int _TreeFindNode(void *dbid, char *path, int *nid);
extern "C" int _TreeFindNodeWild(void *dbid, char *path, int *nid, void **ctx, int mask);
extern "C" int _TreeFindNodeEnd(void *dbid, void **ctx);
extern "C" char *_TreeGetPath(void *dbid, int nid);
extern "C" void TreeFree(void *ptr);
extern "C" int _TreeGetDbi(void *dbid, struct dbi_itm *itmlst);
extern "C" int TreeGetDbi(struct dbi_itm *itmlst);
extern "C" int _TreeSetDbi(void *dbid, struct dbi_itm *itmlst);
extern "C" int TreeSetDbi(struct dbi_itm *itmlst);
extern "C" int _TreeGetNci(void *dbid, int nid, struct nci_itm *itmlst);
extern "C" int _TreeSetNci(void *dbid, int nid, struct nci_itm *itmlst);
extern "C" int _TreeIsOn(void *dbid, int nid);
extern "C" int _TreeTurnOff(void *dbid, int nid);
extern "C" int _TreeTurnOn(void *dbid, int nid);
extern "C" void convertTime(void *dbid, int *time, char *retTime);
extern "C" int _TreeSetDefaultNid(void *dbid, int nid);
extern "C" int _TreeGetDefaultNid(void *dbid, int *nid);
extern "C" char *_TreeFindNodeTags(void *dbid, int nid, void *ctx);
extern "C" int TreeFindTagEnd(void *ctx);
extern "C" int TreeFindTag(char *tagnam, char *treename, int *tagidx);
extern "C" void *TreeSwitchDbid(void *dbid);
extern "C" void RTreeConfigure(int shared, int size);
extern "C" int _RTreeTerminateSegment(void *dbid, int nid);

extern "C" int getTreeData(void *dbid, int nid, void **data, void *tree);
extern "C" int putTreeData(void *dbid, int nid, void *data);
extern "C" int deleteTreeData(void *dbid, int nid);
extern "C" int doTreeMethod(void *dbid, int nid, char *method);
extern "C" int beginTreeSegment(void *dbid, int nid, void *dataDsc, void *startDsc, void *endDsc, 
								void *timeDsc);
extern "C" int makeTreeSegment(void *dbid, int nid, void *dataDsc, void *startDsc, void *endDsc, 
								void *timeDsc, int rowsFilled);
extern "C" int putTreeSegment(void *dbid, int nid, void *dataDsc, int ofs);
extern "C" int updateTreeSegment(void *dbid, int nid, void *startDsc, void *endDsc, 
								void *timeDsc);
extern "C" int getTreeNumSegments(void *dbid, int nid, int *numSegments); 
extern "C" int getTreeSegmentLimits(void *dbid, int nid, int idx, void **startDsc, void **endDsc);
extern "C" int getTreeSegment(void *dbid, int nid, int segIdx, void **dataDsc, void **timeDsc);
extern "C" int setTreeTimeContext(void *startDsc, void *endDsc, void *deltaDsc); 
extern "C" int beginTreeTimestampedSegment(void *dbid, int nid, void *dataDsc);
extern "C" int putTreeTimestampedSegment(void *dbid, int nid, void *dataDsc, _int64 *times);
extern "C" int makeTreeTimestampedSegment(void *dbid, int nid, void *dataDsc, _int64 *times, int rowsFilled);
extern "C" int putTreeRow(void *dbid, int nid, void *dataDsc, _int64 *time, int size);
extern "C" int LibConvertDateString(char *asc_time, _int64 *qtime);
extern "C" int TreeSetViewDate(_int64 *date);

extern "C" const char *MdsClassString(int id);
extern "C" const char *MdsDtypeString(int id);
extern "C" int  TreeSetCurrentShotId(char *experiment, int shot);
extern "C" int  TreeGetCurrentShotId(char *experiment);
extern "C" int _TreeDeletePulseFile(void *dbid, int shotid, int allfiles);
extern "C" int _TreeCreatePulseFile(void *dbid, int shotid, int numnids_in, int *nids_in);
extern "C" char *_TreeFindTagWild(void *dbid, char *wild, int *nidout, void **ctx_inout);
extern "C" int _TreeOpenEdit(void **dbid, char *tree_in, int shot_in);
extern "C" int _TreeOpenNew(void **dbid, char *tree_in, int shot_in);
extern "C" int _TreeWriteTree(void **dbid, char *exp_ptr, int shotid);
extern "C" int _TreeQuitTree(void **dbid, char *exp_ptr, int shotid);
extern "C" int _TreeAddNode(void *dbid, char *name, int *nid_out, char usage);
extern "C" int _TreeDeleteNodeInitialize(void *dbid, int nidin, int *count, int reset);
extern "C" void _TreeDeleteNodeExecute(void *dbid);
extern "C" int _TreeSetDefaultNid(void *dbid, int nid_in);
extern "C" int _TreeGetDefaultNid(void *dbid, int *nid_in);
extern "C" int _TreeAddConglom(void *dbid, char *path, char *congtype, int *nid);
extern "C" int _TreeRenameNode(void *dbid, int nid, char *newname);
extern "C" int _TreeAddTag(void *dbid, int nid_in, char *tagnam);
extern "C" int _TreeRemoveTag(void *dbid, char *name);
extern "C" int _RTreeFlushNode(void *dbid, int nid);
extern "C" int _TreeSetSubtree(void *dbid, int nid);
extern "C" int _TreeSetNoSubtree(void *dbid, int nid);
extern "C" _int64 _TreeGetDatafileSize(void *dbid);
extern "C" void TreeFreeDbid(void *);

#ifdef HAVE_WINDOWS_H
#include <Windows.h>
static HANDLE semH = 0;
void Tree::lock() 
{
	if(!semH)
		semH = CreateSemaphore(NULL, 1, 16, NULL);
	if(!semH)
		throw new MdsException("Cannot create lock semaphore");
    WaitForSingleObject(semH, INFINITE);      
}
void Tree::unlock()
{
	if(semH)
		ReleaseSemaphore(semH, 1, NULL);
}
#else
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <semaphore.h>
static 	sem_t semStruct;
static bool semInitialized = false;


void Tree::lock() 
{
	if(!semInitialized)
	{
		semInitialized = true;
		int status = sem_init(&semStruct, 0, 1);
		if(status != 0)
			throw new MdsException("Cannot create lock semaphore");
	}
	sem_wait(&semStruct);
}
void Tree::unlock()
{
	sem_post(&semStruct);
}
#endif


		
Tree::Tree(char *name, int shot)
{
	this->shot = shot;
	ctx = 0;
	int status = _TreeOpen(&ctx, name, shot, 0);
	if(!(status & 1))
	{
		throw new MdsException(status);
	}
	this->name = new char[strlen(name) + 1];
	strcpy(this->name, name);
	//setActiveTree(this);
}

Tree::Tree(void *dbid, char *name, int shot)
{
	this->shot = shot;
	this->name = new char[strlen(name) + 1];
	strcpy(this->name, name);
	this->ctx = dbid;
}

Tree::Tree(char *name, int shot, char *mode)
{
	this->shot = shot;
	ctx = 0;
	int len = strlen(mode);
	char *upMode = new char[len+1];
	for(int i = 0; i < len; i++)
		upMode[i] = toupper(mode[i]);
	upMode[len] = 0;
	int status = 0;
	if(!strcmp(upMode, "NORMAL"))
		status = _TreeOpen(&ctx, name, shot, 0);
	else if(!strcmp(upMode, "READONLY"))
		status = _TreeOpen(&ctx, name, shot, 0);
	else if(!strcmp(upMode, "NEW"))
		status = _TreeOpenNew(&ctx, name, shot);
	else if(!strcmp(upMode, "EDIT"))
		status = _TreeOpenEdit(&ctx, name, shot);

	else
		throw new MdsException("Invalid Open mode");
	if(!(status & 1))
	{
		throw new MdsException(status);
	}
	this->name = new char[strlen(name) + 1];
	strcpy(this->name, name);
	delete [] upMode;
}


Tree::~Tree()
{
    int status = _TreeClose(&ctx, name, shot);
    if(status & 1)
    	TreeFreeDbid(ctx);
    delete [] name;
}

EXPORT void *Tree::operator new(size_t sz)
{
	return ::operator new(sz);
}
EXPORT void Tree::operator delete(void *p)
{
	::operator delete(p);
}


void Tree::edit()
{
	int status = _TreeOpenEdit(&ctx, name, shot);
	if(!(status & 1))
	{
		throw new MdsException(status);
	}
}

Tree *Tree::create(char *name, int shot)
{
	void *dbid;
	int status = _TreeOpenNew(&dbid, name, shot);
	if(!(status & 1))
	{
		throw new MdsException(status);
	}
	return new Tree(dbid, name, shot);
}

void Tree::write()
{
	int status = _TreeWriteTree(&ctx, name, shot);
	if(!(status & 1))
	{
		throw new MdsException(status);
	}
}
void Tree::quit()
{
	int status = _TreeQuitTree(&ctx, name, shot);
	if(!(status & 1))
	{
		throw new MdsException(status);
	}
}


TreeNode *Tree::addNode(char *name, char *usage)
{

	int newNid;
	int status = _TreeAddNode(ctx, name, &newNid, convertUsage(usage));
	if(!(status & 1))
		throw new MdsException(status);
	return new TreeNode(newNid, this);
}


TreeNode *Tree::addDevice(char *name, char *type)
{
	int newNid;
	int status = _TreeAddConglom(ctx, name, type, &newNid);
	if(!(status & 1))
		throw new MdsException(status);
	return new TreeNode(newNid, this);
}



void Tree::remove(char *name)
{
	int count;
	TreeNode *delNode = getNode(name);
	int status = _TreeDeleteNodeInitialize(ctx, delNode->getNid(), &count, 1);
	delete delNode;
	if(status & 1)_TreeDeleteNodeExecute(ctx);
	if(!(status & 1))
		throw new MdsException(status);
}







TreeNode *Tree::getNode(char *path)
{
	int nid, status;

	status = _TreeFindNode(ctx, path, &nid);
	if(!(status & 1))
	{
		throw new MdsException(status);
	}
	return new TreeNode(nid, this);
}

	
TreeNode *Tree::getNode(TreePath *path)
{
	int nid, status;
	char *pathName = path->getString();
	status = _TreeFindNode(ctx, pathName, &nid);
	delete[] pathName;
	if(!(status & 1))
	{
		throw new MdsException(status);
	}
	return new TreeNode(nid, this);
}

	
TreeNode *Tree::getNode(String *path)
{
	int nid, status;
	char *pathName = path->getString();
	status = _TreeFindNode(ctx, pathName, &nid);
	delete[] pathName;
	if(!(status & 1))
	{
		throw new MdsException(status);
	}
	return new TreeNode(nid, this);
}

	

TreeNodeArray *Tree::getNodeWild(char *path, int usageMask)
{
	int currNid, status; 
	int numNids = 0;
	void *wildCtx = 0;

	while ((status = _TreeFindNodeWild(ctx, path,&currNid,&wildCtx, usageMask)) & 1)
		numNids++;
	_TreeFindNodeEnd(ctx, &wildCtx);

	//printf("%s\n", MdsGetMsg(status));

	TreeNode **retNodes = new TreeNode *[numNids];
	wildCtx = 0;
	for(int i = 0; i < numNids; i++)
	{
		_TreeFindNodeWild(ctx, path,&currNid,&wildCtx, usageMask);
		retNodes[i] = new TreeNode(currNid, this);
	}
	_TreeFindNodeEnd(ctx, &wildCtx);
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
	int status = _TreeSetDefaultNid(ctx, treeNode->getNid());
	if(!(status & 1)) 
		throw new MdsException(status);
}

TreeNode *Tree::getDefault()
{
	int nid;

	int status = _TreeGetDefaultNid(ctx, &nid);
	if(!(status & 1)) 
		throw new MdsException(status);
	return new TreeNode(nid, this);
}

bool Tree::versionsInPulseEnabled()
{
	int supports, len, status;
	struct dbi_itm dbiList[] = 
	{{sizeof(int), DbiVERSIONS_IN_PULSE, &supports, &len},
	{0, DbiEND_OF_LIST,0,0}};

	status = _TreeGetDbi(ctx, dbiList);
	if(!(status & 1)) 
		throw new MdsException(status);
	return (supports)?true:false;
}

bool Tree::versionsInModelEnabled()
{
	int supports, len, status;
	struct dbi_itm dbiList[] = 
	{{sizeof(int), DbiVERSIONS_IN_MODEL, &supports, &len},
	{0, DbiEND_OF_LIST,0,0}};

	status = _TreeGetDbi(ctx, dbiList);
	if(!(status & 1)) 
		throw new MdsException(status);
	return (supports)?true:false;
}

bool Tree::isModified()
{
	int modified, len, status;
	struct dbi_itm dbiList[] = 
	{{sizeof(int), DbiMODIFIED, &modified, &len},
	{0, DbiEND_OF_LIST,0,0}};

	status = _TreeGetDbi(ctx, dbiList);
	if(!(status & 1)) 
		throw new MdsException(status);
	return (modified)?true:false;
}

bool Tree::isOpenForEdit()
{
	int edit, len, status;
	struct dbi_itm dbiList[] = 
	{{sizeof(int), DbiOPEN_FOR_EDIT, &edit, &len},
	{0, DbiEND_OF_LIST,0,0}};

	status = _TreeGetDbi(ctx, dbiList);
	if(!(status & 1)) 
		throw new MdsException(status);
	return (edit)?true:false;
}

bool Tree::isReadOnly()
{
	int readOnly, len, status;
	struct dbi_itm dbiList[] = 
	{{sizeof(int), DbiOPEN_READONLY, &readOnly, &len},
	{0, DbiEND_OF_LIST,0,0}};

	status = _TreeGetDbi(ctx, dbiList);
	if(!(status & 1)) 
		throw new MdsException(status);
	return (readOnly)?true:false;
}

void Tree::setVersionsInModel(bool verEnabled)
{
	int supports, len, status;
	struct dbi_itm dbiList[] = 
	{{sizeof(int), DbiVERSIONS_IN_MODEL, &supports, &len},
	{0, DbiEND_OF_LIST,0,0}};

	supports = (verEnabled)?1:0;
	status = _TreeSetDbi(ctx, dbiList);
	if(!(status & 1)) 
		throw new MdsException(status);
}

void Tree::setVersionsInPulse(bool verEnabled)
{
	int supports, len, status;
	struct dbi_itm dbiList[] = 
	{{sizeof(int), DbiVERSIONS_IN_PULSE, &supports, &len},
	{0, DbiEND_OF_LIST,0,0}};

	supports = (verEnabled)?1:0;
	status = _TreeSetDbi(ctx, dbiList);
	if(!(status & 1)) 
		throw new MdsException(status);
}


void Tree::setViewDate(char *date)
{
	_int64  qtime;

	int status = LibConvertDateString(date, &qtime);
	if(!(status & 1))
		throw new MdsException("Invalid date format");
	status = TreeSetViewDate(&qtime);
	if(!(status & 1)) 
		throw new MdsException(status);
}


void Tree::setTimeContext(Data *start, Data *end, Data *delta)
{
	int status = setTreeTimeContext((start)?start->convertToDsc():0, (end)?end->convertToDsc():0, 
		(delta)?delta->convertToDsc():0);
	if(!(status & 1))
		throw new MdsException(status);
}



void Tree::setCurrent(char *treeName, int shot)
{
	int status = TreeSetCurrentShotId(treeName, shot);
	if(!(status & 1))
		throw new MdsException(status);
}

int Tree::getCurrent(char *treeName)
{
	return TreeGetCurrentShotId(treeName);
}

void Tree::createPulse(int shot)
{
	int  retNids;
	int status = _TreeCreatePulseFile(getCtx(), shot, 0, &retNids);
	if(!(status & 1))
		throw new MdsException(status);
}

void Tree::deletePulse(int shot)
{
	int status = _TreeDeletePulseFile(getCtx(), shot, 1);
	if(!(status & 1))
		throw new MdsException(status);
}



StringArray *Tree::findTags(char *wild)
{
	const int MAX_TAGS = 1024;
	char *tagNames[MAX_TAGS];
	void *ctx = 0;
	int nTags = 0;
	int nidOut;
	for(nTags = 0; nTags < MAX_TAGS; nTags++)
	{
	    char *currTag = _TreeFindTagWild(getCtx(), wild, &nidOut, &ctx);
	    if(!currTag) break;
	    tagNames[nTags] = new char[strlen(currTag) + 1];
	    strcpy(tagNames[nTags], currTag);
	}
	StringArray *stArr = new StringArray(tagNames, nTags);
	for(int i = 0; i < nTags; i++)
	    delete [] tagNames[i];
	return stArr;
}
void Tree::removeTag(char *tagName)
{

	int status = _TreeRemoveTag(getCtx(), tagName);
	if(!(status & 1))
		throw new MdsException(status);
}

_int64 Tree::getDatafileSize()
{
	_int64 size = _TreeGetDatafileSize(getCtx());
	if(size == -1)
		throw new MdsException("Cannot retrieve datafile size");
	return size;
}
//////////////////////TreeNode Methods/////////////////


void *TreeNode::convertToDsc()
{
	Tree::lock();
	setActiveTree(tree);
	void *retDsc = completeConversionToDsc(convertToScalarDsc(clazz, dtype, sizeof(int), (char *)&nid));
	Tree::unlock();
	return retDsc;
}

int TreeNode::getFlag(int flagOfs)
{
	int nciFlags;
	int nciFlagsLen = sizeof(int);
	struct nci_itm nciList[] =  {{4, NciGET_FLAGS, &nciFlags, &nciFlagsLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);

	if(!(status & 1))
		throw new MdsException(status);
	return (nciFlags & flagOfs)?true:false;
}

void TreeNode::setFlag(int flagOfs, bool flag)
{
	int nciFlags;
	int status;
	int nciFlagsLen = sizeof(int);
	struct nci_itm setNciList[] =  {{4, NciSET_FLAGS, &nciFlags, &nciFlagsLen},
		{0, NciEND_OF_LIST, 0, 0}};
	struct nci_itm clearNciList[] =  {{4, NciCLEAR_FLAGS, &nciFlags, &nciFlagsLen},
		{0, NciEND_OF_LIST, 0, 0}};
	resolveNid();
	nciFlags = flagOfs;
	if(flag)
	    status = _TreeSetNci(tree->getCtx(), nid, setNciList);
	else
	    status = _TreeSetNci(tree->getCtx(), nid, clearNciList);
	if(!(status & 1))
		throw new MdsException(status);
}

TreeNode::TreeNode(int nid, Tree *tree, Data *units, Data *error, Data *help, Data *validation)
{
	if(!tree)
		tree = getActiveTree();
	if(!tree)
		throw new MdsException("A Tree instance must be defined when ceating TreeNode instances");
	this->nid = nid;
	this->tree = tree;
	clazz = CLASS_S;
	dtype = DTYPE_NID;
	setAccessory(units, error, help, validation);
}

EXPORT void *TreeNode::operator new(size_t sz)
{
	return ::operator new(sz);
}
EXPORT void TreeNode::operator delete(void *p)
{
	::operator delete(p);
}
		
char *TreeNode::getPath()
{
	resolveNid();
	char *currPath = _TreeGetPath(tree->getCtx(), nid);
	char *path = new char[strlen(currPath) + 1];
	strcpy(path, currPath);
	TreeFree(currPath);
	return path;
}


char *TreeNode::getMinPath()
{
	char path[1024];
	int pathLen = 1024;
	struct nci_itm nciList[] = 
		{{511, NciMINPATH, path, &pathLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new MdsException(status);
	path[pathLen] = 0;
	char *retPath = new char[strlen(path)+1];
	strcpy(retPath, path);
	return retPath;
}

char *TreeNode::getFullPath()
{
	char path[1024];
	int pathLen = 1024;
	struct nci_itm nciList[] = 
		{{511, NciFULLPATH, path, &pathLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new MdsException(status);
	path[pathLen] = 0;
	char *retPath = new char[strlen(path)+1];
	strcpy(retPath, path);
	return retPath;
}

char *TreeNode::getOriginalPartName()
{
	char path[1024];
	int pathLen = 1024;
	struct nci_itm nciList[] = 
		{{511, NciORIGINAL_PART_NAME, path, &pathLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new MdsException(status);
	path[pathLen] = 0;
	char *retPath = new char[strlen(path)+1];
	strcpy(retPath, path);
	return retPath;
}

char *TreeNode::getNodeName()
{
	char path[1024];
	int pathLen = 1024;
	struct nci_itm nciList[] = 
		{{511, NciNODE_NAME, path, &pathLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new MdsException(status);
	path[pathLen] = 0;
	char *retPath = new char[strlen(path)+1];
	strcpy(retPath, path);
	//Trim
	for(int i = strlen(retPath) - 1; i > 0 && retPath[i] == ' '; i--)
			retPath[i] = 0;
	return retPath;
}


Data *TreeNode::getData()
{
	Data *data = 0;
	resolveNid();
	int status = getTreeData(tree->getCtx(), nid, (void **)&data, tree);
	if(!(status & 1))
	{
		throw new MdsException(status);
	}
	return data;
}

void TreeNode::putData(Data *data)
{
	resolveNid();
	int status = putTreeData(tree->getCtx(), nid, (void *)data);
	if(!(status & 1))
	{
		throw new MdsException(status);
	}
}
void TreeNode::deleteData()
{
	resolveNid();
	int status = deleteTreeData(tree->getCtx(), nid);
	if(!(status & 1))
	{
		throw new MdsException(status);
	}
}



bool TreeNode::isOn()
{
	resolveNid();
	bool retOn = (_TreeIsOn(tree->getCtx(), nid) & 1)?true:false;
	return  retOn;
}
void TreeNode::setOn(bool on)
{
	resolveNid();
	if(on)
		_TreeTurnOn(tree->getCtx(), nid);
	else
		_TreeTurnOff(tree->getCtx(), nid);
}

int TreeNode::getLength()
{
	int length;
	int lengthLen = sizeof(int);
	struct nci_itm nciList[] = 
		{{4, NciLENGTH, &length, &lengthLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new MdsException(status);
	return length;
}
int TreeNode::getCompressedLength()
{
	int length;
	int lengthLen = sizeof(int);
	struct nci_itm nciList[] = 
		{{4, NciRLENGTH, &length, &lengthLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new MdsException(status);
	return length;
}

int TreeNode::getStatus()
{
	int id;
	int idLen = sizeof(int);
	struct nci_itm nciList[] = 
		{{4, NciSTATUS, &id, &idLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new MdsException(status);
	return id;
}

int TreeNode::getOwnerId()
{
	int id;
	int idLen = sizeof(int);
	struct nci_itm nciList[] = 
		{{4, NciOWNER_ID, &id, &idLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new MdsException(status);
	return id;
}

_int64 TreeNode::getTimeInserted()
{
	_int64 timeInserted; 
	int timeLen;
	struct nci_itm nciList[] = 
		{{8, NciTIME_INSERTED, (char *)&timeInserted, &timeLen},
		{NciEND_OF_LIST, 0, 0, 0}};
	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new MdsException(status);
	return timeInserted;
}

void TreeNode::doMethod(char *method)
{
	resolveNid();
	int status = doTreeMethod(tree->getCtx(), nid, method);
	if(!(status & 1))
		throw new MdsException(status);
}

bool TreeNode::isSetup()
{
	return getFlag(NciM_SETUP_INFORMATION)?true:false;
}


bool TreeNode::isWriteOnce()
{
	return getFlag(NciM_WRITE_ONCE)?true:false;
}

void TreeNode::setWriteOnce(bool flag)
{
	setFlag(NciM_WRITE_ONCE, flag);
}

bool TreeNode::isCompressOnPut()
{
	return getFlag(NciM_COMPRESS_ON_PUT)?true:false;
}

void TreeNode::setCompressOnPut(bool flag)
{
	setFlag(NciM_COMPRESS_ON_PUT, flag);
}
bool TreeNode::isNoWriteModel()
{
	return getFlag(NciM_NO_WRITE_MODEL)?true:false;
}

void TreeNode::setNoWriteModel(bool flag)
{
	setFlag(NciM_NO_WRITE_MODEL, flag);
}

bool TreeNode::isEssential()
{
	return getFlag(NciM_ESSENTIAL)?true:false;
}

void TreeNode::setEssential(bool flag)
{
	setFlag(NciM_ESSENTIAL, flag);
}


bool TreeNode::isNoWriteShot()
{
	return getFlag(NciM_NO_WRITE_SHOT)?true:false;
}

void TreeNode::setNoWriteShot(bool flag)
{
	setFlag(NciM_NO_WRITE_SHOT, flag);
}

bool TreeNode::isIncludedInPulse()
{
	return getFlag(NciM_INCLUDE_IN_PULSE)?true:false;
}
bool TreeNode::containsVersions()
{
	return getFlag(NciM_VERSIONS)?true:false;
}
bool TreeNode::isMember()
{
	int parLen = 4;
	int par;
	struct nci_itm nciList[] = 
		{{4, NciPARENT_RELATIONSHIP, &par, &parLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new MdsException(status);
	
	return  (par & NciK_IS_MEMBER)?true:false;
}

bool TreeNode::isChild()
{
	int parLen = 4;
	int par;
	struct nci_itm nciList[] = 
		{{4, NciPARENT_RELATIONSHIP, &par, &parLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new MdsException(status);
	
	return  (par & NciK_IS_CHILD)?true:false;
}

void TreeNode::setIncludedInPulse(bool flag)
{
	setFlag(NciM_INCLUDE_IN_PULSE, flag);
}

TreeNode *TreeNode::getParent()
{

	int nidLen = 4;
	int parentNid;
	struct nci_itm nciList[] = 
		{{4, NciPARENT, &parentNid, &nidLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new MdsException(status);

	return new TreeNode(parentNid, tree);
}

TreeNode *TreeNode::getBrother()
{

	int nidLen = 4;
	int brotherNid;
	struct nci_itm nciList[] = 
		{{4, NciBROTHER, &brotherNid, &nidLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new MdsException(status);

	return new TreeNode(brotherNid, tree);
}

TreeNode *TreeNode::getChild()
{

	int nidLen = 4;
	int childNid;
	struct nci_itm nciList[] = 
		{{4, NciCHILD, &childNid, &nidLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new MdsException(status);

	return new TreeNode(childNid, tree);
}

TreeNode *TreeNode::getMember()
{

	int nidLen = 4;
	int memberNid;
	struct nci_itm nciList[] = 
		{{4, NciCHILD, &memberNid, &nidLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new MdsException(status);

	return new TreeNode(memberNid, tree);
}


int TreeNode::getNumChildren()
{
	int numLen = 4;
	int nChildren;
	struct nci_itm nciList[] = 
	{{4, NciNUMBER_OF_CHILDREN, &nChildren, &numLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new MdsException(status);
	return nChildren;
}

int TreeNode::getNumMembers()
{
	int numLen = 4;
	int nChildren;
	struct nci_itm nciList[] = 
	{{4, NciNUMBER_OF_MEMBERS, &nChildren, &numLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new MdsException(status);
	return nChildren;
}

int TreeNode::getNumDescendants()
{
	int numLen = 4;
	int numMembers, nChildren;
	struct nci_itm nciList[] = 
		{{4, NciNUMBER_OF_MEMBERS, &numMembers, &numLen},
		{4, NciNUMBER_OF_CHILDREN, &nChildren, &numLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new MdsException(status);

	return numMembers+nChildren;
}


TreeNode **TreeNode::getDescendants(int *numDescendants)
{

	int numLen = 4;
	int numMembers, nChildren;
	struct nci_itm nciList[] = 
		{{4, NciNUMBER_OF_MEMBERS, &numMembers, &numLen},
		{4, NciNUMBER_OF_CHILDREN, &nChildren, &numLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
	{
		throw new MdsException(status);
	}

	int *childrenNids = new int[numMembers+nChildren];

	int retLen = sizeof(int) * (numMembers+nChildren);
	struct nci_itm nciList1[] = 
		{{4, NciMEMBER_NIDS, &childrenNids[0], &retLen},
		{4, NciCHILDREN_NIDS, &childrenNids[numMembers], &retLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
	{
		delete [] childrenNids;
		throw new MdsException(status);
	}
	TreeNode **retChildren = new TreeNode* [numMembers+nChildren];
	for(int i = 0; i < numMembers+nChildren; i++)
		retChildren[i] = new TreeNode(childrenNids[i], tree);

	delete[] childrenNids;
	*numDescendants = numMembers+nChildren;
	return retChildren;
}

TreeNode **TreeNode::getChildren(int *numChildren)
{

	int numLen = 4;
	int nChildren;
	struct nci_itm nciList[] = 
	{{4, NciNUMBER_OF_CHILDREN, &nChildren, &numLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
	{
		throw new MdsException(status);
	}

	int *childrenNids = new int[nChildren];

	int retLen = sizeof(int) * (nChildren);
	struct nci_itm nciList1[] = 
		{{4, NciCHILDREN_NIDS, &childrenNids[0], &retLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
	{
		delete [] childrenNids;
		throw new MdsException(status);
	}
	TreeNode **retChildren = new TreeNode* [nChildren];
	for(int i = 0; i < nChildren; i++)
		retChildren[i] = new TreeNode(childrenNids[i], tree);

	delete[] childrenNids;
	*numChildren = nChildren;
	return retChildren;
}

TreeNode **TreeNode::getMembers(int *numMembers)
{

	int numLen = 4;
	int nMembers;
	struct nci_itm nciList[] = 
	{{4, NciNUMBER_OF_MEMBERS, &nMembers, &numLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
	{
		throw new MdsException(status);
	}

	int *memberNids = new int[nMembers];

	int retLen = sizeof(int) * (nMembers);
	struct nci_itm nciList1[] = 
		{{4, NciMEMBER_NIDS, &memberNids[0], &retLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
	{
		delete [] memberNids;
		throw new MdsException(status);
	}
	TreeNode **retMembers = new TreeNode* [nMembers];
	for(int i = 0; i < nMembers; i++)
		retMembers[i] = new TreeNode(memberNids[i], tree);

	delete[] memberNids;
	*numMembers = nMembers;
	return retMembers;
}



const char *TreeNode::getClass()
{

	int clsLen = 1;
	char cls;
	struct nci_itm nciList[] = 
		{{1, NciCLASS, &cls, &clsLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new MdsException(status);

	return MdsClassString(cls);
}
	
const char *TreeNode::getDType()
{

	int clsLen = 1;
	char cls;
	struct nci_itm nciList[] = 
		{{1, NciDTYPE, &cls, &clsLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new MdsException(status);

	return MdsDtypeString(cls);
}
	
const char *TreeNode::getUsage()
{

	int usageLen = 4;
	int usage = 0;
	struct nci_itm nciList[] = 
		{{4, NciUSAGE, &usage, &usageLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new MdsException(status);

	switch(usage)  {
		case TreeUSAGE_ACTION: return "ACTION";
		case TreeUSAGE_ANY: return "ANY";
		case TreeUSAGE_AXIS: return "AXIS";
		case TreeUSAGE_COMPOUND_DATA: return "COMPOUND_DATA";
		case TreeUSAGE_DEVICE: return "DEVICE";
		case TreeUSAGE_DISPATCH: return "DISPATCH";
		case TreeUSAGE_STRUCTURE: return "STRUCTURE";
		case TreeUSAGE_NUMERIC: return "NUMERIC";
		case TreeUSAGE_SIGNAL: return "SIGNAL";
		case TreeUSAGE_SUBTREE: return "SUBTREE";
		case TreeUSAGE_TASK: return "TASK";
		case TreeUSAGE_TEXT: return "TEXT";
		case TreeUSAGE_WINDOW: return "WINDOW";
	}
	return "Unknown";
}

int TreeNode::getConglomerateElt()
{

	int eltLen;
	int elt = 0;
	struct nci_itm nciList[] = 
		{{1, NciCONGLOMERATE_ELT, (char *)&elt, &eltLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new MdsException(status);

	return elt;
}
int TreeNode::getNumElts()
{

	int nNidsLen;
	int nNids;
	struct nci_itm nciList[] = 
		{{4, NciNUMBER_OF_ELTS, (char *)&nNids, &nNidsLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new MdsException(status);
	return nNidsLen;
}

	
TreeNodeArray *TreeNode::getConglomerateNodes()
{

	int nNidsLen, retLen;
	int nNids;
	struct nci_itm nciList[] = 
		{{4, NciNUMBER_OF_ELTS, (char *)&nNids, &nNidsLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
	{
		throw new MdsException(status);
	}

	int *nids = new int[nNids];
	struct nci_itm nciList1[] = 
		{{4*nNids, NciCONGLOMERATE_ELT, (char *)nids, &retLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	status = _TreeGetNci(tree->getCtx(), nid, nciList1);
	if(!(status & 1))
		throw new MdsException(status);

	TreeNodeArray *resArray = new TreeNodeArray(nids, nNids, tree);
	delete [] nids;
	return resArray;
}
	
int TreeNode::getDepth()
{

	int depthLen;
	int depth;
	struct nci_itm nciList[] = 
		{{1, NciDEPTH, (char *)&depth, &depthLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw new MdsException(status);

	return depth;
}
#ifdef HAVE_WINDOWS_H
#define pthread_mutex_t int
static void LockMdsShrMutex(){}
static void UnlockMdsShrMutex(){}
#endif
#ifdef HAVE_VXWORKS_H
#define pthread_mutex_t int
static void LockMdsShrMutex(){}
static void UnlockMdsShrMutex(){}
#endif
	

void TreeNode::makeSegment(Data *start, Data *end, Data *time, Array *initialData)
{
	resolveNid();
	int numDims;
	int *shape = initialData->getShape(&numDims);
	//if(tree) tree->lock();
	int status = makeTreeSegment(tree->getCtx(), getNid(), initialData->convertToDsc(), start->convertToDsc(), 
		end->convertToDsc(), time->convertToDsc(), shape[0]);
	deleteNativeArray(shape);
	//if(tree) tree->unlock();
	if(!(status & 1))
		throw new MdsException(status);
}

void TreeNode::beginSegment(Data *start, Data *end, Data *time, Array *initialData)
{
	resolveNid();
	//if(tree) tree->lock();
	int status = beginTreeSegment(tree->getCtx(), getNid(), initialData->convertToDsc(), start->convertToDsc(), 
		end->convertToDsc(), time->convertToDsc());
	//if(tree) tree->unlock();
	if(!(status & 1))
		throw new MdsException(status);
}

void TreeNode::putSegment(Array *data, int ofs)
{
	resolveNid();
	//if(tree) tree->lock();
	int status = putTreeSegment(tree->getCtx(), getNid(), data->convertToDsc(), ofs);
	//if(tree) tree->unlock();
	if(!(status & 1))
		throw new MdsException(status);
}

void TreeNode::updateSegment(Data *start, Data *end, Data *time)
{
	resolveNid();
	//if(tree) tree->lock();
	int status = updateTreeSegment(tree->getCtx(), getNid(), start->convertToDsc(), 
		end->convertToDsc(), time->convertToDsc());
	//if(tree) tree->unlock();
	if(!(status & 1))
		throw new MdsException(status);
}

int TreeNode::getNumSegments()
{
	int numSegments;

	resolveNid();
	//if(tree) tree->lock();
	int status = getTreeNumSegments(tree->getCtx(), getNid(), &numSegments);
	//if(tree) tree->unlock();
	if(!(status & 1))
		throw new MdsException(status);
	return  numSegments;
}

void TreeNode::getSegmentLimits(int segmentIdx, Data **start, Data **end)
{
	void *startDsc, *endDsc;
	resolveNid();
	//if(tree) tree->lock();
	int status = getTreeSegmentLimits(tree->getCtx(), getNid(), segmentIdx, &startDsc, &endDsc);
	//if(tree) tree->unlock();
	if(!(status & 1))
		throw new MdsException(status);
	*start = (Data*)convertFromDsc(startDsc);
	freeDsc(startDsc);
	*end = (Data*)convertFromDsc(endDsc);
	freeDsc(endDsc);
}
	
Array *TreeNode::getSegment(int segIdx)
{
	void *dataDsc;
	void *timeDsc;
	
	resolveNid();
	//if(tree) tree->lock();
	int status = getTreeSegment(tree->getCtx(), getNid(), segIdx, &dataDsc, &timeDsc);
	//if(tree) tree->unlock();
	if(!(status & 1))
	{
		freeDsc(dataDsc);
		freeDsc(timeDsc);
		throw new MdsException(status);
	}
	Array *retData = (Array *)convertFromDsc(dataDsc);
	freeDsc(dataDsc);
	freeDsc(timeDsc);
	return retData;
}


void TreeNode::beginTimestampedSegment(Array *initData)
{
	resolveNid();
	//if(tree) tree->lock();
	int status = beginTreeTimestampedSegment(tree->getCtx(), getNid(), initData->convertToDsc());
	//if(tree) tree->unlock();
	if(!(status & 1))
		throw new MdsException(status);
}

void TreeNode::makeTimestampedSegment(Array *data, _int64 *times)
{
	int nTimesArray;
	int numDims;
	int *shape = data->getShape(&numDims);

	resolveNid();
	//if(tree) tree->lock();
	int status = makeTreeTimestampedSegment(tree->getCtx(), getNid(), data->convertToDsc(), times, shape[0]);
	//if(tree) tree->unlock();
	deleteNativeArray(shape);
	if(!(status & 1))
		throw new MdsException(status);
}

void TreeNode::putTimestampedSegment(Array *data, _int64 *times)
{
	int nTimesArray;
	resolveNid();
	//if(tree) tree->lock();
	int status = putTreeTimestampedSegment(tree->getCtx(), getNid(), data->convertToDsc(), times);
	//if(tree) tree->unlock();
	if(!(status & 1))
		throw new MdsException(status);
}

void TreeNode::putRow(Data *data, _int64 *time, int size)
{
	resolveNid();
	//if(tree) tree->lock();
	int status = putTreeRow(tree->getCtx(), getNid(), data->convertToDsc(), time, size);
	//if(tree) tree->unlock();
	if(!(status & 1))
		throw new MdsException(status);
}

void TreeNode::acceptSegment(Array *data, Data *start, Data *end, Data *times)
{
	resolveNid();
	int status = beginTreeSegment(tree->getCtx(), getNid(), data->convertToDsc(), start->convertToDsc(), 
		end->convertToDsc(), times->convertToDsc());
	if(!(status & 1))
		throw new MdsException(status);
}
void TreeNode::acceptRow(Data *data, _int64 time, bool isLast)
{
	resolveNid();
	int status = putTreeRow(tree->getCtx(), getNid(), data->convertToDsc(), &time, 1024);
	if(!(status & 1))
		throw new MdsException(status);
}


TreeNode *TreeNode::getNode(char *relPath)
{
	int defNid;
	int newNid;
	resolveNid();
	int status = _TreeGetDefaultNid(tree->getCtx(), &defNid);
	if(status & 1) status = _TreeSetDefaultNid(tree->getCtx(), nid);
	if(status & 1) status = _TreeFindNode(tree->getCtx(), relPath, &newNid);
	if(status & 1) status = _TreeSetDefaultNid(tree->getCtx(), defNid);
	if(!(status & 1))
		throw new MdsException(status);
	return new TreeNode(newNid, tree);
}

TreeNode *TreeNode::getNode(String *relPathStr)
{
	int defNid;
	int newNid;
	resolveNid();
	char *relPath = relPathStr->getString();
	int status = _TreeGetDefaultNid(tree->getCtx(), &defNid);
	if(status & 1) status = _TreeSetDefaultNid(tree->getCtx(), nid);
	if(status & 1) status = _TreeFindNode(tree->getCtx(), relPath, &newNid);
	if(status & 1) status = _TreeSetDefaultNid(tree->getCtx(), defNid);
	if(!(status & 1))
		throw new MdsException(status);
	delete [] relPath;
	return new TreeNode(newNid, tree);
}



TreeNode *TreeNode::addNode(char *name, char *usage)
{

	int defNid;
	int newNid;
	resolveNid();
	int status = _TreeGetDefaultNid(tree->getCtx(), &defNid);
	if(status & 1) status = _TreeSetDefaultNid(tree->getCtx(), nid);
	if(status & 1) status = _TreeAddNode(tree->getCtx(), name, &newNid, convertUsage(usage));
	if(status & 1) status = _TreeSetDefaultNid(tree->getCtx(), defNid);
	if(!(status & 1))
		throw new MdsException(status);
	return new TreeNode(newNid, tree);
}


void TreeNode::remove(char *name)
{
	int count;
	int defNid;
	resolveNid();
	int status = _TreeGetDefaultNid(tree->getCtx(), &defNid);
	if(status & 1) status = _TreeSetDefaultNid(tree->getCtx(), nid);
	TreeNode *delNode = getNode(name);
	status = _TreeDeleteNodeInitialize(tree->getCtx(), delNode->nid, &count, 1);
	delete delNode;
	if(status & 1)_TreeDeleteNodeExecute(tree->getCtx());
	if(status & 1) status = _TreeSetDefaultNid(tree->getCtx(), defNid);
	if(!(status & 1))
		throw new MdsException(status);
}

TreeNode *TreeNode::addDevice(char *name, char *type)
{

	int defNid;
	int newNid;
	resolveNid();
	int status = _TreeGetDefaultNid(tree->getCtx(), &defNid);
	if(status & 1) status = _TreeSetDefaultNid(tree->getCtx(), nid);
	if(status & 1) status = _TreeAddConglom(tree->getCtx(), name, type, &newNid);
	if(status & 1) status = _TreeSetDefaultNid(tree->getCtx(), defNid);
	if(!(status & 1))
		throw new MdsException(status);
	return new TreeNode(newNid, tree);
}

void TreeNode::rename(char *newName)
{
	resolveNid();
	int status = _TreeRenameNode(tree->getCtx(), nid, newName);
	if(!(status & 1))
		throw new MdsException(status);
}

void TreeNode::move(TreeNode *parent, char *newName)
{
	resolveNid();
	char *parentPath = parent->getFullPath();
	char *newPath = new char[strlen(newName) + strlen(parentPath) + 2];
	sprintf(newPath, "%s:%s", parentPath, newName);
	rename(newPath);
	delete [] parentPath;
	delete [] newPath;
}


void TreeNode::move(TreeNode *parent)
{
	resolveNid();
	char *name = getNodeName();
	move(parent, name);
	delete [] name;
}

void TreeNode::addTag(char *tagName)
{
	resolveNid();
	int status = _TreeAddTag(tree->getCtx(), nid, tagName);
	if(!(status & 1))
		throw new MdsException(status);
}

void TreeNode::removeTag(char *tagName)
{

	resolveNid();
	int currNid;
	char *bTag = new char[strlen(tagName) + 2];
	sprintf(bTag, "\\%s", tagName);
	int status = _TreeFindNode(tree->getCtx(), bTag, &currNid);
	delete [] bTag;
	if(currNid != nid)
		throw new MdsException("No such tag for this tree node"); //The tag does not refer to this node
	status = _TreeRemoveTag(tree->getCtx(), tagName);
	if(!(status & 1))
		throw new MdsException(status);
}

void TreeNode::setSubtree(bool isSubtree)
{
	resolveNid();
	int status;
	if(isSubtree)
		status = _TreeSetSubtree(tree->getCtx(), nid);
	else
		status = _TreeSetNoSubtree(tree->getCtx(), nid);
	if(!(status & 1))
		throw new MdsException(status);
}

StringArray *TreeNode::findTags()
{
	resolveNid();
	const int MAX_TAGS = 1024;
	char *tagNames[MAX_TAGS];
	void *ctx = 0;
	int nTags = 0;
	while(nTags < MAX_TAGS && (tagNames[nTags] = _TreeFindNodeTags(tree->getCtx(), nid, &ctx)))
		nTags++;
	StringArray *stArr = new StringArray(tagNames, nTags);
	for(int i = 0; i < nTags; i++)
	    TreeFree(tagNames[i]);
	return stArr;
}


//////////////////TreePath Methods//////////////////


TreePath::TreePath(char *val, Tree *tree, Data *units, Data *error, Data *help, Data *validation):TreeNode(0, tree, units, error, help,validation)
{
	clazz = CLASS_S;
	dtype = DTYPE_PATH;
	length = strlen(val);
	ptr = new char[length];
	memcpy(ptr, val, length);
	setAccessory(units, error, help, validation);
}
TreePath::TreePath(char *val, int len, Tree *tree, Data *units, Data *error, Data *help, Data *validation):TreeNode(0, tree, units, error, help,validation)
{
	clazz = CLASS_S;
	dtype = DTYPE_PATH;
	length = len;
	ptr = new char[length];
	memcpy(ptr, val, length);
	setAccessory(units, error, help, validation);
}

void TreePath::resolveNid()
{
	int status = _TreeFindNode(tree->getCtx(), ptr, &nid);
	if(!(status & 1))
		nid = -1;
}
	

///////////////TreeNode methods

EXPORT TreeNodeArray::TreeNodeArray(TreeNode **nodes, int numNodes)
{
	this->numNodes = numNodes;
	this->nodes = new TreeNode *[numNodes];

	for(int i = 0; i < numNodes; i++)
		this->nodes[i] = nodes[i];
}
EXPORT TreeNodeArray::TreeNodeArray(int *nids, int numNodes, Tree *tree)
{
	this->numNodes = numNodes;
	this->nodes = new TreeNode *[numNodes];

	for(int i = 0; i < numNodes; i++)
		this->nodes[i] = new TreeNode(nids[i], tree);
}
EXPORT TreeNodeArray::~TreeNodeArray()
{
	if(numNodes > 0)
	{
		for(int i = 0; i < numNodes; i++)
			deleteData(nodes[i]);
		delete [] nodes;
	}
			
}


EXPORT void *TreeNodeArray::operator new(size_t sz)
{
	return ::operator new(sz);
}
EXPORT void TreeNodeArray::operator delete(void *p)
{
	::operator delete(p);
}

EXPORT StringArray *TreeNodeArray::getPath()
{
	int i;
	char **paths = new char *[numNodes];
	for(i = 0; i < numNodes; i++)
	{
		paths[i] = nodes[i]->getPath();
	}

	StringArray *retData = new StringArray(paths, numNodes);
	for(i = 0; i < numNodes; i++)
	{
		deleteString(paths[i]);
	}
	delete [] paths;
	return  retData;
}
EXPORT StringArray *TreeNodeArray::getFullPath()
{
	int i;
	char **paths = new char *[numNodes];
	for(i = 0; i < numNodes; i++)
	{
		paths[i] = nodes[i]->getFullPath();
	}

	StringArray *retData = new StringArray(paths, numNodes);
	for(i = 0; i < numNodes; i++)
	{
		deleteString(paths[i]);
	}
	delete [] paths;
	return  retData;
}

EXPORT Int32Array *TreeNodeArray::getNid()
{
	int i;
	int *nids = new int[numNodes];
	for(i = 0; i < numNodes; i++)
	{
		nids[i] = nodes[i]->getNid();
	}

	Int32Array *retData = new Int32Array(nids, numNodes);
	delete [] nids;
	return  retData;
}

EXPORT Int8Array *TreeNodeArray::isOn()
{
	int i;
	char *info = new char[numNodes];
	for(i = 0; i < numNodes; i++)
	{
		info[i] = nodes[i]->isOn();
	}

	Int8Array *retData = new Int8Array(info, numNodes);
	delete [] info;
	return  retData;
}

EXPORT void TreeNodeArray::setOn(Int8Array *info)
{
	int i, numInfo;
	char *infoArray = info->getByteArray(&numInfo);
	if(numInfo > numNodes)
		numInfo = numNodes;

	for(i = 0; i < numInfo; i++)
	{
		nodes[i]->setOn((infoArray[i])?true:false);
	}
	delete [] infoArray;
}

EXPORT Int8Array *TreeNodeArray::isSetup()
{
	int i;
	char *info = new char[numNodes];
	for(i = 0; i < numNodes; i++)
	{
		info[i] = nodes[i]->isSetup();
	}

	Int8Array *retData = new Int8Array(info, numNodes);
	delete [] info;
	return  retData;
}


EXPORT Int8Array *TreeNodeArray::isWriteOnce()
{
	int i;
	char *info = new char[numNodes];
	for(i = 0; i < numNodes; i++)
	{
		info[i] = nodes[i]->isWriteOnce();
	}

	Int8Array *retData = new Int8Array(info, numNodes);
	delete [] info;
	return  retData;
}

EXPORT void TreeNodeArray::setWriteOnce(Int8Array *info)
{
	int i, numInfo;
	char *infoArray = info->getByteArray(&numInfo);
	if(numInfo > numNodes)
		numInfo = numNodes;

	for(i = 0; i < numInfo; i++)
	{
		nodes[i]->setWriteOnce((infoArray[i])?true:false);
	}
	delete [] infoArray;
}

EXPORT Int8Array *TreeNodeArray::isCompressOnPut()
{
	int i;
	char *info = new char[numNodes];
	for(i = 0; i < numNodes; i++)
	{
		info[i] = nodes[i]->isCompressOnPut();
	}

	Int8Array *retData = new Int8Array(info, numNodes);
	delete [] info;
	return  retData;
}

EXPORT void TreeNodeArray::setCompressOnPut(Int8Array *info)
{
	int i, numInfo;
	char *infoArray = info->getByteArray(&numInfo);
	if(numInfo > numNodes)
		numInfo = numNodes;

	for(i = 0; i < numInfo; i++)
	{
		nodes[i]->setCompressOnPut((infoArray[i])?true:false);
	}
	delete [] infoArray;
}

EXPORT Int8Array *TreeNodeArray::isNoWriteModel()
{
	int i;
	char *info = new char[numNodes];
	for(i = 0; i < numNodes; i++)
	{
		info[i] = nodes[i]->isNoWriteModel();
	}

	Int8Array *retData = new Int8Array(info, numNodes);
	delete [] info;
	return  retData;
}

EXPORT void TreeNodeArray::setNoWriteModel(Int8Array *info)
{
	int i, numInfo;
	char *infoArray = info->getByteArray(&numInfo);
	if(numInfo > numNodes)
		numInfo = numNodes;

	for(i = 0; i < numInfo; i++)
	{
		nodes[i]->setNoWriteModel((infoArray[i])?true:false);
	}
	delete [] infoArray;
}

EXPORT Int8Array *TreeNodeArray::isNoWriteShot()
{
	int i;
	char *info = new char[numNodes];
	for(i = 0; i < numNodes; i++)
	{
		info[i] = nodes[i]->isNoWriteShot();
	}

	Int8Array *retData = new Int8Array(info, numNodes);
	delete [] info;
	return  retData;
}

EXPORT void TreeNodeArray::setNoWriteShot(Int8Array *info)
{
	int i, numInfo;
	char *infoArray = info->getByteArray(&numInfo);
	if(numInfo > numNodes)
		numInfo = numNodes;

	for(i = 0; i < numInfo; i++)
	{
		nodes[i]->setNoWriteShot((infoArray[i])?true:false);
	}
	delete [] infoArray;
}

EXPORT Int32Array *TreeNodeArray::getLength()
{
	int i;
	int *sizes = new int[numNodes];
	for(i = 0; i < numNodes; i++)
	{
		sizes[i] = nodes[i]->getLength();
	}

	Int32Array *retData = new Int32Array(sizes, numNodes);
	delete [] sizes;
	return  retData;
}
EXPORT Int32Array *TreeNodeArray::getCompressedLength()
{
	int i;
	int *sizes = new int[numNodes];
	for(i = 0; i < numNodes; i++)
	{
		sizes[i] = nodes[i]->getCompressedLength();
	}

	Int32Array *retData = new Int32Array(sizes, numNodes);
	delete [] sizes;
	return  retData;
}

EXPORT StringArray *TreeNodeArray::getUsage()
{
	int i;
	const char **usages = new const char *[numNodes];
	for(i = 0; i < numNodes; i++)
	{
		usages[i] = nodes[i]->getUsage();
	}

	StringArray *retData = new StringArray((char **)usages, numNodes);
	delete [] usages;
	return  retData;
}



////////////////End TreeNodeArray methods/////////////////////







EXPORT void MDSplus::setActiveTree(Tree *tree)
{
	TreeSwitchDbid(tree->getCtx());
}

EXPORT Tree *MDSplus::getActiveTree()
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
		throw new MdsException(status);
	return new Tree(name, shot);
}

ostream &operator<<(ostream &stream, TreeNode *treeNode)
{
	return stream << treeNode->getPath();
}

