#include <mdsobjects.h>
#include <usagedef.h>
#include <treeshr.h>

#include <algorithm>
#include <string>
#include <cctype>

using namespace MDSplus;
using namespace std;

extern "C" {
	// From treeshrp.h
	int TreeFindTag(char *tagnam, char *treename, int *tagidx);

	// From libroutines.h
	int LibConvertDateString(char *asc_time, int64_t *qtime);

	// From mdsshr.h
	const char *MdsClassString(int id);
	const char *MdsDtypeString(int id);

	// From mdsdata.c
	void freeChar(void *);
	void freeDsc(void *dscPtr);
	void * compileFromExprWithArgs(char *expr, int nArgs, void *args, void *tree);
	void * evaluateData(void *dscPtr, int isEvaluate);
	void * deserializeData(char *serialized, int size);
	char * decompileDsc(void *dscPtr);
	void * convertFromDsc(void *dscPtr);

	void * convertToScalarDsc(int clazz, int dtype, int length, char *ptr);
	void * convertToArrayDsc(int clazz, int dtype, int length, int l_length, int nDims, int *dims, void *ptr);
	void * convertToCompoundDsc(int clazz, int dtype, int length, void *ptr, int ndescs, void **descs);
	void * convertToApdDsc(int ndescs, void **ptr);
	void * convertToByte(void *dsc); 
	void * convertToShort(void *dsc); 
	void * convertToInt(void *dsc); 
	void * convertToLong(void *dsc); 
	void * convertToFloat(void *dsc); 
	void * convertToDouble(void *dsc); 
	void * convertToShape(void *dcs);

	// From mdstree.c
	void convertTime(void *dbid, int *time, char *retTime);
	int getTreeData(void *dbid, int nid, void **data, void *tree);
	int putTreeData(void *dbid, int nid, void *data);
	int deleteTreeData(void *dbid, int nid);
	int doTreeMethod(void *dbid, int nid, char *method);
	int beginTreeSegment(void *dbid, int nid, void *dataDsc, void *startDsc, void *endDsc,
									void *timeDsc);
	int makeTreeSegment(void *dbid, int nid, void *dataDsc, void *startDsc, void *endDsc,
									void *timeDsc, int rowsFilled);
	int putTreeSegment(void *dbid, int nid, void *dataDsc, int ofs);
	int updateTreeSegment(void *dbid, int nid, void *startDsc, void *endDsc,
									void *timeDsc);
	int getTreeNumSegments(void *dbid, int nid, int *numSegments);
	int getTreeSegmentLimits(void *dbid, int nid, int idx, void **startDsc, void **endDsc);
	int getTreeSegment(void *dbid, int nid, int segIdx, void **dataDsc, void **timeDsc);
	int setTreeTimeContext(void *startDsc, void *endDsc, void *deltaDsc);
	int beginTreeTimestampedSegment(void *dbid, int nid, void *dataDsc);
	int putTreeTimestampedSegment(void *dbid, int nid, void *dataDsc, int64_t *times);
	int makeTreeTimestampedSegment(void *dbid, int nid, void *dataDsc, int64_t *times, int rowsFilled);
	int putTreeRow(void *dbid, int nid, void *dataDsc, int64_t *time, int size);
	int getTreeSegmentInfo(void *dbid, int nid, int segIdx, char *dtype, char *dimct, int *dims, int *nextRow);
	// From TreeFindTagWild.c
	char * _TreeFindTagWild(void *dbid, char *wild, int *nidout, void **ctx_inout);
}

#ifdef HAVE_WINDOWS_H
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#define MAX_ARGS 512

static int convertUsage(std::string const & usage)
{
	if (usage == "ACTION")
		return TreeUSAGE_ACTION;
	else if (usage == "ANY")
		return TreeUSAGE_ANY;
	else if (usage == "AXIS")
		return TreeUSAGE_AXIS;
	else if (usage == "COMPOUND_DATA")
		return TreeUSAGE_COMPOUND_DATA;
	else if (usage == "DEVICE")
		return TreeUSAGE_DEVICE;
	else if (usage == "DISPATCH")
		return TreeUSAGE_DISPATCH;
	else if (usage == "STRUCTURE")
		return TreeUSAGE_STRUCTURE;
	else if (usage == "NUMERIC")
		return TreeUSAGE_NUMERIC;
	else if (usage == "SIGNAL")
		return TreeUSAGE_SIGNAL;
	else if (usage == "SUBTREE")
		return TreeUSAGE_SUBTREE;
	else if (usage == "TASK")
		return TreeUSAGE_TASK;
	else if (usage == "TEXT")
		return TreeUSAGE_TEXT;
	else if (usage == "WINDOW")
		return TreeUSAGE_WINDOW;
	else
		return TreeUSAGE_ANY;
}

#ifdef HAVE_WINDOWS_H
#include <Windows.h>
static HANDLE semH = 0;
void Tree::lock() 
{
	if(!semH)
		semH = CreateSemaphore(NULL, 1, 16, NULL);
	if(!semH)
		throw MdsException("Cannot create lock semaphore");
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
			throw MdsException("Cannot create lock semaphore");
	}
	sem_wait(&semStruct);
}

void Tree::unlock()
{
	sem_post(&semStruct);
}
#endif


Tree::Tree(char const *name, int shot) : shot(shot), ctx(nullptr)
{
	int status = _TreeOpen(&ctx, name, shot, 0);
	if(!(status & 1))
	{
		throw MdsException(status);
	}
	this->name = new char[strlen(name) + 1];
	strcpy(this->name, name);
	//setActiveTree(this);
}

Tree::Tree(void *dbid, char const *name, int shot) : shot(shot), ctx(dbid)
{
	this->name = new char[strlen(name) + 1];
	strcpy(this->name, name);
}

Tree::Tree(char const *name, int shot, char const *mode) : shot(shot), ctx(nullptr)
{
	std::string upMode(mode);
	std::transform(upMode.begin(), upMode.end(), upMode.begin(), static_cast<int(*)(int)>(&std::toupper));

	int status = 0;
	if(upMode == "NORMAL")
		status = _TreeOpen(&ctx, name, shot, 0);
	else if(upMode == "READONLY")
		status = _TreeOpen(&ctx, name, shot, 0);
	else if(upMode == "NEW")
		status = _TreeOpenNew(&ctx, name, shot);
	else if(upMode == "EDIT")
		status = _TreeOpenEdit(&ctx, name, shot);
	else
		throw MdsException("Invalid Open mode");

	if(!(status & 1))
		throw MdsException(status);

	this->name = new char[strlen(name) + 1];
	strcpy(this->name, name);
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
		throw MdsException(status);
	}
}

Tree *Tree::create(char const * name, int shot)
{
	void *dbid;
	int status = _TreeOpenNew(&dbid, name, shot);
	if(!(status & 1))
	{
		throw MdsException(status);
	}
	return new Tree(dbid, name, shot);
}

void Tree::write()
{
	int status = _TreeWriteTree(&ctx, name, shot);
	if(!(status & 1))
	{
		throw MdsException(status);
	}
}

void Tree::quit()
{
	int status = _TreeQuitTree(&ctx, name, shot);
	if(!(status & 1))
	{
		throw MdsException(status);
	}
}

TreeNode *Tree::addNode(char const * name, char *usage)
{
	int newNid;
	int status = _TreeAddNode(ctx, name, &newNid, convertUsage(usage));
	if(!(status & 1))
		throw MdsException(status);
	return new TreeNode(newNid, this);
}

TreeNode *Tree::addDevice(char const * name, char *type)
{
	int newNid;
	int status = _TreeAddConglom(ctx, name, type, &newNid);
	if(!(status & 1))
		throw MdsException(status);
	return new TreeNode(newNid, this);
}

void Tree::remove(char const * name)
{
	int count;
	TreeNode *delNode = getNode(name);
	int status = _TreeDeleteNodeInitialize(ctx, delNode->getNid(), &count, 1);
	delete delNode;
	if(status & 1)_TreeDeleteNodeExecute(ctx);
	if(!(status & 1))
		throw MdsException(status);
}

TreeNode *Tree::getNode(char const * path)
{
	int nid;

	int status = _TreeFindNode(ctx, path, &nid);
	if(!(status & 1))
	{
		throw MdsException(status);
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
		throw MdsException(status);
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
		throw MdsException(status);
	}
	return new TreeNode(nid, this);
}

TreeNodeArray *Tree::getNodeWild(char const * path, int usageMask)
{
	int currNid, status; 
	int numNids = 0;
	void *wildCtx = 0;
	int chunkSize = 10000;
	int *currNids = new int[chunkSize];


	while ((status = _TreeFindNodeWild(ctx, path,&currNids[numNids],&wildCtx, usageMask)) & 1)
	{
		numNids++;
		if(numNids == chunkSize)
		{
			int* newNids = new int[2 * chunkSize];
			memcpy(newNids, currNids, chunkSize * sizeof(int));
			delete []currNids;
			currNids = newNids;
			chunkSize = 2 * chunkSize;
		}
	}
	_TreeFindNodeEnd(ctx, &wildCtx);

	//printf("%s\n", MdsGetMsg(status));

	TreeNode **retNodes = new TreeNode *[numNids];
	for(int i = 0; i < numNids; i++)
	{
		retNodes[i] = new TreeNode(currNids[i], this);
	}
	TreeNodeArray *nodeArray = new TreeNodeArray(retNodes, numNids);
	delete [] retNodes;
	delete[]currNids;
	return nodeArray;
}

TreeNodeArray *Tree::getNodeWild(char const *path)
{
	return getNodeWild(path, -1);
}


void Tree::setDefault(TreeNode *treeNode)
{
	int status = _TreeSetDefaultNid(ctx, treeNode->getNid());
	if(!(status & 1)) 
		throw MdsException(status);
}

TreeNode *Tree::getDefault()
{
	int nid;

	int status = _TreeGetDefaultNid(ctx, &nid);
	if(!(status & 1)) 
		throw MdsException(status);
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
		throw MdsException(status);
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
		throw MdsException(status);
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
		throw MdsException(status);
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
		throw MdsException(status);
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
		throw MdsException(status);
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
		throw MdsException(status);
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
		throw MdsException(status);
}


void Tree::setViewDate(char *date)
{
	int64_t  qtime;

	int status = LibConvertDateString(date, &qtime);
	if(!(status & 1))
		throw MdsException("Invalid date format");
	status = TreeSetViewDate(&qtime);
	if(!(status & 1)) 
		throw MdsException(status);
}


void Tree::setTimeContext(Data *start, Data *end, Data *delta)
{
	int status = setTreeTimeContext((start)?start->convertToDsc():0, (end)?end->convertToDsc():0, 
		(delta)?delta->convertToDsc():0);
	if(!(status & 1))
		throw MdsException(status);
}



void Tree::setCurrent(char const * treeName, int shot)
{
	int status = TreeSetCurrentShotId(treeName, shot);
	if(!(status & 1))
		throw MdsException(status);
}

int Tree::getCurrent(char const * treeName)
{
	return TreeGetCurrentShotId(treeName);
}

void Tree::createPulse(int shot)
{
	int  retNids;
	int status = _TreeCreatePulseFile(getCtx(), shot, 0, &retNids);
	if(!(status & 1))
		throw MdsException(status);
}

void Tree::deletePulse(int shot)
{
	int status = _TreeDeletePulseFile(getCtx(), shot, 1);
	if(!(status & 1))
		throw MdsException(status);
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
void Tree::removeTag(char const * tagName)
{
	int status = _TreeRemoveTag(getCtx(), tagName);
	if(!(status & 1))
		throw MdsException(status);
}

int64_t Tree::getDatafileSize()
{
	int64_t size = _TreeGetDatafileSize(getCtx());
	if(size == -1)
		throw MdsException("Cannot retrieve datafile size");
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

Data *TreeNode::data()
{
	Data *d = getData();
	Data *outD = d->data();
	MDSplus::deleteData(d);
	return outD;
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
		throw MdsException(status);
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
		throw MdsException(status);
}

TreeNode::TreeNode(int nid, Tree *tree, Data *units, Data *error, Data *help, Data *validation)
{
	this->tree = 0;
	if(!tree && nid != 0)
		this->tree = getActiveTree();
	if(!tree && nid != 0) //exclude the case in which this constructor has been called in a TreePath instantiation 
		throw MdsException("A Tree instance must be defined when ceating TreeNode instances");
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

std::string TreeNode::getPathStr()
{
	resolveNid();
	char *currPath = _TreeGetPath(tree->getCtx(), nid);
	std::string path(currPath);
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
		throw MdsException(status);
	path[pathLen] = 0;
	char *retPath = new char[strlen(path)+1];
	strcpy(retPath, path);
	return retPath;
}

std::string TreeNode::getMinPathStr()
{
	char *currPath = getMinPath();
	std::string retPath(currPath);
	delete [] currPath;
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
		throw MdsException(status);
	path[pathLen] = 0;
	char *retPath = new char[strlen(path)+1];
	strcpy(retPath, path);
	return retPath;
}


std::string TreeNode::getFullPathStr()
{
	char *currPath = getFullPath();
	std::string retPath(currPath);
	delete [] currPath;
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
		throw MdsException(status);
	path[pathLen] = 0;
	char *retPath = new char[strlen(path)+1];
	strcpy(retPath, path);
	return retPath;
}

std::string TreeNode::getOriginalPartNameStr()
{
	char *currPath = getOriginalPartName();
	std::string retPath(currPath);
	delete [] currPath;
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
		throw MdsException(status);
	path[pathLen] = 0;
	char *retPath = new char[strlen(path)+1];
	strcpy(retPath, path);
	//Trim
	for(int i = strlen(retPath) - 1; i > 0 && retPath[i] == ' '; i--)
			retPath[i] = 0;
	return retPath;
}

std::string TreeNode::getNodeNameStr()
{
	char *currPath = getNodeName();
	std::string retPath(currPath);
	delete [] currPath;
	return retPath;
}



Data *TreeNode::getData()
{
	Data *data = 0;
	resolveNid();
	int status = getTreeData(tree->getCtx(), nid, (void **)&data, tree);
	if(!(status & 1))
	{
		throw MdsException(status);
	}
	return data;
}

void TreeNode::putData(Data *data)
{
	resolveNid();
	int status = putTreeData(tree->getCtx(), nid, (void *)data);
	if(!(status & 1))
	{
		throw MdsException(status);
	}
}
void TreeNode::deleteData()
{
	resolveNid();
	int status = deleteTreeData(tree->getCtx(), nid);
	if(!(status & 1))
	{
		throw MdsException(status);
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
		throw MdsException(status);
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
		throw MdsException(status);
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
		throw MdsException(status);
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
		throw MdsException(status);
	return id;
}

int64_t TreeNode::getTimeInserted()
{
	int64_t timeInserted; 
	int timeLen;
	struct nci_itm nciList[] = 
		{{8, NciTIME_INSERTED, (char *)&timeInserted, &timeLen},
		{NciEND_OF_LIST, 0, 0, 0}};
	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw MdsException(status);
	return timeInserted;
}

void TreeNode::doMethod(char *method)
{
	resolveNid();
	int status = doTreeMethod(tree->getCtx(), nid, method);
	if(!(status & 1))
		throw MdsException(status);
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
		throw MdsException(status);
	
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
		throw MdsException(status);
	
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
		throw MdsException(status);

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
		throw MdsException(status);

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
		throw MdsException(status);

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
		throw MdsException(status);

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
		throw MdsException(status);
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
		throw MdsException(status);
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
		throw MdsException(status);

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
		throw MdsException(status);
	}

	int *childrenNids = new int[numMembers+nChildren];

	int retLen = sizeof(int) * (numMembers+nChildren);
	struct nci_itm nciList1[] = 
		{{sizeof(int) * numMembers, NciMEMBER_NIDS, &childrenNids[0], &retLen},
		{sizeof(int) * nChildren, NciCHILDREN_NIDS, &childrenNids[numMembers], &retLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
	{
		delete [] childrenNids;
		throw MdsException(status);
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
		throw MdsException(status);
	}

	int *childrenNids = new int[nChildren];

	int retLen = sizeof(int) * (nChildren);
	struct nci_itm nciList1[] = 
		{{retLen, NciCHILDREN_NIDS, &childrenNids[0], &retLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	status = _TreeGetNci(tree->getCtx(), nid, nciList1);
	if(!(status & 1))
	{
		delete [] childrenNids;
		throw MdsException(status);
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
		throw MdsException(status);
	}

	int *memberNids = new int[nMembers];

	int retLen = sizeof(int) * (nMembers);
	struct nci_itm nciList1[] = 
		{{retLen, NciMEMBER_NIDS, &memberNids[0], &retLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
	{
		delete [] memberNids;
		throw MdsException(status);
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
		throw MdsException(status);

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
		throw MdsException(status);

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
		throw MdsException(status);

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
	int elt = -1;
	struct nci_itm nciList[] = 
		{{1, NciCONGLOMERATE_ELT, (char *)&elt, &eltLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	resolveNid();
	int status = _TreeGetNci(tree->getCtx(), nid, nciList);
	if(!(status & 1))
		throw MdsException(status);
	if(elt == -1)
		throw MdsException("Not a conglomerate element");
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
		throw MdsException(status);
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
		throw MdsException(status);

	int *nids = new int[nNids];
	struct nci_itm nciList1[] = 
		{{4*nNids, NciCONGLOMERATE_ELT, (char *)nids, &retLen},
		{NciEND_OF_LIST, 0, 0, 0}};

	status = _TreeGetNci(tree->getCtx(), nid, nciList1);
	if(!(status & 1))
		throw MdsException(status);

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
		throw MdsException(status);

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
		throw MdsException(status);
}

void TreeNode::beginSegment(Data *start, Data *end, Data *time, Array *initialData)
{
	resolveNid();
	//if(tree) tree->lock();
	int status = beginTreeSegment(tree->getCtx(), getNid(), initialData->convertToDsc(), start->convertToDsc(), 
		end->convertToDsc(), time->convertToDsc());
	//if(tree) tree->unlock();
	if(!(status & 1))
		throw MdsException(status);
}

void TreeNode::putSegment(Array *data, int ofs)
{
	resolveNid();
	//if(tree) tree->lock();
	int status = putTreeSegment(tree->getCtx(), getNid(), data->convertToDsc(), ofs);
	//if(tree) tree->unlock();
	if(!(status & 1))
		throw MdsException(status);
}

void TreeNode::updateSegment(Data *start, Data *end, Data *time)
{
	resolveNid();
	//if(tree) tree->lock();
	int status = updateTreeSegment(tree->getCtx(), getNid(), start->convertToDsc(), 
		end->convertToDsc(), time->convertToDsc());
	//if(tree) tree->unlock();
	if(!(status & 1))
		throw MdsException(status);
}

int TreeNode::getNumSegments()
{
	int numSegments;

	resolveNid();
	//if(tree) tree->lock();
	int status = getTreeNumSegments(tree->getCtx(), getNid(), &numSegments);
	//if(tree) tree->unlock();
	if(!(status & 1))
		throw MdsException(status);
	return  numSegments;
}

void TreeNode::getSegmentInfo(int segIdx, char *dtype, char *dimct, int *dims, int *nextRow)
{
	resolveNid();
	int status = getTreeSegmentInfo(tree->getCtx(), getNid(), segIdx, dtype, dimct, dims, nextRow);
	if(!(status & 1))
		throw MdsException(status);
}


void TreeNode::getSegmentLimits(int segmentIdx, Data **start, Data **end)
{
	void *startDsc, *endDsc;
	resolveNid();
	//if(tree) tree->lock();
	int status = getTreeSegmentLimits(tree->getCtx(), getNid(), segmentIdx, &startDsc, &endDsc);
	//if(tree) tree->unlock();
	if(!(status & 1))
		throw MdsException(status);
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
		throw MdsException(status);
	}
	Array *retData = (Array *)convertFromDsc(dataDsc);
	freeDsc(dataDsc);
	freeDsc(timeDsc);
	return retData;
}

Data *TreeNode::getSegmentDim(int segIdx)
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
		throw MdsException(status);
	}
	Data *retDim = (Data *)convertFromDsc(timeDsc);
	freeDsc(dataDsc);
	freeDsc(timeDsc);
	return retDim;
}

void TreeNode::beginTimestampedSegment(Array *initData)
{
	resolveNid();
	//if(tree) tree->lock();
	int status = beginTreeTimestampedSegment(tree->getCtx(), getNid(), initData->convertToDsc());
	//if(tree) tree->unlock();
	if(!(status & 1))
		throw MdsException(status);
}

void TreeNode::makeTimestampedSegment(Array *data, int64_t *times)
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
		throw MdsException(status);
}

void TreeNode::putTimestampedSegment(Array *data, int64_t *times)
{
	int nTimesArray;
	resolveNid();
	//if(tree) tree->lock();
	int status = putTreeTimestampedSegment(tree->getCtx(), getNid(), data->convertToDsc(), times);
	//if(tree) tree->unlock();
	if(!(status & 1))
		throw MdsException(status);
}

void TreeNode::putRow(Data *data, int64_t *time, int size)
{
	resolveNid();
	//if(tree) tree->lock();
	int status = putTreeRow(tree->getCtx(), getNid(), data->convertToDsc(), time, size);
	//if(tree) tree->unlock();
	if(!(status & 1))
		throw MdsException(status);
}

void TreeNode::acceptSegment(Array *data, Data *start, Data *end, Data *times)
{
	resolveNid();
	int status = beginTreeSegment(tree->getCtx(), getNid(), data->convertToDsc(), start->convertToDsc(), 
		end->convertToDsc(), times->convertToDsc());
	if(!(status & 1))
		throw MdsException(status);
}

void TreeNode::acceptRow(Data *data, int64_t time, bool isLast)
{
	resolveNid();
	int status = putTreeRow(tree->getCtx(), getNid(), data->convertToDsc(), &time, 1024);
	if(!(status & 1))
		throw MdsException(status);
}

TreeNode *TreeNode::getNode(char const * relPath)
{
	int defNid;
	int newNid;
	resolveNid();
	int status = _TreeGetDefaultNid(tree->getCtx(), &defNid);
	if(status & 1) status = _TreeSetDefaultNid(tree->getCtx(), nid);
	if(status & 1) status = _TreeFindNode(tree->getCtx(), relPath, &newNid);
	if(status & 1) status = _TreeSetDefaultNid(tree->getCtx(), defNid);
	if(!(status & 1))
		throw MdsException(status);
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
		throw MdsException(status);
	delete [] relPath;
	return new TreeNode(newNid, tree);
}

TreeNode *TreeNode::addNode(char const * name, char const * usage)
{
	int defNid;
	int newNid;
	resolveNid();
	int status = _TreeGetDefaultNid(tree->getCtx(), &defNid);
	if(status & 1) status = _TreeSetDefaultNid(tree->getCtx(), nid);
	if(status & 1) status = _TreeAddNode(tree->getCtx(), name, &newNid, convertUsage(usage));
	if(status & 1) status = _TreeSetDefaultNid(tree->getCtx(), defNid);
	if(!(status & 1))
		throw MdsException(status);
	return new TreeNode(newNid, tree);
}

void TreeNode::remove(char const * name)
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
		throw MdsException(status);
}

TreeNode *TreeNode::addDevice(char const * name, char const * type)
{
	int defNid;
	int newNid;
	resolveNid();
	int status = _TreeGetDefaultNid(tree->getCtx(), &defNid);
	if(status & 1) status = _TreeSetDefaultNid(tree->getCtx(), nid);
	if(status & 1) status = _TreeAddConglom(tree->getCtx(), name, type, &newNid);
	if(status & 1) status = _TreeSetDefaultNid(tree->getCtx(), defNid);
	if(!(status & 1))
		throw MdsException(status);
	return new TreeNode(newNid, tree);
}

void TreeNode::rename(std::string const & newName)
{
	resolveNid();
	int status = _TreeRenameNode(tree->getCtx(), nid, newName.c_str());
	if(!(status & 1))
		throw MdsException(status);
}

void TreeNode::move(TreeNode *parent, std::string const & newName)
{
	resolveNid();
	char *parentPath = parent->getFullPath();
	rename(std::string(parentPath) + ":" + newName);
	delete [] parentPath;
}

void TreeNode::move(TreeNode *parent)
{
	resolveNid();
	char *name = getNodeName();
	move(parent, name);
	delete [] name;
}

void TreeNode::addTag(std::string const & tagName)
{
	resolveNid();
	int status = _TreeAddTag(tree->getCtx(), nid, tagName.c_str());
	if(!(status & 1))
		throw MdsException(status);
}

void TreeNode::removeTag(std::string const & tagName)
{
	resolveNid();
	int currNid;
	std::string bTag("\\" + tagName);
	int status = _TreeFindNode(tree->getCtx(), bTag.c_str(), &currNid);
	if(currNid != nid)
		throw MdsException("No such tag for this tree node"); //The tag does not refer to this node
	status = _TreeRemoveTag(tree->getCtx(), tagName.c_str());
	if(!(status & 1))
		throw MdsException(status);
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
		throw MdsException(status);
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
TreePath::~TreePath()
{
	if(length > 0)
		delete[]ptr;
}
void TreePath::resolveNid()
{
	char *path = new char[length+1];
	memcpy(path, ptr, length);
	path[length] = 0;
	if(!tree)
		tree = getActiveTree();
	int status = _TreeFindNode(tree->getCtx(), path, &nid);
	if(!(status & 1))
		nid = -1;
	delete [] path;
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
#ifdef HAVE_WINDOWS_H
	char **paths = new char *[numNodes];
#else
	char * paths[numNodes];
#endif
	for(int i = 0; i < numNodes; ++i)
		paths[i] = nodes[i]->getPath();

	StringArray *retData = new StringArray(paths, numNodes);
	for(int i = 0; i < numNodes; ++i)
		deleteString(paths[i]);
#ifdef HAVE_WINDOWS_H
	delete [] paths;
#endif

	return retData;
}

EXPORT StringArray *TreeNodeArray::getFullPath()
{
#ifdef HAVE_WINDOWS_H
	char **paths = new char *[numNodes];
#else
	char * paths[numNodes];
#endif

	for(int i = 0; i < numNodes; ++i)
		paths[i] = nodes[i]->getFullPath();

	StringArray *retData = new StringArray(paths, numNodes);
	for(int i = 0; i < numNodes; ++i)
		deleteString(paths[i]);

#ifdef HAVE_WINDOWS_H
	delete [] paths;
#endif

	return  retData;
}

EXPORT Int32Array *TreeNodeArray::getNid()
{
#ifdef HAVE_WINDOWS_H
	int *nids = new int[numNodes];
#else
	int nids[numNodes];
#endif
	for(int i = 0; i < numNodes; ++i)
		nids[i] = nodes[i]->getNid();

#ifdef HAVE_WINDOWS_H
	delete [] nids;
#endif
	return new Int32Array(nids, numNodes);;
}


EXPORT Int8Array *TreeNodeArray::isOn()
{
#ifdef HAVE_WINDOWS_H
	char *info = new char[numNodes];
#else
	char info[numNodes];
#endif
	for(int i = 0; i < numNodes; ++i)
		info[i] = nodes[i]->isOn();

	Int8Array *retData = new Int8Array(info, numNodes);
#ifdef HAVE_WINDOWS_H
	delete [] info;
#endif
	return  retData;
}

EXPORT void TreeNodeArray::setOn(Int8Array *info)
{
	int numInfo;
	char *infoArray = info->getByteArray(&numInfo);
	if(numInfo > numNodes)
		numInfo = numNodes;

	for(int i = 0; i < numInfo; ++i)
		nodes[i]->setOn((infoArray[i]) ? true : false);

	delete [] infoArray;
}

EXPORT Int8Array *TreeNodeArray::isSetup()
{
#ifdef HAVE_WINDOWS_H
	char *info = new char[numNodes];
#else
	char info[numNodes];
#endif
	for(int i = 0; i < numNodes; ++i)
		info[i] = nodes[i]->isSetup();

#ifdef HAVE_WINDOWS_H
	delete [] info;
#endif
	return new Int8Array(info, numNodes);
}


EXPORT Int8Array *TreeNodeArray::isWriteOnce()
{
#ifdef HAVE_WINDOWS_H
	char *info = new char[numNodes];
#else
	char info[numNodes];
#endif
	for(int i = 0; i < numNodes; ++i)
		info[i] = nodes[i]->isWriteOnce();

#ifdef HAVE_WINDOWS_H
	delete [] info;
#endif
	return new Int8Array(info, numNodes);
}

EXPORT void TreeNodeArray::setWriteOnce(Int8Array *info)
{
	int numInfo;
	char *infoArray = info->getByteArray(&numInfo);
	if(numInfo > numNodes)
		numInfo = numNodes;

	for(int i = 0; i < numInfo; ++i)
		nodes[i]->setWriteOnce((infoArray[i])?true:false);

	delete [] infoArray;
}

EXPORT Int8Array *TreeNodeArray::isCompressOnPut()
{
#ifdef HAVE_WINDOWS_H
	char *info = new char[numNodes];
#else
	char info[numNodes];
#endif
	for(int i = 0; i < numNodes; ++i)
		info[i] = nodes[i]->isCompressOnPut();

#ifdef HAVE_WINDOWS_H
	delete [] info;
#endif
	return new Int8Array(info, numNodes);
}

EXPORT void TreeNodeArray::setCompressOnPut(Int8Array *info)
{
	int numInfo;
	char *infoArray = info->getByteArray(&numInfo);
	if(numInfo > numNodes)
		numInfo = numNodes;

	for(int i = 0; i < numInfo; ++i)
		nodes[i]->setCompressOnPut((infoArray[i])?true:false);

	delete [] infoArray;
}

EXPORT Int8Array *TreeNodeArray::isNoWriteModel()
{
#ifdef HAVE_WINDOWS_H
	char *info = new char[numNodes];
#else
	char info[numNodes];
#endif
	for(int i = 0; i < numNodes; ++i)
		info[i] = nodes[i]->isNoWriteModel();

#ifdef HAVE_WINDOWS_H
	delete [] info;
#endif
	return new Int8Array(info, numNodes);
}

EXPORT void TreeNodeArray::setNoWriteModel(Int8Array *info)
{
	int numInfo;
	char *infoArray = info->getByteArray(&numInfo);
	if(numInfo > numNodes)
		numInfo = numNodes;

	for(int i = 0; i < numInfo; ++i)
		nodes[i]->setNoWriteModel((infoArray[i])?true:false);

	delete [] infoArray;
}

EXPORT Int8Array *TreeNodeArray::isNoWriteShot()
{
#ifdef HAVE_WINDOWS_H
	char *info = new char[numNodes];
#else
	char info[numNodes];
#endif
	for(int i = 0; i < numNodes; ++i)
		info[i] = nodes[i]->isNoWriteShot();

#ifdef HAVE_WINDOWS_H
	delete [] info;
#endif
	return new Int8Array(info, numNodes);
}

EXPORT void TreeNodeArray::setNoWriteShot(Int8Array *info)
{
	int numInfo;
	char *infoArray = info->getByteArray(&numInfo);
	if(numInfo > numNodes)
		numInfo = numNodes;

	for(int i = 0; i < numInfo; ++i)
		nodes[i]->setNoWriteShot((infoArray[i])?true:false);

	delete [] infoArray;
}

EXPORT Int32Array *TreeNodeArray::getLength()
{
#ifdef HAVE_WINDOWS_H
	int *sizes = new int[numNodes];
#else
	int sizes[numNodes];
#endif
	for(int i = 0; i < numNodes; ++i)
		sizes[i] = nodes[i]->getLength();

#ifdef HAVE_WINDOWS_H
	delete [] sizes;
#endif
	return new Int32Array(sizes, numNodes);
}

EXPORT Int32Array *TreeNodeArray::getCompressedLength()
{
#ifdef HAVE_WINDOWS_H
	int *sizes = new int[numNodes];
#else
	int sizes[numNodes];
#endif
	for(int i = 0; i < numNodes; ++i)
		sizes[i] = nodes[i]->getCompressedLength();

#ifdef HAVE_WINDOWS_H
	delete [] sizes;
#endif
	return new Int32Array(sizes, numNodes);
}

EXPORT StringArray *TreeNodeArray::getUsage()
{
#ifdef HAVE_WINDOWS_H
	const char **usages = new const char *[numNodes];
#else
	const char *usages[numNodes];
#endif
	for(int i = 0; i < numNodes; ++i)
		usages[i] = nodes[i]->getUsage();

#ifdef HAVE_WINDOWS_H
	delete [] usages;
#endif
	return new StringArray((char **)usages, numNodes);
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
	int status = TreeGetDbi(dbiItems);
	if(!(status & 1))
		throw MdsException(status);
	return new Tree(name, shot);
}

ostream &operator<<(ostream &stream, TreeNode *treeNode)
{
	return stream << treeNode->getPathStr();
}

