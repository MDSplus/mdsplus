/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <mdsobjects.h>
#include <mdsplus/mdsplus.h>
#include <mdsplus/AutoPointer.hpp>
#include <mdsplus/Mutex.hpp>
#include <usagedef.h>
#include <treeshr.h>

#include <algorithm>
#include <string>
#include <vector>
#include <cctype>

#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif


#ifdef _WIN32
#define GET_THREAD_ID GetCurrentThreadId()
#define THREAD_ID DWORD
#else
#define GET_THREAD_ID pthread_self()
#define THREAD_ID pthread_t
#endif


using namespace MDSplus;
using namespace std;

extern "C"
{
  // From treeshrp.h
  int TreeFindTag(char *tagnam, char *treename, int *tagidx);

  // From libroutines.h
  int LibConvertDateString(char *asc_time, int64_t *qtime);

  // From mdsshr.h
  const char *MdsClassString(int id);
  const char *MdsDtypeString(int id);

  // From mdsdata.c
  void freeDsc(void *dscPtr);
  void *compileFromExprWithArgs(char *expr, int nArgs, void *args, void *tree);
  void *evaluateData(void *dscPtr, int isEvaluate);
  void *deserializeData(char *serialized, int size);
  char *decompileDsc(void *dscPtr);
  void *convertFromDsc(void *dscPtr, Tree *tree);

  void *convertToScalarDsc(int clazz, int dtype, int length, char *ptr);
  void *convertToArrayDsc(int clazz, int dtype, int length, int l_length,
                          int nDims, int *dims, void *ptr);
  void *convertToCompoundDsc(int clazz, int dtype, int length, void *ptr,
                             int ndescs, void **descs);
  void *convertToApdDsc(int ndescs, void **ptr);
  void *convertToByte(void *dsc);
  void *convertToShort(void *dsc);
  void *convertToInt(void *dsc);
  void *convertToLong(void *dsc);
  void *convertToFloat(void *dsc);
  void *convertToDouble(void *dsc);
  void *convertToShape(void *dcs);

  // From mdstree.c
  void convertTime(void *dbid, int *time, char *retTime);
  int getTreeData(void *dbid, int nid, void **data, void *tree);
  int putTreeData(void *dbid, int nid, void *data);
  int deleteTreeData(void *dbid, int nid);
  int doTreeMethod(void *dbid, int nid, char *method);
  int beginTreeSegment(void *dbid, int nid, void *dataDsc, void *startDsc,
                       void *endDsc, void *timeDsc);
  int beginTreeSegmentResampled(void *dbid, int nid, void *dataDsc,
                                void *startDsc, void *endDsc, void *timeDsc,
                                int resNid, int resFactor);
  int beginTreeSegmentMinMax(void *dbid, int nid, void *dataDsc, void *startDsc,
                             void *endDsc, void *timeDsc, int resNid,
                             int resFactor);
  int makeTreeSegment(void *dbid, int nid, void *dataDsc, void *startDsc,
                      void *endDsc, void *timeDsc, int rowsFilled);
  int makeTreeSegmentResampled(void *dbid, int nid, void *dataDsc, void *startDsc,
                               void *endDsc, void *timeDsc, int rowsFilled,
                               int resNid, int resFactor);
  int makeTreeSegmentMinMax(void *dbid, int nid, void *dataDsc, void *startDsc,
                            void *endDsc, void *timeDsc, int rowsFilled,
                            int resNid, int resFactor);
  int putTreeSegment(void *dbid, int nid, void *dataDsc, int ofs);
  int putTreeSegmentResampled(void *dbid, int nid, void *dataDsc, int ofs,
                              int resNid, int resFactor);
  int putTreeSegmentMinMax(void *dbid, int nid, void *dataDsc, int ofs,
                           int resNid, int resFactor);
  int updateTreeSegment(void *dbid, int nid, int segIdx, void *startDsc,
                        void *endDsc, void *timeDsc);
  int getTreeNumSegments(void *dbid, int nid, int *numSegments);
  int getTreeSegmentScale(void *dbid, int nid, void *scale);
  int setTreeSegmentScale(void *dbid, int nid, void *scale);
  int getTreeSegmentLimits(void *dbid, int nid, int idx, void **startDsc,
                           void **endDsc);
  int getTreeSegment(void *dbid, int nid, int segIdx, void **dataDsc,
                     void **timeDsc);
  int setTreeTimeContext(void *dbid, void *startDsc, void *endDsc,
                         void *deltaDsc);
  int beginTreeTimestampedSegment(void *dbid, int nid, void *dataDsc);
  int putTreeTimestampedSegment(void *dbid, int nid, void *dataDsc,
                                int64_t *times);
  int makeTreeTimestampedSegment(void *dbid, int nid, void *dataDsc,
                                 int64_t *times, int rowsFilled);
  int setTreeXNci(void *dbid, int nid, const char *name, void *dataDsc);
  int getTreeXNci(void *dbid, int nid, const char *name, void **data, void *tree);
  int putTreeRow(void *dbid, int nid, void *dataDsc, int64_t *time, int size);
  int getTreeSegmentInfo(void *dbid, int nid, int segIdx, char *dtype,
                         char *dimct, int *dims, int *nextRow);
  // From TreeFindTagWild.c
  char *_TreeFindTagWild(void *dbid, char *wild, int *nidout, void **ctx_inout);
  char *MdsGetMsg(int status);
  void *TreeDbid();
}

#define MAX_ARGS 512

static int convertUsage(std::string const &usage)
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

static Mutex treeMutex;
static Mutex treeContextMutex;

Tree::Tree(char const *name, int shot)
    : name(name), shot(shot), fromActiveTree(false)
{
  void *ctx = 0;
  int status = _TreeOpen(&ctx, name, shot, 0);
  if (STATUS_NOT_OK)
    throw MdsException(status);
  struct TreeThreadContextInfo ttci = {GET_THREAD_ID, ctx};
  threadContextV.push_back(ttci);
  isEdit = false;
  ronly = false;
  // setActiveTree(this);
}

Tree::Tree(char const *name, int shot, void *ctx)
    : name(name), shot(shot), fromActiveTree(true) 
{
  struct TreeThreadContextInfo ttci = {GET_THREAD_ID, ctx};
  threadContextV.push_back(ttci);
}

Tree::Tree(Tree *tree)
    : name(tree->name), shot(tree->shot), fromActiveTree(true), isEdit(tree->isEdit), ronly(tree->ronly)
{
  struct TreeThreadContextInfo ttci = {GET_THREAD_ID, tree->getCtx()};
  threadContextV.push_back(ttci);
}

Tree::Tree(char const *name, int shot, char const *mode)
    : name(name), shot(shot), fromActiveTree(false)
{
  void *ctx = 0;
  std::string upMode(mode);
  std::transform(upMode.begin(), upMode.end(), upMode.begin(),
                 static_cast<int (*)(int)>(&std::toupper));

  int status = 0;
  
  if (upMode == "NORMAL")
  {
    status = _TreeOpen(&ctx, name, shot, 0);
    isEdit = false;
    ronly = false;
  }
  else if (upMode == "READONLY")
  {
    status = _TreeOpen(&ctx, name, shot, 1);
    isEdit = false;
    ronly = true;
  }
  else if (upMode == "NEW")
  {
    status = _TreeOpenNew(&ctx, name, shot);
    isEdit = true;
    ronly = false;
  }
  else if (upMode == "EDIT")
  {
    status = _TreeOpenEdit(&ctx, name, shot);
    isEdit = true;
    ronly = false;
  }
  else
  {
    throw MdsException("Invalid Open mode");
  }
  if (STATUS_NOT_OK)
    throw MdsException(status);
  struct TreeThreadContextInfo ttci = {GET_THREAD_ID, ctx};
  threadContextV.push_back(ttci);
}

Tree::~Tree()
{
  if (fromActiveTree)
    return;
  void *ctx = getCtx();
  if (isModified())
  {
    int status = _TreeQuitTree(&ctx, name.c_str(), shot);
    (void)status;
    //        if(STATUS_NOT_OK)
    //            throw MdsException(status);
  }
  else
  {
    int status = _TreeClose(&ctx, name.c_str(), shot);
    (void)status;
    //        if(STATUS_NOT_OK)
    //            throw MdsException(status);
  }
  TreeFreeDbid(ctx);
}

// WINDOWS dll force export new and delete //

EXPORT void *Tree::operator new(size_t sz) { return ::operator new(sz); }

EXPORT void Tree::operator delete(void *p) { ::operator delete(p); }

void Tree::edit(const bool st)
{
  void *ctx = 0;
  if(isEdit == st) return;
  if (isReadOnly())
    throw MdsException("Tree is read only");
  ctx = getCtx();
  int status = _TreeClose(&ctx, name.c_str(), shot);
  if (STATUS_NOT_OK)
    throw MdsException(status);
  TreeFreeDbid(ctx);
  ctx = 0;
  status = st ? _TreeOpenEdit(&ctx, name.c_str(), shot)
                  : _TreeOpen(&ctx, name.c_str(), shot, 0);
  if (STATUS_NOT_OK)
    throw MdsException(status);
  isEdit = st;
  ronly = false;
  THREAD_ID tid = GET_THREAD_ID;
  for(size_t i = 0; i < threadContextV.size(); i++)
  {
    if(threadContextV[i].tid == tid)
    {
      threadContextV[i].ctx = ctx;
      return;
    }
  }
  struct TreeThreadContextInfo ttci = {tid, ctx};
  threadContextV.push_back(ttci);
}

void Tree::write()
{
  void *ctx = getCtx();
  int status = _TreeWriteTree(&ctx, name.c_str(), shot);
  if (STATUS_NOT_OK)
    throw MdsException(status);
}


void *Tree::getCtx()
{
    AutoLock lock(treeMutex);
    THREAD_ID thisTid = GET_THREAD_ID;
    for(size_t i = 0; i < threadContextV.size(); i++)
    {
        if(threadContextV[i].tid == thisTid)
        {
            return threadContextV[i].ctx;
        }
    }
    //If we arrive here this is the first tree operation in a new thread and the tree must be opened again
    void *ctx = 0;
    int status = isEdit ? _TreeOpenEdit(&ctx, name.c_str(), shot)
                  : _TreeOpen(&ctx, name.c_str(), shot, ronly);
    if (STATUS_NOT_OK)
      throw MdsException(status);

    struct TreeThreadContextInfo ttci = {thisTid, ctx};
    threadContextV.push_back(ttci);
    return ctx;
}
    



// void Tree::quit()
//{
//	int status = _TreeQuitTree(&ctx, name.c_str(), shot);
//	if(STATUS_NOT_OK)
//		throw MdsException(status);
//}

Data *Tree::tdiEvaluate(Data *data) { return data->evaluate(this); }

Data *Tree::tdiData(Data *data) { return data->data(this); }

Data *Tree::tdiCompile(const char *expr)
{
  return MDSplus::compile(expr, this);
}

Data *Tree::tdiCompile(const char *expr, Data *arg1)
{
  return MDSplus::compileWithArgs(expr, this, 1, arg1);
}
Data *Tree::tdiCompile(const char *expr, Data *arg1, Data *arg2)
{
  return MDSplus::compileWithArgs(expr, this, 2, arg1, arg2);
}
Data *Tree::tdiCompile(const char *expr, Data *arg1, Data *arg2, Data *arg3)
{
  return MDSplus::compileWithArgs(expr, this, 3, arg1, arg2, arg3);
}
Data *Tree::tdiCompile(const char *expr, Data *arg1, Data *arg2, Data *arg3,
                       Data *arg4)
{
  return MDSplus::compileWithArgs(expr, this, 4, arg1, arg2, arg3, arg4);
}
Data *Tree::tdiCompile(const char *expr, Data *arg1, Data *arg2, Data *arg3,
                       Data *arg4, Data *arg5)
{
  return MDSplus::compileWithArgs(expr, this, 5, arg1, arg2, arg3, arg4, arg5);
}
Data *Tree::tdiCompile(const char *expr, Data *arg1, Data *arg2, Data *arg3,
                       Data *arg4, Data *arg5, Data *arg6)
{
  return MDSplus::compileWithArgs(expr, this, 6, arg1, arg2, arg3, arg4, arg5,
                                  arg6);
}
Data *Tree::tdiCompile(const char *expr, Data *arg1, Data *arg2, Data *arg3,
                       Data *arg4, Data *arg5, Data *arg6, Data *arg7)
{
  return MDSplus::compileWithArgs(expr, this, 7, arg1, arg2, arg3, arg4, arg5,
                                  arg6, arg7);
}
Data *Tree::tdiCompile(const char *expr, Data *arg1, Data *arg2, Data *arg3,
                       Data *arg4, Data *arg5, Data *arg6, Data *arg7,
                       Data *arg8)
{
  return MDSplus::compileWithArgs(expr, this, 8, arg1, arg2, arg3, arg4, arg5,
                                  arg6, arg7, arg8);
}

Data *Tree::tdiExecute(const char *expr) { return execute(expr, this); }

Data *Tree::tdiExecute(const char *expr, Data *arg1)
{
  return MDSplus::executeWithArgs(expr, this, 1, arg1);
}
Data *Tree::tdiExecute(const char *expr, Data *arg1, Data *arg2)
{
  return MDSplus::executeWithArgs(expr, this, 2, arg1, arg2);
}
Data *Tree::tdiExecute(const char *expr, Data *arg1, Data *arg2, Data *arg3)
{
  return MDSplus::executeWithArgs(expr, this, 3, arg1, arg2, arg3);
}
Data *Tree::tdiExecute(const char *expr, Data *arg1, Data *arg2, Data *arg3,
                       Data *arg4)
{
  return MDSplus::executeWithArgs(expr, this, 4, arg1, arg2, arg3, arg4);
}
Data *Tree::tdiExecute(const char *expr, Data *arg1, Data *arg2, Data *arg3,
                       Data *arg4, Data *arg5)
{
  return MDSplus::executeWithArgs(expr, this, 5, arg1, arg2, arg3, arg4, arg5);
}
Data *Tree::tdiExecute(const char *expr, Data *arg1, Data *arg2, Data *arg3,
                       Data *arg4, Data *arg5, Data *arg6)
{
  return MDSplus::executeWithArgs(expr, this, 6, arg1, arg2, arg3, arg4, arg5,
                                  arg6);
}
Data *Tree::tdiExecute(const char *expr, Data *arg1, Data *arg2, Data *arg3,
                       Data *arg4, Data *arg5, Data *arg6, Data *arg7)
{
  return MDSplus::executeWithArgs(expr, this, 7, arg1, arg2, arg3, arg4, arg5,
                                  arg6, arg7);
}
Data *Tree::tdiExecute(const char *expr, Data *arg1, Data *arg2, Data *arg3,
                       Data *arg4, Data *arg5, Data *arg6, Data *arg7,
                       Data *arg8)
{
  return MDSplus::executeWithArgs(expr, this, 8, arg1, arg2, arg3, arg4, arg5,
                                  arg6, arg7, arg8);
}

TreeNode *Tree::addNode(char const *name, char const *usage)
{
  int newNid;
  int status = _TreeAddNode(getCtx(), name, &newNid, convertUsage(usage));
  if (STATUS_NOT_OK)
    throw MdsException(status);
  return new TreeNode(newNid, this);
}

TreeNode *Tree::addDevice(char const *name, char const *type)
{
  int newNid;
  int status = _TreeAddConglom(getCtx(), name, type, &newNid);
  if (STATUS_NOT_OK)
    throw MdsException(status);
  return new TreeNode(newNid, this);
}

void Tree::remove(char const *name)
{
  int count;
  AutoPointer<TreeNode> delNode(getNode(name));
  int status = _TreeDeleteNodeInitialize(getCtx(), delNode.ptr->getNid(), &count, 1);
  if (STATUS_OK)
    _TreeDeleteNodeExecute(getCtx());
  if (STATUS_NOT_OK)
    throw MdsException(status);
}

TreeNode *Tree::getNode(char const *path)
{
  int nid;

  int status = _TreeFindNode(getCtx(), path, &nid);
  if (STATUS_NOT_OK)
  {
    throw MdsException(status);
  }
  return new TreeNode(nid, this);
}

TreeNode *Tree::getNode(TreePath *path)
{
  AutoArray<char> pathName(path->getString());
  return getNode(pathName.ptr);
}

TreeNode *Tree::getNode(String *path)
{
  AutoArray<char> pathName(path->getString());
  return getNode(pathName.ptr);
}

TreeNodeArray *Tree::getNodeWild(char const *path, int usageMask)
{
  // FIXME: This function doesn't need nids at all. Store a vector of TreeNodes
  // built right in the while loop.  Need better testing before that change,
  // though.
  int status;
  void *wildCtx = 0;

  std::vector<int> nids;
  nids.reserve(10000);

  int temp = 0;
  while ((status = _TreeFindNodeWild(getCtx(), path, &temp, &wildCtx, usageMask)) &
         1)
    nids.push_back(temp);
  _TreeFindNodeEnd(getCtx(), &wildCtx);

  TreeNode **retNodes = new TreeNode *[nids.size()];
  for (std::size_t i = 0; i < nids.size(); ++i)
    retNodes[i] = new TreeNode(nids[i], this);

  TreeNodeArray *nodeArray = new TreeNodeArray(retNodes, nids.size());
  delete[] retNodes;
  return nodeArray;
}

TreeNodeArray *Tree::getNodeWild(char const *path)
{
  return getNodeWild(path, -1);
}

void Tree::setDefault(TreeNode *treeNode)
{
  int status = _TreeSetDefaultNid(getCtx(), treeNode->getNid());
  if (STATUS_NOT_OK)
    throw MdsException(status);
}

TreeNode *Tree::getDefault()
{
  int nid;

  int status = _TreeGetDefaultNid(getCtx(), &nid);
  if (STATUS_NOT_OK)
    throw MdsException(status);
  return new TreeNode(nid, this);
}

static bool dbiTest(void *ctx, short int code)
{
  int supports;
  int len;
  struct dbi_itm dbiList[] = {{sizeof(int), code, &supports, &len},
                              {0, DbiEND_OF_LIST, 0, 0}};

  int status = _TreeGetDbi(ctx, dbiList);
  if (STATUS_NOT_OK)
    throw MdsException(status);

  return supports ? true : false;
}

bool Tree::versionsInPulseEnabled()
{
  return dbiTest(getCtx(), DbiVERSIONS_IN_PULSE);
}

bool Tree::versionsInModelEnabled()
{
  return dbiTest(getCtx(), DbiVERSIONS_IN_MODEL);
}

bool Tree::isModified() { return dbiTest(getCtx(), DbiMODIFIED); }

bool Tree::isOpenForEdit() { return dbiTest(getCtx(), DbiOPEN_FOR_EDIT); }

bool Tree::isReadOnly() { return dbiTest(getCtx(), DbiOPEN_READONLY); }

static void dbiSet(void *ctx, short int code, bool value)
{
  int len;
  int intVal = value ? 1 : 0;
  struct dbi_itm dbiList[] = {{sizeof(int), code, &intVal, &len},
                              {0, DbiEND_OF_LIST, 0, 0}};

  int status = _TreeSetDbi(ctx, dbiList);
  if (STATUS_NOT_OK)
    throw MdsException(status);
}

void Tree::setVersionsInModel(bool verEnabled)
{
  dbiSet(getCtx(), DbiVERSIONS_IN_MODEL, verEnabled);
}

void Tree::setVersionsInPulse(bool verEnabled)
{
  dbiSet(getCtx(), DbiVERSIONS_IN_PULSE, verEnabled);
}

void Tree::setViewDate(char *date)
{
  int64_t qtime;

  int status = LibConvertDateString(date, &qtime);
  if (STATUS_NOT_OK)
    throw MdsException("Invalid date format");
  status = TreeSetViewDate(&qtime);
  if (STATUS_NOT_OK)
    throw MdsException(status);
}

void Tree::setTimeContext(Data *start, Data *end, Data *delta)
{
  int status = setTreeTimeContext(getCtx(), (start) ? start->convertToDsc() : 0,
                                  (end) ? end->convertToDsc() : 0,
                                  (delta) ? delta->convertToDsc() : 0);
  if (STATUS_NOT_OK)
    throw MdsException(status);
}

void Tree::setCurrent(char const *treeName, int shot)
{
  int status = TreeSetCurrentShotId(treeName, shot);
  if (STATUS_NOT_OK)
    throw MdsException(status);
}

int Tree::getCurrent(char const *treeName)
{
  return TreeGetCurrentShotId(treeName);
}

void Tree::createPulse(int shot)
{
  int status = _TreeCreatePulseFile(getCtx(), shot, 0, NULL);
  if (STATUS_NOT_OK)
    throw MdsException(status);
}

void Tree::deletePulse(int shot)
{
  int status = _TreeDeletePulseFile(getCtx(), shot, 1);
  if (STATUS_NOT_OK)
    throw MdsException(status);
}

StringArray *Tree::findTags(char *wild)
{
  const int MAX_TAGS = 1024;
  char *tagNames[MAX_TAGS];
  void *ctx = 0;
  int nTags = 0;
  int nidOut;
  for (nTags = 0; nTags < MAX_TAGS; nTags++)
  {
    char *currTag = _TreeFindTagWild(getCtx(), wild, &nidOut, &ctx);
    if (!currTag)
      break;
    tagNames[nTags] = new char[strlen(currTag) + 1];
    strcpy(tagNames[nTags], currTag);
  }
  StringArray *stArr = new StringArray(tagNames, nTags);
  for (int i = 0; i < nTags; i++)
    delete[] tagNames[i];
  return stArr;
}
void Tree::removeTag(char const *tagName)
{
  int status = _TreeRemoveTag(getCtx(), tagName);
  if (STATUS_NOT_OK)
    throw MdsException(status);
}

int64_t Tree::getDatafileSize()
{
  int64_t size = _TreeGetDatafileSize(getCtx());
  if (size == -1)
    throw MdsException("Cannot retrieve datafile size");
  return size;
}

////////////////////////////////////////////////////////////////////////////////
//  TreeNode  //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void *TreeNode::convertToDsc()
{
  //	AutoLock lock(treeMutex);
  //	setActiveTree(tree);
  void *retDsc = completeConversionToDsc(
      convertToScalarDsc(clazz, dtype, sizeof(int), (char *)&nid));

  return retDsc;
}

Data *TreeNode::data()
{
  Data *d = getData();
  AutoLock lock(treeMutex);
  setActiveTree(tree);
  Data *outD = d->data();
  MDSplus::deleteData(d);
  return outD;
}

char TreeNode::getByte()
{
  Data *d = data();
  char res = d->getByte();
  MDSplus::deleteData(d);
  return res;
}
short TreeNode::getShort()
{
  Data *d = data();
  short res = d->getShort();
  MDSplus::deleteData(d);
  return res;
}
int TreeNode::getInt()
{
  Data *d = data();
  int res = d->getInt();
  MDSplus::deleteData(d);
  return res;
}
int64_t TreeNode::getLong()
{
  Data *d = data();
  long res = d->getLong();
  MDSplus::deleteData(d);
  return res;
}
unsigned char TreeNode::getByteUnsigned()
{
  Data *d = data();
  unsigned char res = d->getByteUnsigned();
  MDSplus::deleteData(d);
  return res;
}
unsigned short TreeNode::getShortUnsigned()
{
  Data *d = data();
  unsigned short res = d->getShortUnsigned();
  MDSplus::deleteData(d);
  return res;
}
unsigned int TreeNode::getIntUnsigned()
{
  Data *d = data();
  unsigned int res = d->getIntUnsigned();
  MDSplus::deleteData(d);
  return res;
}
uint64_t TreeNode::getLongUnsigned()
{
  Data *d = data();
  uint64_t res = d->getLongUnsigned();
  MDSplus::deleteData(d);
  return res;
}
float TreeNode::getFloat()
{
  Data *d = data();
  float res = d->getFloat();
  MDSplus::deleteData(d);
  return res;
}
double TreeNode::getDouble()
{
  Data *d = data();
  double res = d->getDouble();
  MDSplus::deleteData(d);
  return res;
}
std::complex<double> TreeNode::getComplex()
{
  Data *d = data();
  std::complex<double> res = d->getComplex();
  MDSplus::deleteData(d);
  return res;
}
char *TreeNode::getByteArray(int *numElements)
{
  Data *d = data();
  char * res = d->getByteArray(numElements);
  MDSplus::deleteData(d);
  return res;
}
std::vector<char> TreeNode::getByteArray()
{
  Data *d = data();
  std::vector<char> res = d->getByteArray();
  MDSplus::deleteData(d);
  return res;
}
short *TreeNode::getShortArray(int *numElements)
{
  Data *d = data();
  short * res = d->getShortArray(numElements);
  MDSplus::deleteData(d);
  return res;
}
std::vector<short> TreeNode::getShortArray()
{
  Data *d = data();
  std::vector<short>  res = d->getShortArray();
  MDSplus::deleteData(d);
  return res;
}
int *TreeNode::getIntArray(int *numElements)
{
  Data *d = data();
  int * res = d->getIntArray(numElements);
  MDSplus::deleteData(d);
  return res;
}
std::vector<int> TreeNode::getIntArray()
{
  Data *d = data();
  std::vector<int> res = d->getIntArray();
  MDSplus::deleteData(d);
  return res;
}
int64_t *TreeNode::getLongArray(int *numElements)
{
  Data *d = data();
  int64_t * res = d->getLongArray(numElements);
  MDSplus::deleteData(d);
  return res;
}
std::vector<int64_t> TreeNode::getLongArray()
{
  Data *d = data();
  std::vector<int64_t> res = d->getLongArray();
  MDSplus::deleteData(d);
  return res;
}
float *TreeNode::getFloatArray(int *numElements)
{
  Data *d = data();
  float * res = d->getFloatArray(numElements);
  MDSplus::deleteData(d);
  return res;
}
std::vector<float> TreeNode::getFloatArray()
{
  Data *d = data();
  std::vector<float> res = d->getFloatArray();
  MDSplus::deleteData(d);
  return res;
}
unsigned char *TreeNode::getByteUnsignedArray(int *numElements)
{
  Data *d = data();
  unsigned char * res = d->getByteUnsignedArray(numElements);
  MDSplus::deleteData(d);
  return res;
}
std::vector<unsigned char> TreeNode::getByteUnsignedArray()
{
  Data *d = data();
  std::vector<unsigned char> res = d->getByteUnsignedArray();
  MDSplus::deleteData(d);
  return res;
}
unsigned short *TreeNode::getShortUnsignedArray(int *numElements)
{
  Data *d = data();
  unsigned short * res = d->getShortUnsignedArray(numElements);
  MDSplus::deleteData(d);
  return res;
}
std::vector<unsigned short> TreeNode::getShortUnsignedArray()
{
  Data *d = data();
  std::vector<unsigned short> res = d->getShortUnsignedArray();
  MDSplus::deleteData(d);
  return res;
}
unsigned int *TreeNode::getIntUnsignedArray(int *numElements)
{
  Data *d = data();
  unsigned int * res = d->getIntUnsignedArray(numElements);
  MDSplus::deleteData(d);
  return res;
}
std::vector<unsigned int> TreeNode::getIntUnsignedArray()
{
  Data *d = data();
  std::vector<unsigned int> res = d->getIntUnsignedArray();
  MDSplus::deleteData(d);
  return res;
}
uint64_t *TreeNode::getLongUnsignedArray(int *numElements)
{
  Data *d = data();
  uint64_t * res = d->getLongUnsignedArray(numElements);
  MDSplus::deleteData(d);
  return res;
}
std::vector<uint64_t> TreeNode::getLongUnsignedArray()
{
  Data *d = data();
  std::vector<uint64_t> res = d->getLongUnsignedArray();
  MDSplus::deleteData(d);
  return res;
}
double *TreeNode::getDoubleArray(int *numElements)
{
  Data *d = data();
  double * res = d->getDoubleArray(numElements);
  MDSplus::deleteData(d);
  return res;
}
std::vector<double> TreeNode::getDoubleArray()
{
  Data *d = data();
  std::vector<double> res = d->getDoubleArray();
  MDSplus::deleteData(d);
  return res;
}
std::complex<double> *TreeNode::getComplexArray(int *numElements)
{
  Data *d = data();
  std::complex<double> *res = d->getComplexArray(numElements);
  MDSplus::deleteData(d);
  return res;
}
std::vector<std::complex<double> > TreeNode::getComplexArray()
{
  Data *d = data();
  std::vector<std::complex<double> > res = d->getComplexArray();
  MDSplus::deleteData(d);
  return res;
}
char **TreeNode::getStringArray(int *numElements)
{
  Data *d = data();
  char ** res = d->getStringArray(numElements);
  MDSplus::deleteData(d);
  return res;
}





template <class T>
static T getNci(void *ctx, int nid, short int code)
{
  T value = 0;
  int len;
  struct nci_itm nciList[] = {{sizeof(T), (nci_t)code, &value, &len},
                              {0, NciEND_OF_LIST, 0, 0}};

  int status = _TreeGetNci(ctx, nid, nciList);
  if (STATUS_NOT_OK)
    throw MdsException(status);

  return value;
}

template <>
std::string getNci(void *ctx, int nid, short int code)
{
  char value[1024] = {};
  int len;
  struct nci_itm nciList[] = {{sizeof(value), (nci_t)code, &value, &len},
                              {0, NciEND_OF_LIST, 0, 0}};

  int status = _TreeGetNci(ctx, nid, nciList);
  if (STATUS_NOT_OK)
    throw MdsException(status);

  return value;
}

void TreeNode::setFlag(int flagOfs, bool flag)
{
  int nciFlags;
  int status;
  int nciFlagsLen = sizeof(int);
  struct nci_itm setNciList[] = {{4, NciSET_FLAGS, &nciFlags, &nciFlagsLen},
                                 {0, NciEND_OF_LIST, 0, 0}};
  struct nci_itm clearNciList[] = {{4, NciCLEAR_FLAGS, &nciFlags, &nciFlagsLen},
                                   {0, NciEND_OF_LIST, 0, 0}};
  resolveNid();
  nciFlags = flagOfs;
  if (flag)
    status = _TreeSetNci(tree->getCtx(), nid, setNciList);
  else
    status = _TreeSetNci(tree->getCtx(), nid, clearNciList);
  if (STATUS_NOT_OK)
    throw MdsException(status);
}

TreeNode::TreeNode(int nid, Tree *tree, Data *units, Data *error, Data *help,
                   Data *validation)
{
  this->tree = 0;
  if (!tree && nid != 0)
    this->tree = getActiveTree();
  if (!tree && nid != 0) // exclude the case in which this constructor has been
                         // called in a TreePath instantiation
    throw MdsException(
        "A Tree instance must be defined when ceating TreeNode instances");
  this->nid = nid;
  this->tree = new Tree(tree);
  clazz = CLASS_S;
  dtype = DTYPE_NID;
  setAccessory(units, error, help, validation);
}

void TreeNode::setTree(Tree *tree)
{
  if (this->tree)
    delete this->tree;
  this->tree = new Tree(tree);
}

EXPORT void *TreeNode::operator new(size_t sz) { return ::operator new(sz); }
EXPORT void TreeNode::operator delete(void *p) { ::operator delete(p); }

TreeNode::~TreeNode()
{
  if (tree)
    delete tree;
}

std::string TreeNode::getNciString(int itm)
{
  return getNci<std::string>(tree->getCtx(), nid, itm);
}

char TreeNode::getNciChar(int itm)
{
  return getNci<char>(tree->getCtx(), nid, itm);
}

int TreeNode::getNciInt(int itm)
{
  return getNci<int>(tree->getCtx(), nid, itm);
}

int64_t TreeNode::getNciInt64(int itm)
{
  return getNci<int64_t>(tree->getCtx(), nid, itm);
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
  return AutoString(getPath()).string;
}

char *TreeNode::getMinPath()
{
  resolveNid();
  std::string path = getNciString(NciMINPATH);
  char *retPath = new char[path.length() + 1];
  strcpy(retPath, path.c_str());
  return retPath;
}

std::string TreeNode::getMinPathStr()
{
  return AutoString(getMinPath()).string;
}

char *TreeNode::getFullPath()
{
  resolveNid();
  std::string path = getNciString(NciFULLPATH);
  char *retPath = new char[path.length() + 1];
  strcpy(retPath, path.c_str());
  return retPath;
}

std::string TreeNode::getFullPathStr()
{
  return AutoString(getFullPath()).string;
}

char *TreeNode::getOriginalPartName()
{
  resolveNid();
  std::string path = getNciString(NciORIGINAL_PART_NAME);
  char *retPath = new char[path.length() + 1];
  strcpy(retPath, path.c_str());
  return retPath;
}

std::string TreeNode::getOriginalPartNameStr()
{
  return AutoString(getOriginalPartName()).string;
}

char *TreeNode::getNodeName()
{
  resolveNid();
  std::string path = getNciString(NciNODE_NAME);
  char *retPath = new char[path.length() + 1];
  strcpy(retPath, path.c_str());

  // Trim
  for (int i = strlen(retPath) - 1; i > 0 && retPath[i] == ' '; i--)
    retPath[i] = 0;
  return retPath;
}

std::string TreeNode::getNodeNameStr()
{
  return AutoString(getNodeName()).string;
}

Data *TreeNode::getData()
{
  Data *data = 0;
  resolveNid();
  int status = getTreeData(tree->getCtx(), nid, (void **)&data, tree);
  if (STATUS_NOT_OK)
  {
    throw MdsException(status);
  }
  return data;
}

void TreeNode::putData(Data *data)
{
  resolveNid();
  int status = putTreeData(tree->getCtx(), nid, (void *)data);
  if (STATUS_NOT_OK)
  {
    throw MdsException(status);
  }
}

void TreeNode::deleteData()
{
  resolveNid();
  int status = deleteTreeData(tree->getCtx(), nid);
  if (STATUS_NOT_OK)
  {
    throw MdsException(status);
  }
}

bool TreeNode::isOn()
{
  resolveNid();
  return (_TreeIsOn(tree->getCtx(), nid) & 1) ? true : false;
}

void TreeNode::setOn(bool on)
{
  resolveNid();
  if (on)
    _TreeTurnOn(tree->getCtx(), nid);
  else
    _TreeTurnOff(tree->getCtx(), nid);
}

int TreeNode::getLength()
{
  resolveNid();
  return getNciInt(NciLENGTH);
}

int TreeNode::getCompressedLength()
{
  resolveNid();
  return getNciInt(NciRLENGTH);
}

int TreeNode::getStatus()
{
  resolveNid();
  return getNciInt(NciSTATUS);
}

int TreeNode::getOwnerId()
{
  resolveNid();
  return getNciInt(NciOWNER_ID);
}

int64_t TreeNode::getTimeInserted()
{
  resolveNid();
  return getNciInt64(NciTIME_INSERTED);
}

Data *TreeNode::getExtendedAttribute(const char *name)
{
  Data *data = NULL;
  int status = getTreeXNci(tree->getCtx(), nid, name, (void **)&data, tree);
  if (STATUS_NOT_OK)
    throw MdsException(status);
  return data;
}

Data *TreeNode::getExtendedAttribute(std::string name)
{
  return getExtendedAttribute(name.c_str());
}

void TreeNode::setExtendedAttribute(const char *name, Data *data)
{
  resolveNid();
  int status = setTreeXNci(tree->getCtx(), nid, name, data->convertToDsc());
  if (STATUS_NOT_OK)
    throw MdsException(status);
}

void TreeNode::setExtendedAttribute(std::string name, Data *data)
{
  resolveNid();
  int status =
      setTreeXNci(tree->getCtx(), nid, name.c_str(), data->convertToDsc());
  if (STATUS_NOT_OK)
    throw MdsException(status);
}

void TreeNode::doMethod(char *method)
{
  resolveNid();
  int status = doTreeMethod(tree->getCtx(), nid, method);
  if (STATUS_NOT_OK)
    throw MdsException(status);
}

bool TreeNode::getFlag(int flagOfs)
{
  resolveNid();
  int nciFlags = getNciInt(NciGET_FLAGS);
  return (nciFlags & flagOfs) ? true : false;
}

bool TreeNode::isSetup() { return getFlag(NciM_SETUP_INFORMATION); }

bool TreeNode::isWriteOnce() { return getFlag(NciM_WRITE_ONCE); }

void TreeNode::setWriteOnce(bool flag) { setFlag(NciM_WRITE_ONCE, flag); }

bool TreeNode::isCompressOnPut() { return getFlag(NciM_COMPRESS_ON_PUT); }

void TreeNode::setCompressOnPut(bool flag)
{
  setFlag(NciM_COMPRESS_ON_PUT, flag);
}

bool TreeNode::isNoWriteModel() { return getFlag(NciM_NO_WRITE_MODEL); }

void TreeNode::setNoWriteModel(bool flag)
{
  setFlag(NciM_NO_WRITE_MODEL, flag);
}

bool TreeNode::isEssential() { return getFlag(NciM_ESSENTIAL); }

void TreeNode::setEssential(bool flag) { setFlag(NciM_ESSENTIAL, flag); }

bool TreeNode::isNoWriteShot() { return getFlag(NciM_NO_WRITE_SHOT); }

void TreeNode::setNoWriteShot(bool flag) { setFlag(NciM_NO_WRITE_SHOT, flag); }

bool TreeNode::isIncludedInPulse() { return TreeNode::isIncludeInPulse(); }
bool TreeNode::isIncludeInPulse() { return getFlag(NciM_INCLUDE_IN_PULSE); }

bool TreeNode::containsVersions() { return getFlag(NciM_VERSIONS); }

bool TreeNode::isMember()
{
  resolveNid();
  int par = getNciInt(NciPARENT_RELATIONSHIP);
  return (par & NciK_IS_MEMBER) ? true : false;
}

bool TreeNode::isChild()
{
  resolveNid();
  int par = getNciInt(NciPARENT_RELATIONSHIP);
  return (par & NciK_IS_CHILD) ? true : false;
}

void TreeNode::setIncludedInPulse(bool flag)
{
  TreeNode::setIncludeInPulse(flag);
}
void TreeNode::setIncludeInPulse(bool flag)
{
  setFlag(NciM_INCLUDE_IN_PULSE, flag);
}

TreeNode *TreeNode::getParent()
{
  resolveNid();
  int nodeId = getNciInt(NciPARENT);
  return new TreeNode(nodeId, tree);
}

TreeNode *TreeNode::getBrother()
{
  resolveNid();
  int nodeId = getNciInt(NciBROTHER);
  return new TreeNode(nodeId, tree);
}

TreeNode *TreeNode::getChild()
{
  resolveNid();
  int nodeId = getNciInt(NciCHILD);
  return new TreeNode(nodeId, tree);
}

TreeNode *TreeNode::getMember()
{
  resolveNid();
  int nodeId = getNciInt(NciMEMBER);
  return new TreeNode(nodeId, tree);
}

int TreeNode::getNumChildren()
{
  resolveNid();
  return getNciInt(NciNUMBER_OF_CHILDREN);
}

int TreeNode::getNumMembers()
{
  resolveNid();
  return getNciInt(NciNUMBER_OF_MEMBERS);
}

int TreeNode::getNumDescendants()
{
  resolveNid();
  int members = getNciInt(NciNUMBER_OF_MEMBERS);
  int children = getNciInt(NciNUMBER_OF_CHILDREN);
  return members + children;
}

TreeNode **TreeNode::getDescendants(int *numDescendants)
{
  int numMembers;
  TreeNode **members = getMembers(&numMembers);

  int nChildren;
  TreeNode **children = getChildren(&nChildren);

  int total = numMembers + nChildren;
  TreeNode **retDescendants = new TreeNode *[total];

  for (int i = 0; i < numMembers; ++i)
    retDescendants[i] = members[i];
  delete[] members;

  for (int i = 0; i < nChildren; ++i)
    retDescendants[i + numMembers] = children[i];
  delete[] children;

  *numDescendants = total;
  return retDescendants;
}

TreeNode **TreeNode::getChildren(int *numChildren)
{
  int nidCnt = getNumChildren();
  std::vector<int> nids(nidCnt);
  *numChildren = nidCnt;

  // FIXME: int and short as size == bad
  int retLen = sizeof(int) * (nidCnt);
  struct nci_itm nciList1[] = {
      {(short)retLen, NciCHILDREN_NIDS, &nids[0], &retLen},
      {0, NciEND_OF_LIST, 0, 0}};

  int status = _TreeGetNci(tree->getCtx(), nid, nciList1);
  if (STATUS_NOT_OK)
    throw MdsException(status);

  TreeNode **retNodes = new TreeNode *[nidCnt];
  for (int i = 0; i < nidCnt; i++)
    retNodes[i] = new TreeNode(nids[i], tree);
  return retNodes;
}

TreeNode **TreeNode::getMembers(int *numMembers)
{
  int nidCnt = getNumMembers();
  std::vector<int> nids(nidCnt);
  *numMembers = nidCnt;

  // FIXME: int and short as size == bad
  int retLen = sizeof(int) * (nidCnt);
  struct nci_itm nciList1[] = {
      {(short)retLen, NciMEMBER_NIDS, &nids[0], &retLen},
      {0, NciEND_OF_LIST, 0, 0}};

  int status = _TreeGetNci(tree->getCtx(), nid, nciList1);
  if (STATUS_NOT_OK)
    throw MdsException(status);

  TreeNode **retNodes = new TreeNode *[nidCnt];
  for (int i = 0; i < nidCnt; i++)
    retNodes[i] = new TreeNode(nids[i], tree);
  return retNodes;
}

const char *TreeNode::getClass()
{
  resolveNid();
  char type = getNciChar(NciCLASS);
  return MdsClassString(type);
}

const char *TreeNode::getDType()
{
  resolveNid();
  char type = getNciChar(NciDTYPE);
  return MdsDtypeString(type);
}

const char *TreeNode::getUsage()
{
  resolveNid();
  int usage = getNciInt(NciUSAGE);

  switch (usage)
  {
  case TreeUSAGE_ACTION:
    return "ACTION";
  case TreeUSAGE_ANY:
    return "ANY";
  case TreeUSAGE_AXIS:
    return "AXIS";
  case TreeUSAGE_COMPOUND_DATA:
    return "COMPOUND_DATA";
  case TreeUSAGE_DEVICE:
    return "DEVICE";
  case TreeUSAGE_DISPATCH:
    return "DISPATCH";
  case TreeUSAGE_STRUCTURE:
    return "STRUCTURE";
  case TreeUSAGE_NUMERIC:
    return "NUMERIC";
  case TreeUSAGE_SIGNAL:
    return "SIGNAL";
  case TreeUSAGE_SUBTREE:
    return "SUBTREE";
  case TreeUSAGE_TASK:
    return "TASK";
  case TreeUSAGE_TEXT:
    return "TEXT";
  case TreeUSAGE_WINDOW:
    return "WINDOW";
  }
  return "Unknown";
}

int TreeNode::getConglomerateElt()
{
  resolveNid();
  return getNciChar(NciCONGLOMERATE_ELT);
}

int TreeNode::getNumElts()
{
  resolveNid();
  return getNciInt(NciNUMBER_OF_ELTS);
}

TreeNodeArray *TreeNode::getConglomerateNodes()
{
  int nNidsLen, retLen;
  int nNids;
  struct nci_itm nciList[] = {{4, NciNUMBER_OF_ELTS, (char *)&nNids, &nNidsLen},
                              {0, NciEND_OF_LIST, 0, 0}};

  resolveNid();
  int status = _TreeGetNci(tree->getCtx(), nid, nciList);
  if (STATUS_NOT_OK)
    throw MdsException(status);

  int *nids = new int[nNids];
  struct nci_itm nciList1[] = {
      {(short)(4 * nNids), NciCONGLOMERATE_ELT, (char *)nids, &retLen},
      {0, NciEND_OF_LIST, 0, 0}};

  status = _TreeGetNci(tree->getCtx(), nid, nciList1);
  if (STATUS_NOT_OK)
    throw MdsException(status);

  TreeNodeArray *resArray = new TreeNodeArray(nids, nNids, tree);
  delete[] nids;
  return resArray;
}

int TreeNode::getDepth()
{
  resolveNid();
  return getNciInt(NciDEPTH);
}

#ifdef _WIN32
#define pthread_mutex_t int
// static void LockMdsShrMutex(){}
// static void UnlockMdsShrMutex(){}
#endif

void TreeNode::makeSegment(Data *start, Data *end, Data *time,
                           Array *initialData)
{
  resolveNid();
  int numDims;
  int *shape = initialData->getShape(&numDims);
  // if(tree) tree->lock();
  int status =
      makeTreeSegment(tree->getCtx(), getNid(), initialData->convertToDsc(),
                      start->convertToDsc(), end->convertToDsc(),
                      time->convertToDsc(), shape[0]);
  deleteNativeArray(shape);
  // if(tree) tree->unlock();
  if (STATUS_NOT_OK)
    throw MdsException(status);
}

void TreeNode::makeSegmentMinMax(Data *start, Data *end, Data *time,
                                 Array *initialData, TreeNode *resampledNode,
                                 int resFactor)
{
  resolveNid();
  int numDims;
  int *shape = initialData->getShape(&numDims);
  // if(tree) tree->lock();
  int status = makeTreeSegmentMinMax(
      tree->getCtx(), getNid(), initialData->convertToDsc(),
      start->convertToDsc(), end->convertToDsc(), time->convertToDsc(),
      shape[0], resampledNode->getNid(), resFactor);
  deleteNativeArray(shape);
  // if(tree) tree->unlock();
  if (STATUS_NOT_OK)
    throw MdsException(status);
}

void TreeNode::makeSegmentResampled(Data *start, Data *end, Data *time,
                                    Array *initialData, TreeNode *resampledNode,
                                    int resFactor)
{
  resolveNid();
  int numDims;
  int *shape = initialData->getShape(&numDims);
  // if(tree) tree->lock();
  int status = makeTreeSegmentResampled(
      tree->getCtx(), getNid(), initialData->convertToDsc(),
      start->convertToDsc(), end->convertToDsc(), time->convertToDsc(),
      shape[0], resampledNode->getNid(), resFactor);
  deleteNativeArray(shape);
  // if(tree) tree->unlock();
  if (STATUS_NOT_OK)
    throw MdsException(status);
}
void TreeNode::beginSegment(Data *start, Data *end, Data *time,
                            Array *initialData)
{
  resolveNid();
  // if(tree) tree->lock();
  int status = beginTreeSegment(
      tree->getCtx(), getNid(), initialData->convertToDsc(),
      start->convertToDsc(), end->convertToDsc(), time->convertToDsc());
  // if(tree) tree->unlock();
  if (STATUS_NOT_OK)
    throw MdsException(status);
}
void TreeNode::beginSegmentResampled(Data *start, Data *end, Data *time,
                                     Array *initialData,
                                     TreeNode *resampledNode, int resFactor)
{
  resolveNid();
  int status = beginTreeSegmentResampled(
      tree->getCtx(), getNid(), initialData->convertToDsc(),
      start->convertToDsc(), end->convertToDsc(), time->convertToDsc(),
      resampledNode->getNid(), resFactor);
  if (STATUS_NOT_OK)
    throw MdsException(status);
}
void TreeNode::beginSegmentMinMax(Data *start, Data *end, Data *time,
                                  Array *initialData, TreeNode *resampledNode,
                                  int resFactor)
{
  resolveNid();
  int status = beginTreeSegmentMinMax(
      tree->getCtx(), getNid(), initialData->convertToDsc(),
      start->convertToDsc(), end->convertToDsc(), time->convertToDsc(),
      resampledNode->getNid(), resFactor);
  if (STATUS_NOT_OK)
    throw MdsException(status);
}
/*void TreeNode::beginSegmentMinMax(Data *start, Data *end, Data *time, Array
*initialData, TreeNode*resampledNode, int resFactor)
{
        //Resampled aray always converted to float, Assumed 1D array
        int numRows;
        float *arrSamples = initialData->getFloatArray(&numRows);
        float *resSamples = new float[2* numRows/resFactor + 1]; //It has top
keep minimum and maximum. Ensure enough room even if numRows is not a multiplier
of resFactor for(int i = 0; i < numRows; i+= resFactor)
        {
                float minVal = arrSamples[i];
                float maxVal = minVal;
                for(int j = 0; j < resFactor && i+j < numRows; j++)
                {
                        if(arrSamples[i+j] < minVal)
                                minVal = arrSamples[i+j];
                        if(arrSamples[i+j] > maxVal)
                                maxVal = arrSamples[i+j];
                }
                resSamples[2*i/resFactor] = minVal;
                resSamples[2*i/resFactor+1] = maxVal;
        }
        AutoData<Array> resData(new Float32Array(resSamples, 2*
numRows/resFactor)); char dimExpr[64];
        sprintf(dimExpr,"BUILD_RANGE($1,$2,%d*($3-$4)/%d)",
resFactor/2,numRows); AutoData<Data> resDim(compileWithArgs(dimExpr, getTree(),
4, start, end, end, start)); resampledNode->beginSegment(start, end, resDim,
resData); delete[] arrSamples; delete[] resSamples;

        int numSegments = getNumSegments();
        if(numSegments == 0)  //Set XNCI only when writing the first segment
        {
                AutoData<Data> resModeD(new String("MinMax"));
                setTreeXNci(tree->getCtx(), nid, "ResampleMode",
resModeD->convertToDsc()); AutoData<Data> resSamplesD(new Int32(resFactor/2));
                setTreeXNci(tree->getCtx(), nid, "ResampleFactor",
resSamplesD->convertToDsc()); setTreeXNci(tree->getCtx(), nid, "ResampleNid",
resampledNode->convertToDsc());
        }
        beginSegment(start, end, time, initialData);
}
*/
void TreeNode::putSegment(Array *data, int ofs)
{
  resolveNid();
  // if(tree) tree->lock();
  int status =
      putTreeSegment(tree->getCtx(), getNid(), data->convertToDsc(), ofs);
  // if(tree) tree->unlock();
  if (STATUS_NOT_OK)
    throw MdsException(status);
}

void TreeNode::putSegmentResampled(Array *data, int ofs,
                                   TreeNode *resampledNode, int resFactor)
{
  resolveNid();
  // if(tree) tree->lock();
  int status =
      putTreeSegmentResampled(tree->getCtx(), getNid(), data->convertToDsc(),
                              ofs, resampledNode->getNid(), resFactor);
  // if(tree) tree->unlock();
  if (STATUS_NOT_OK)
    throw MdsException(status);
}
void TreeNode::putSegmentMinMax(Array *data, int ofs, TreeNode *resampledNode,
                                int resFactor)
{
  resolveNid();
  // if(tree) tree->lock();
  int status =
      putTreeSegmentMinMax(tree->getCtx(), getNid(), data->convertToDsc(), ofs,
                           resampledNode->getNid(), resFactor);
  // if(tree) tree->unlock();
  if (STATUS_NOT_OK)
    throw MdsException(status);
}
/*void TreeNode::putSegmentMinMax(Array *data, int ofs, TreeNode*resampledNode,
int resFactor)
{
        //Resampled aray always converted to float, Assumed 1D array
        int numRows;
        float *arrSamples = data->getFloatArray(&numRows);
        float *resSamples = new float[2* numRows/resFactor + 1]; //It has top
keep minimum and maximum. Ensure enough room even if numRows is not a multiplier
of resFactor for(int i = 0; i < numRows; i+= resFactor)
        {
                float minVal = arrSamples[i];
                float maxVal = minVal;
                for(int j = 0; j < resFactor && i+j < numRows; j++)
                {
                        if(arrSamples[i+j] < minVal)
                                minVal = arrSamples[i+j];
                        if(arrSamples[i+j] > maxVal)
                                maxVal = arrSamples[i+j];
                }
                resSamples[2*i/resFactor] = minVal;
                resSamples[2*i/resFactor+1] = maxVal;
        }
        if(numRows > resFactor) //Avoid writing null sized arrays
        {
            AutoData<Array> resData(new Float32Array(resSamples, 2*
numRows/resFactor)); resampledNode->putSegment(resData, ofs);
        }
        delete[] arrSamples;
        delete[] resSamples;

        putSegment(data, ofs);
}
*/
void TreeNode::updateSegment(Data *start, Data *end, Data *time)
{
  resolveNid();
  // if(tree) tree->lock();
  int status =
      updateTreeSegment(tree->getCtx(), getNid(), -1, start->convertToDsc(),
                        end->convertToDsc(), time->convertToDsc());
  // if(tree) tree->unlock();
  if (STATUS_NOT_OK)
    throw MdsException(status);
}

void TreeNode::updateSegment(int segIdx, Data *start, Data *end, Data *time)
{
  resolveNid();
  // if(tree) tree->lock();
  int status =
      updateTreeSegment(tree->getCtx(), getNid(), segIdx, start->convertToDsc(),
                        end->convertToDsc(), time->convertToDsc());
  // if(tree) tree->unlock();
  if (STATUS_NOT_OK)
    throw MdsException(status);
}

int TreeNode::getNumSegments()
{
  int numSegments;

  resolveNid();
  // if(tree) tree->lock();
  int status = getTreeNumSegments(tree->getCtx(), getNid(), &numSegments);
  // if(tree) tree->unlock();
  if (STATUS_NOT_OK)
    throw MdsException(status);
  return numSegments;
}

void TreeNode::getSegmentInfo(int segIdx, char *dtype, char *dimct, int *dims,
                              int *nextRow)
{
  resolveNid();
  int status = getTreeSegmentInfo(tree->getCtx(), getNid(), segIdx, dtype,
                                  dimct, dims, nextRow);
  if (STATUS_NOT_OK)
    throw MdsException(status);
}

void TreeNode::getSegmentLimits(int segmentIdx, Data **start, Data **end)
{
  void *startDsc, *endDsc;
  resolveNid();
  // if(tree) tree->lock();
  int status = getTreeSegmentLimits(tree->getCtx(), getNid(), segmentIdx,
                                    &startDsc, &endDsc);
  // if(tree) tree->unlock();
  if (STATUS_NOT_OK)
    throw MdsException(status);
  *start = (Data *)convertFromDsc(startDsc, tree);
  freeDsc(startDsc);
  *end = (Data *)convertFromDsc(endDsc, tree);
  freeDsc(endDsc);
}

Array *TreeNode::getSegment(int segIdx)
{
  void *dataDsc;
  void *timeDsc;
  resolveNid();
  // if(tree) tree->lock();
  int status =
      getTreeSegment(tree->getCtx(), getNid(), segIdx, &dataDsc, &timeDsc);
  // if(tree) tree->unlock();
  if (STATUS_NOT_OK)
  {
    freeDsc(dataDsc);
    freeDsc(timeDsc);
    throw MdsException(status);
  }
  Array *retData = (Array *)convertFromDsc(dataDsc, tree);
  freeDsc(dataDsc);
  freeDsc(timeDsc);
  return retData;
}

Data *TreeNode::getSegmentDim(int segIdx)
{
  void *timeDsc;
  resolveNid();
  // if(tree) tree->lock();
  int status = getTreeSegment(tree->getCtx(), getNid(), segIdx, NULL, &timeDsc);
  // if(tree) tree->unlock();
  if (STATUS_NOT_OK)
  {
    freeDsc(timeDsc);
    throw MdsException(status);
  }
  Data *retDim = (Data *)convertFromDsc(timeDsc, tree);
  freeDsc(timeDsc);
  return retDim;
}

void TreeNode::getSegmentAndDimension(int segIdx, Array *&segment,
                                      Data *&dimension)
{
  void *dataDsc;
  void *timeDsc;
  resolveNid();
  // if(tree) tree->lock();
  int status =
      getTreeSegment(tree->getCtx(), getNid(), segIdx, &dataDsc, &timeDsc);
  // if(tree) tree->unlock();
  if (STATUS_NOT_OK)
  {
    freeDsc(dataDsc);
    freeDsc(timeDsc);
    throw MdsException(status);
  }
  segment = (Array *)convertFromDsc(dataDsc, tree);
  dimension = (Data *)convertFromDsc(dataDsc, tree);
  freeDsc(dataDsc);
  freeDsc(timeDsc);
}

Data *TreeNode::getSegmentScale()
{
  void *sclDsc;
  resolveNid();
  // if(tree) tree->lock();
  int status = getTreeSegmentScale(tree->getCtx(), getNid(), &sclDsc);
  // if(tree) tree->unlock();
  if (STATUS_NOT_OK)
  {
    freeDsc(sclDsc);
    sclDsc = NULL;
  }
  Data *retScl = (Data *)convertFromDsc(sclDsc, tree);
  freeDsc(sclDsc);
  return retScl;
}

void TreeNode::setSegmentScale(Data *scale)
{
  resolveNid();
  // if(tree) tree->lock();
  int status =
      setTreeSegmentScale(tree->getCtx(), getNid(), scale->convertToDsc());
  // if(tree) tree->unlock();
  if (STATUS_NOT_OK)
    throw MdsException(status);
}

void TreeNode::beginTimestampedSegment(Array *initData)
{
  resolveNid();
  // if(tree) tree->lock();
  int status = beginTreeTimestampedSegment(tree->getCtx(), getNid(),
                                           initData->convertToDsc());
  // if(tree) tree->unlock();
  if (STATUS_NOT_OK)
    throw MdsException(status);
}

void TreeNode::makeTimestampedSegment(Array *data, int64_t *times)
{
  int numDims;
  int *shape = data->getShape(&numDims);

  resolveNid();
  // if(tree) tree->lock();
  int status = makeTreeTimestampedSegment(
      tree->getCtx(), getNid(), data->convertToDsc(), times, shape[0]);
  // if(tree) tree->unlock();
  deleteNativeArray(shape);
  if (STATUS_NOT_OK)
    throw MdsException(status);
}

void TreeNode::putTimestampedSegment(Array *data, int64_t *times)
{
  resolveNid();
  // if(tree) tree->lock();
  int status = putTreeTimestampedSegment(tree->getCtx(), getNid(),
                                         data->convertToDsc(), times);
  // if(tree) tree->unlock();
  if (STATUS_NOT_OK)
    throw MdsException(status);
}

void TreeNode::putRow(Data *data, int64_t *time, int size)
{
  resolveNid();
  // if(tree) tree->lock();
  int status =
      putTreeRow(tree->getCtx(), getNid(), data->convertToDsc(), time, size);
  // if(tree) tree->unlock();
  if (STATUS_NOT_OK)
    throw MdsException(status);
}

TreeNode *TreeNode::getNode(char const *relPath)
{
  int newNid;
  resolveNid();

  int status = _TreeFindNodeRelative(tree->getCtx(), relPath, nid, &newNid);

  if (STATUS_NOT_OK)
    throw MdsException(status);
  return new TreeNode(newNid, tree);
}

TreeNode *TreeNode::getNode(String *relPathStr)
{
  int newNid;
  resolveNid();
  AutoArray<char> relPath(relPathStr->getString());

  int status = _TreeFindNodeRelative(tree->getCtx(), relPath.ptr, nid, &newNid);

  if (STATUS_NOT_OK)
    throw MdsException(status);
  return new TreeNode(newNid, tree);
}

TreeNodeArray *TreeNode::getNodeWild(char const *path, int usageMask)
{
  int status;
  void *wildCtx = 0;

  std::vector<int> nids;
  nids.reserve(10000);

  int temp = 0;
  while ((status = _TreeFindNodeWildRelative(tree->getCtx(), path, nid, &temp,
                                             &wildCtx, usageMask)) &
         1)
    nids.push_back(temp);
  _TreeFindNodeEnd(tree->getCtx(), &wildCtx);

  TreeNode **retNodes = new TreeNode *[nids.size()];
  for (std::size_t i = 0; i < nids.size(); ++i)
    retNodes[i] = new TreeNode(nids[i], tree);

  TreeNodeArray *nodeArray = new TreeNodeArray(retNodes, nids.size());
  delete[] retNodes;
  return nodeArray;
}

TreeNodeArray *TreeNode::getNodeWild(char const *path)
{
  return getNodeWild(path, -1);
}

TreeNode *TreeNode::addNode(char const *name, char const *usage)
{
  int defNid;
  int newNid;
  resolveNid();
  int status = _TreeGetDefaultNid(tree->getCtx(), &defNid);
  if (STATUS_OK)
    status = _TreeSetDefaultNid(tree->getCtx(), nid);
  if (STATUS_OK)
    status = _TreeAddNode(tree->getCtx(), name, &newNid, convertUsage(usage));
  if (STATUS_OK)
    status = _TreeSetDefaultNid(tree->getCtx(), defNid);
  if (STATUS_NOT_OK)
    throw MdsException(status);
  return new TreeNode(newNid, tree);
}

void TreeNode::remove(char const *name)
{
  int count;
  int defNid;
  resolveNid();
  int status = _TreeGetDefaultNid(tree->getCtx(), &defNid);
  if (STATUS_OK)
    status = _TreeSetDefaultNid(tree->getCtx(), nid);
  AutoPointer<TreeNode> delNode(getNode(name));
  status =
      _TreeDeleteNodeInitialize(tree->getCtx(), delNode.ptr->nid, &count, 1);
  if (STATUS_OK)
    _TreeDeleteNodeExecute(tree->getCtx());
  if (STATUS_OK)
    status = _TreeSetDefaultNid(tree->getCtx(), defNid);
  if (STATUS_NOT_OK)
    throw MdsException(status);
}

TreeNode *TreeNode::addDevice(char const *name, char const *type)
{
  int defNid;
  int newNid;
  resolveNid();
  int status = _TreeGetDefaultNid(tree->getCtx(), &defNid);
  if (STATUS_OK)
    status = _TreeSetDefaultNid(tree->getCtx(), nid);
  if (STATUS_OK)
    status = _TreeAddConglom(tree->getCtx(), name, type, &newNid);
  if (STATUS_OK)
    status = _TreeSetDefaultNid(tree->getCtx(), defNid);
  if (STATUS_NOT_OK)
    throw MdsException(status);
  return new TreeNode(newNid, tree);
}

void TreeNode::rename(std::string const &newName)
{
  resolveNid();
  int status = _TreeRenameNode(tree->getCtx(), nid, newName.c_str());
  if (STATUS_NOT_OK)
    throw MdsException(status);
}

void TreeNode::move(TreeNode *parent, std::string const &newName)
{
  resolveNid();
  std::string parentPath(parent->getFullPathStr());
  rename(parentPath + ":" + newName);
}

void TreeNode::move(TreeNode *parent)
{
  resolveNid();
  std::string name(getNodeNameStr());
  move(parent, name);
}

void TreeNode::addTag(std::string const &tagName)
{
  resolveNid();
  int status = _TreeAddTag(tree->getCtx(), nid, tagName.c_str());
  if (STATUS_NOT_OK)
    throw MdsException(status);
}

void TreeNode::removeTag(std::string const &tagName)
{
  resolveNid();
  int currNid;
  std::string bTag("\\" + tagName);
  int status = _TreeFindNode(tree->getCtx(), bTag.c_str(), &currNid);
  if (currNid != nid)
    throw MdsException("No such tag for this tree node"); // The tag does not
                                                          // refer to this node
  status = _TreeRemoveTag(tree->getCtx(), tagName.c_str());
  if (STATUS_NOT_OK)
    throw MdsException(status);
}

void TreeNode::setSubtree(bool isSubtree)
{
  resolveNid();
  int status;
  if (isSubtree)
    status = _TreeSetSubtree(tree->getCtx(), nid);
  else
    status = _TreeSetNoSubtree(tree->getCtx(), nid);
  if (STATUS_NOT_OK)
    throw MdsException(status);
}

StringArray *TreeNode::findTags()
{
  resolveNid();
  const int MAX_TAGS = 1024;
  char *tagNames[MAX_TAGS];
  void *ctx = 0;
  int nTags = 0;
  while (nTags < MAX_TAGS &&
         (tagNames[nTags] = _TreeFindNodeTags(tree->getCtx(), nid, &ctx)))
    nTags++;
  StringArray *stArr = new StringArray(tagNames, nTags);
  for (int i = 0; i < nTags; i++)
    TreeFree(tagNames[i]);
  return stArr;
}

////////////////////////////////////////////////////////////////////////////////
//  TreePath  //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

TreePath::TreePath(std::string const &path, Tree *tree, Data *units,
                   Data *error, Data *help, Data *validation)
    : TreeNode(0, tree, units, error, help, validation), path(path)
{
  clazz = CLASS_S;
  dtype = DTYPE_PATH;
}

TreePath::TreePath(char const *path, int len, Tree *tree, Data *units,
                   Data *error, Data *help, Data *validation)
    : TreeNode(0, tree, units, error, help, validation), path(path, len)
{
  clazz = CLASS_S;
  dtype = DTYPE_PATH;
}

void TreePath::resolveNid()
{
  if (!tree)
    tree = getActiveTree();
  int status = _TreeFindNode(tree->getCtx(), path.c_str(), &nid);
  if (STATUS_NOT_OK)
    nid = -1;
}

////////////////////////////////////////////////////////////////////////////////
//  TreeNodeArray  /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

TreeNodeArray::TreeNodeArray(TreeNode **nodes, int numNodes)
{
  this->numNodes = numNodes;
  this->nodes = new TreeNode *[numNodes];

  for (int i = 0; i < numNodes; i++)
    this->nodes[i] = nodes[i];
}

TreeNodeArray::TreeNodeArray(int *nids, int numNodes, Tree *tree)
{
  this->numNodes = numNodes;
  this->nodes = new TreeNode *[numNodes];

  for (int i = 0; i < numNodes; i++)
    this->nodes[i] = new TreeNode(nids[i], tree);
}

TreeNodeArray::~TreeNodeArray()
{
  if (numNodes > 0)
  {
    for (int i = 0; i < numNodes; i++)
      deleteData(nodes[i]);
    delete[] nodes;
  }
}

void *TreeNodeArray::operator new(size_t sz) { return ::operator new(sz); }

void TreeNodeArray::operator delete(void *p) { ::operator delete(p); }

StringArray *TreeNodeArray::getPath()
{
  /* WRONG!! AutoArray cannot wok here because AutoArray objects are
  instantiated several times and then go out of scope, this triggering multiple
  deallocation of the same C string and crashing the program

          std::vector<AutoArray<char> > paths;
          for(int i = 0; i < numNodes; ++i)
                  paths.push_back(nodes[i]->getPath());

          std::vector<char *> nativePaths;
          for(std::vector<AutoArray<char> >::iterator i = paths.begin(); i !=
  paths.end(); ++i) nativePaths.push_back(i->get()); return new
  StringArray(&nativePaths[0], numNodes);
  */
  // Use std::string instead   ....not beautiful.....
  std::vector<std::string> paths;
  for (int i = 0; i < numNodes; ++i)
    paths.push_back(std::string(AutoArray<char>(nodes[i]->getPath()).ptr));

  std::vector<char *> nativePaths;
  for (std::vector<std::string>::iterator i = paths.begin(); i != paths.end();
       ++i)
    nativePaths.push_back((char *)i->c_str());
  return new StringArray(&nativePaths[0], numNodes);
}

StringArray *TreeNodeArray::getFullPath()
{
  /* Same as before
          std::vector<AutoArray<char> > paths;
          for(int i = 0; i < numNodes; ++i)
                  paths.push_back(nodes[i]->getFullPath());

          std::vector<char *> nativePaths;
          for(std::vector<AutoArray<char> >::iterator i = paths.begin(); i !=
     paths.end(); ++i) nativePaths.push_back(i->get()); return new
     StringArray(&nativePaths[0], numNodes);
  */

  std::vector<std::string> paths;
  for (int i = 0; i < numNodes; ++i)
    paths.push_back(std::string(AutoArray<char>(nodes[i]->getFullPath()).ptr));

  std::vector<char *> nativePaths;
  for (std::vector<std::string>::iterator i = paths.begin(); i != paths.end();
       ++i)
    nativePaths.push_back((char *)i->c_str());
  return new StringArray(&nativePaths[0], numNodes);
}

Int32Array *TreeNodeArray::getNid()
{
  std::vector<int> nids;
  for (int i = 0; i < numNodes; ++i)
    nids.push_back(nodes[i]->getNid());

  return new Int32Array(&nids[0], numNodes);
}

Int8Array *TreeNodeArray::isOn()
{
  std::vector<char> info;
  for (int i = 0; i < numNodes; ++i)
    info.push_back(nodes[i]->isOn());

  return new Int8Array(&info[0], numNodes);
}

void TreeNodeArray::setOn(Int8Array *info)
{
  std::vector<char> infoArray = dynamic_cast<Data *>(info)->getByteArray();

  // FIXME: numNodes should be size_t
  size_t const MAX = std::min(infoArray.size(), (size_t)numNodes);
  for (std::size_t i = 0; i < MAX; ++i)
    nodes[i]->setOn((infoArray[i]) ? true : false);
}

Int8Array *TreeNodeArray::isSetup()
{
  std::vector<char> info;
  for (int i = 0; i < numNodes; ++i)
    info[i] = nodes[i]->isSetup();

  return new Int8Array(&info[0], numNodes);
}

Int8Array *TreeNodeArray::isWriteOnce()
{
  std::vector<char> info;
  for (int i = 0; i < numNodes; ++i)
    info[i] = nodes[i]->isWriteOnce();

  return new Int8Array(&info[0], numNodes);
}

void TreeNodeArray::setWriteOnce(Int8Array *info)
{
  std::vector<char> infoArray = dynamic_cast<Data *>(info)->getByteArray();

  // FIXME: numNodes should be size_t
  std::size_t const MAX = std::min(infoArray.size(), (size_t)numNodes);
  for (std::size_t i = 0; i < MAX; ++i)
    nodes[i]->setWriteOnce((infoArray[i]) ? true : false);
}
Int8Array *TreeNodeArray::isCompressOnPut()
{
  std::vector<char> info;
  for (int i = 0; i < numNodes; ++i)
    info[i] = nodes[i]->isCompressOnPut();

  return new Int8Array(&info[0], numNodes);
}

void TreeNodeArray::setCompressOnPut(Int8Array *info)
{
  std::vector<char> infoArray = dynamic_cast<Data *>(info)->getByteArray();

  // FIXME: numNodes should be size_t
  std::size_t const MAX = std::min(infoArray.size(), (size_t)numNodes);
  for (std::size_t i = 0; i < MAX; ++i)
    nodes[i]->setCompressOnPut((infoArray[i]) ? true : false);
}

Int8Array *TreeNodeArray::isNoWriteModel()
{
  std::vector<char> info;
  for (int i = 0; i < numNodes; ++i)
    info[i] = nodes[i]->isNoWriteModel();

  return new Int8Array(&info[0], numNodes);
}

void TreeNodeArray::setNoWriteModel(Int8Array *info)
{
  std::vector<char> infoArray = dynamic_cast<Data *>(info)->getByteArray();

  // FIXME: numNodes should be size_t
  std::size_t const MAX = std::min(infoArray.size(), (size_t)numNodes);
  for (std::size_t i = 0; i < MAX; ++i)
    nodes[i]->setNoWriteModel((infoArray[i]) ? true : false);
}

Int8Array *TreeNodeArray::isNoWriteShot()
{
  std::vector<char> info;
  for (int i = 0; i < numNodes; ++i)
    info[i] = nodes[i]->isNoWriteShot();

  return new Int8Array(&info[0], numNodes);
}

void TreeNodeArray::setNoWriteShot(Int8Array *info)
{
  std::vector<char> infoArray = dynamic_cast<Data *>(info)->getByteArray();

  // FIXME: numNodes should be size_t
  std::size_t const MAX = std::min(infoArray.size(), (size_t)numNodes);
  for (std::size_t i = 0; i < MAX; ++i)
    nodes[i]->setNoWriteShot((infoArray[i]) ? true : false);
}

Int32Array *TreeNodeArray::getLength()
{
  std::vector<int> sizes;
  for (int i = 0; i < numNodes; ++i)
    sizes.push_back(nodes[i]->getLength());

  return new Int32Array(&sizes[0], numNodes);
}

Int32Array *TreeNodeArray::getCompressedLength()
{
  std::vector<int> sizes;
  for (int i = 0; i < numNodes; ++i)
    sizes.push_back(nodes[i]->getCompressedLength());

  return new Int32Array(&sizes[0], numNodes);
}

StringArray *TreeNodeArray::getUsage()
{
  std::vector<char const *> usages;
  for (int i = 0; i < numNodes; ++i)
    usages.push_back(nodes[i]->getUsage());

  // FIXME: Change StringArray ctor to handle const
  return new StringArray(const_cast<char **>(&usages[0]), numNodes);
}

////////////////End TreeNodeArray methods/////////////////////
////////////////TreeNodeThinClient methods ///////////////////

static const char *convertNciItm(int nciItm)
{
  switch (nciItm)
  {
  case NciTIME_INSERTED:
    return "\'TIME_INSERTED\'";
  case NciOWNER_ID:
    return "\'OWNER_ID\'";
  case NciCLASS:
    return "\'CLASS\'";
  case NciDTYPE:
    return "\'DTYPE\'";
  case NciLENGTH:
    return "\'LENGTH\'";
  case NciSTATUS:
    return "\'STATUS\'";
  case NciCONGLOMERATE_ELT:
    return "\'CONGLOMERATE_ELT\'";
  case NciGET_FLAGS:
    return "\'GET_FLAGS\'";
  case NciNODE_NAME:
    return "\'NODE_NAME\'";
  case NciPATH:
    return "\'PATH\'";
  case NciDEPTH:
    return "\'DEPTH\'";
  case NciPARENT:
    return "\'PARENT\'";
  case NciBROTHER:
    return "\'BROTHER\'";
  case NciMEMBER:
    return "\'MEMBER\'";
  case NciCHILD:
    return "\'CHILD\'";
  case NciPARENT_RELATIONSHIP:
    return "\'PARENT_RELATIONSHIP\'";
  case NciK_IS_CHILD:
    return "\'IS_CHILD\'";
  case NciK_IS_MEMBER:
    return "\'IS_MEMBER\'";
  case NciCONGLOMERATE_NIDS:
    return "\'CONGLOMERATE_NIDS\'";
  case NciORIGINAL_PART_NAME:
    return "\'ORIGINAL_PART_NAME\'";
  case NciNUMBER_OF_MEMBERS:
    return "\'NUMBER_OF_MEMBERS\'";
  case NciNUMBER_OF_CHILDREN:
    return "\'NUMBER_OF_CHILDREN\'";
  case NciMEMBER_NIDS:
    return "\'MEMBER_NIDS\'";
  case NciCHILDREN_NIDS:
    return "\'CHILDREN_NIDS\'";
  case NciFULLPATH:
    return "\'FULLPATH\'";
  case NciMINPATH:
    return "\'MINPATH\'";
  case NciUSAGE:
    return "\'USAGE\'";
  default:
    throw MdsException("Unexpected NCI item in TreeNodeThinClient");
  }
}

TreeNodeThinClient::TreeNodeThinClient(int nid, Connection *connection,
                                       Data *units, Data *error, Data *help,
                                       Data *validation)
{
  this->tree = 0;
  this->nid = nid;
  this->connection = connection;
  clazz = CLASS_S;
  dtype = DTYPE_NID;
  setAccessory(units, error, help, validation);
}

std::string TreeNodeThinClient::getNciString(int itm)
{
  char expr[64];
  sprintf(expr, "GETNCI(%d,%s)", nid, convertNciItm(itm));
  AutoData<Data> retStringData(connection->get(expr));
  if (!retStringData.get())
    throw MdsException("Error in Remote evaluation of getnci");

  AutoString as(retStringData->getString());
  return as.string;
}

char TreeNodeThinClient::getNciChar(int itm)
{
  char expr[64];
  sprintf(expr, "GETNCI(%d,%s)", nid, convertNciItm(itm));
  AutoData<Data> retData(connection->get(expr));
  if (!retData.get())
    throw MdsException("Error in Remote evaluation of getnci");
  char retChar = retData->getByte();
  return retChar;
}

int TreeNodeThinClient::getNciInt(int itm)
{
  char expr[64];
  sprintf(expr, "GETNCI(%d,%s)", nid, convertNciItm(itm));
  AutoData<Data> retData(connection->get(expr));
  if (!retData.get())
    throw MdsException("Error in Remote evaluation of getnci");
  return retData->getInt();
}

int64_t TreeNodeThinClient::getNciInt64(int itm)
{
  char expr[64];
  sprintf(expr, "GETNCI(%d,%s)", nid, convertNciItm(itm));
  AutoData<Data> retData(connection->get(expr));
  if (!retData.get())
    throw MdsException("Error in Remote evaluation of getnci");
  return retData->getLong();
}

char *TreeNodeThinClient::getPath()
{
  char expr[64];
  sprintf(expr, "GETNCI(%d,\'PATH\')", nid);
  AutoData<Data> retData(connection->get(expr));
  if (!retData.get())
    throw MdsException("Error in Remote evaluation of getnci(path)");
  return retData->getString();
}

EXPORT Data *TreeNodeThinClient::getData()
{
  char expr[64];
  sprintf(expr, "GETNCI(%d,\'RECORD\')", nid);
  Data *retData = connection->get(expr);
  if (!retData)
    throw MdsException("Error in Remote evaluation of getnci(record)");
  return retData;
}

EXPORT void TreeNodeThinClient::putData(Data *data)
{
  AutoArray<char> path(getPath());
  AutoData<Data> argsD[] = {data->data()};
  Data *args[] = {argsD[0].get()};
  connection->put(path, (char *)"$", args, 1);
}

EXPORT void TreeNodeThinClient::deleteData()
{
  AutoArray<char> path = getFullPath();
  Data *args[1] = {0};
  connection->put(path, (char *)"*", args, 0);
}

EXPORT bool TreeNodeThinClient::isOn()
{
  char expr[64];
  sprintf(expr, "GETNCI(%d,\'STATE\')", nid);
  AutoData<Data> retData(connection->get(expr));
  if (!retData.get())
    throw MdsException("Error in Remote evaluation of getnci(state)");

  char onData = retData->getByte();
  return (onData) ? false : true;
}

EXPORT void TreeNodeThinClient::setOn(bool on)
{
  char expr[64];
  if (on)
    sprintf(expr, "TreeTurnOn(%d)", nid);
  else
    sprintf(expr, "TreeTurnOff(%d)", nid);
  AutoData<Data> retData(connection->get(expr));
}

EXPORT void TreeNodeThinClient::beginSegment(Data *start, Data *end, Data *time,
                                             Array *initialData)
{
  char expr[256];
  AutoData<Data> argsD[] = {start->data(), end->data(), time->data(),
                            initialData->data()};
  Data *args[] = {argsD[0].get(), argsD[1].get(), argsD[2].get(),
                  argsD[3].get()};
  sprintf(expr, "BeginSegment(%d, $1, $2, $3, $4, -1)", nid);
  AutoData<Data> retData(connection->get(expr, args, 4));
}

EXPORT void TreeNodeThinClient::makeSegment(Data *start, Data *end, Data *time,
                                            Array *initialData)
{
  char expr[256];
  AutoData<Data> argsD[] = {start->data(), end->data(), time->data(),
                            initialData->data()};
  Data *args[] = {argsD[0].get(), argsD[1].get(), argsD[2].get(),
                  argsD[3].get()};
  sprintf(expr, "MakeSegment(%d, $1, $2, $3, $4, -1, size($4))", nid);
  AutoData<Data> retData(connection->get(expr, args, 4));
}

EXPORT void TreeNodeThinClient::putSegment(Array *data, int ofs)
{
  char expr[256];
  AutoData<Data> argsD[] = {data->data()};
  Data *args[] = {argsD[0].get()};
  sprintf(expr, "PutSegment(%d, %d, $1)", nid, ofs);
  AutoData<Data> retData(connection->get(expr, args, 1));
}

EXPORT int TreeNodeThinClient::getNumSegments()
{
  char expr[64];
  sprintf(expr, "GetNumSegments(%d)", nid);
  AutoData<Data> data(connection->get(expr));
  if (!data.get())
    throw MdsException("Error in Remote evaluation of GetNumSegmentss");
  return data->getInt();
}

EXPORT void TreeNodeThinClient::getSegmentLimits(int segmentIdx, Data **start,
                                                 Data **end)
{
  char expr[64];
  sprintf(expr, "GetSegmentLimits(%d, %d)", nid, segmentIdx);
  AutoData<Array> limitsArr((Array *)connection->get(expr));
  if (!limitsArr)
    throw MdsException("Error in Remote evaluation of GetSegmentLimits");
  int startIdx = 0, endIdx = 1;
  *start = limitsArr->getElementAt(&startIdx, 1);
  *end = limitsArr->getElementAt(&endIdx, 1);
}

EXPORT Array *TreeNodeThinClient::getSegment(int segIdx)
{
  char expr[64];
  sprintf(expr, "GetSegment(%d, %d)", nid, segIdx);
  Array *retSegment = (Array *)connection->get(expr);
  if (!retSegment)
    throw MdsException("Error in Remote evaluation of GetSegment");
  return retSegment;
}

EXPORT Data *TreeNodeThinClient::getSegmentDim(int segIdx)
{
  char expr[64];
  sprintf(expr, "DIM_OF(GetSegment(%d, %d))", nid, segIdx);
  Data *retDim = connection->get(expr);
  if (!retDim)
    throw MdsException("Error in Remote evaluation of dim_of(GetSegment)");
  return retDim;
}

EXPORT void TreeNodeThinClient::getSegmentAndDimension(int segIdx,
                                                       Array *&segment,
                                                       Data *&dimension)
{
  char expr[64];
  sprintf(expr, "GetSegment(%d, %d)", nid, segIdx);
  segment = (Array *)connection->get(expr);
  sprintf(expr, "DIM_OF(GetSegment(%d, %d))", nid, segIdx);
  dimension = connection->get(expr);
  if (!segment || !dimension)
    throw MdsException("Error in Remote evaluation of GetSegment");
}

EXPORT void TreeNodeThinClient::beginTimestampedSegment(Array *initData)
{
  char expr[64];
  AutoData<Data> argsD[] = {initData->data()};
  Data *args[] = {argsD[0].get()};
  sprintf(expr, "BeginTimestampedSegment(%d, $1, -1)", nid);
  AutoData<Data> retData(connection->get(expr, args, 1));
}

EXPORT void TreeNodeThinClient::putTimestampedSegment(Array *data,
                                                      int64_t *times)
{
  char expr[64];
  int len = data->getSize();
  AutoData<Data> argsD[] = {new Int64Array(times, len), data->data()};
  Data *args[] = {argsD[0].get(), argsD[1].get()};
  sprintf(expr, "PutTimestampedSegment(%d, $1, $2)", nid);
  AutoData<Data> retData(connection->get(expr, args, 2));
}

EXPORT void TreeNodeThinClient::makeTimestampedSegment(Array *data,
                                                       int64_t *times)
{
  beginTimestampedSegment(data);
  putTimestampedSegment(data, times);
}

EXPORT void TreeNodeThinClient::putRow(Data *data, int64_t *time, int size)
{
  AutoData<Data> argsD[] = {new Int64(*time), data->data()};
  Data *args[] = {argsD[0].get(), argsD[1].get()};
  char expr[64];
  sprintf(expr, "PutRow(%d, %d, $1, $2)", nid, size);
  AutoData<Data> retData(connection->get(expr, args, 2));
}

EXPORT StringArray *TreeNodeThinClient::findTags()
{
  char expr[64];
  sprintf(expr, "TreeFindNodeTags(%d)", nid);
  AutoData<Data> retData(connection->get(expr));
  if (!retData.get())
    throw MdsException("Error in Remote evaluation of TreeFindNodeTags");

  int numTags;
  char **tags = retData->getStringArray(&numTags);
  StringArray *tagsArray = new StringArray(tags, numTags);
  for (int i = 0; i < numTags; i++)
    delete[] tags[i];
  delete[] tags;
  return tagsArray;
}
//////////////////////////////////////////////////////////////

void MDSplus::setActiveTree(Tree *tree) { TreeSwitchDbid(tree->getCtx()); }

Tree *MDSplus::getActiveTree()
{
  char name[1024];
  int shot;
  int retNameLen, retShotLen;
  DBI_ITM dbiItems[] = {{1024, DbiNAME, name, &retNameLen},
                        {sizeof(int), DbiSHOTID, &shot, &retShotLen},
                        {0, DbiEND_OF_LIST, 0, 0}};
  int status = TreeGetDbi(dbiItems);
  if (STATUS_NOT_OK)
  {
    throw MdsException(status);
  }
  return new Tree(name, shot, TreeDbid());
}

ostream &operator<<(ostream &stream, TreeNode *treeNode)
{
  return stream << treeNode->getPathStr();
}
