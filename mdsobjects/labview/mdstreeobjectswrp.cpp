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
#include "mdsobjectswrp.h"

#include <cstring>

using namespace MDSplus;
using namespace std;
/********************************************************************************************************
												TREE
 ********************************************************************************************************/

EXPORT void mdsplus_tree_addDevice(const void *lvTreePtr, void **lvTreeNodePtrOut, const char *nameIn, const char *typeIn, ErrorCluster *error)
{
	Tree *treePtr = NULL;
	TreeNode *treeNodePtrOut = NULL;
	MgErr errorCode = noErr;
	char const * errorSource = __func__;
	char const * errorMessage = (char *)"";
	try {
		treePtr = reinterpret_cast<Tree *>(const_cast<void *>(lvTreePtr));
		treeNodePtrOut = treePtr->addDevice(const_cast<char *>(nameIn), const_cast<char *>(typeIn));
		*lvTreeNodePtrOut = reinterpret_cast<void *>(treeNodePtrOut);
	} catch (const MdsException &e) {
		errorCode = bogusError;
		errorMessage = e.what();
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_tree_addNode(const void *lvTreePtr, void **lvTreeNodePtrOut, const char *nameIn, const char *usageIn, ErrorCluster *error)
{
	Tree *treePtr = NULL;
	TreeNode *treeNodePtrOut = NULL;
	MgErr errorCode = noErr;
	char const * errorSource = __func__;
	char const * errorMessage = (char *)"";
	try {
		treePtr = reinterpret_cast<Tree *>(const_cast<void *>(lvTreePtr));
		treeNodePtrOut = treePtr->addNode(const_cast<char *>(nameIn), const_cast<char *>(usageIn));
		*lvTreeNodePtrOut = reinterpret_cast<void *>(treeNodePtrOut);
	} catch (const MdsException &e) {
		errorCode = bogusError;
		errorMessage = e.what();
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_tree_constructor(void **lvTreePtrOut, const char *nameIn, int shotIn, ErrorCluster *error)
{
	Tree *treePtrOut = NULL;
	MgErr errorCode = noErr;
	char const * errorSource = __func__;
	char const * errorMessage = (char *)"";
	try {
		treePtrOut = new Tree(const_cast<char *>(nameIn), shotIn);
		*lvTreePtrOut = reinterpret_cast<void *>(treePtrOut);
	} catch (const MdsException &e) {
		errorCode = bogusError;
		errorMessage = e.what();
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		*lvTreePtrOut = 0;
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_tree_constructor_mode(void **lvTreePtrOut, const char *nameIn, int shotIn, const char *modeIn, ErrorCluster *error)
{
	Tree *treePtrOut = NULL;
	MgErr errorCode = noErr;
	char const * errorSource = __func__;
	char const * errorMessage = (char *)"";
	try {
		treePtrOut = new Tree(const_cast<char *>(nameIn), shotIn, const_cast<char *>(modeIn));
		*lvTreePtrOut = reinterpret_cast<void *>(treePtrOut);
	} catch (const MdsException &e) {
		errorCode = bogusError;
		errorMessage = e.what();
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		*lvTreePtrOut = 0;
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_tree_createPulse(const void *lvTreePtr, int shotIn, ErrorCluster *error)
{
	Tree *treePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";

	try
	{
		treePtr = reinterpret_cast<Tree *>(const_cast<void *>(lvTreePtr));
		treePtr->createPulse(shotIn);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_tree_deletePulse(const void *lvTreePtr, int shotIn, ErrorCluster *error)
{
	Tree *treePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";
	try
	{
		treePtr = reinterpret_cast<Tree *>(const_cast<void *>(lvTreePtr));
		treePtr->deletePulse(shotIn);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_tree_destructor(void **lvTreePtr)
{
	Tree *treePtr = reinterpret_cast<Tree *>(*lvTreePtr);
	delete treePtr;
	*lvTreePtr = NULL;
}

EXPORT void mdsplus_tree_edit(const void *lvTreePtr, ErrorCluster *error)
{
	Tree *treePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;

	char const * errorMessage = "";
	try
	{
		treePtr = reinterpret_cast<Tree *>(const_cast<void *>(lvTreePtr));
		treePtr->edit();
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_tree_findTags(const void *lvTreePtr, void **lvStringArrayPtrOut, const char *wildIn, ErrorCluster *error)
{
	Tree *treePtr = NULL;
	StringArray *stringArrayPtrOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treePtr = reinterpret_cast<Tree *>(const_cast<void *>(lvTreePtr));
		stringArrayPtrOut = treePtr->findTags(const_cast<char *>(wildIn));
		*lvStringArrayPtrOut = reinterpret_cast<void *>(stringArrayPtrOut);
	}
	catch (const MdsException &e)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(e.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_tree_getActiveTree(void **lvTreePtrOut, ErrorCluster *error)
{
	Tree *treePtrOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treePtrOut = getActiveTree();
		*lvTreePtrOut = reinterpret_cast<void *>(treePtrOut);
	}
	catch (const MdsException &mdsE)
	{
		delete treePtrOut;
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_tree_getCurrent(int *currentOut, const char *treeNameIn, ErrorCluster *error)
{
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		*currentOut = Tree::getCurrent(const_cast<char *>(treeNameIn));
	}
	catch (const MdsException &e)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(e.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_tree_getDatafileSize(const void *lvTreePtr, int64_t *sizeOut, ErrorCluster *error)
{
	Tree *treePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treePtr = reinterpret_cast<Tree *>(const_cast<void *>(lvTreePtr));
		*sizeOut = treePtr->getDatafileSize();
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_tree_getDefault(const void *lvTreePtr, void **lvTreeNodePtrOut, ErrorCluster *error)
{
	Tree *treePtr = NULL;
	TreeNode *treeNodePtrOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treePtr = reinterpret_cast<Tree *>(const_cast<void *>(lvTreePtr));
		treeNodePtrOut = treePtr->getDefault();
		*lvTreeNodePtrOut = reinterpret_cast<void *>(treeNodePtrOut);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_tree_getNode(const void *lvTreePtr, void **lvTreeNodePtrOut, const char *pathIn, ErrorCluster *error)
{
	Tree *treePtr = NULL;
	TreeNode *treeNodePtrOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treePtr = reinterpret_cast<Tree *>(const_cast<void *>(lvTreePtr));
		treeNodePtrOut = treePtr->getNode(const_cast<char *>(pathIn));
		*lvTreeNodePtrOut = reinterpret_cast<void *>(treeNodePtrOut);
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		*lvTreeNodePtrOut = 0;
		fillErrorCluster(errorCode, errorSource, const_cast<char *>(mdsE.what()), error);
	}
}

EXPORT void mdsplus_tree_hasNode(const void *lvTreePtr, LVBoolean *lvhasNodeOut, const char *pathIn, ErrorCluster *error)
{
	Tree *treePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = (char *)"";
	*lvhasNodeOut = LVBooleanTrue;
	try
	{
		treePtr = reinterpret_cast<Tree *>(const_cast<void *>(lvTreePtr));
		TreeNode *node = treePtr->getNode(const_cast<char *>(pathIn));
		delete node;

	}
	catch (...)
	{
		*lvhasNodeOut = LVBooleanFalse;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}


EXPORT void mdsplus_tree_getNode_string(const void *lvTreePtr, void **lvTreeNodePtrOut, const void *lvStringPtrIn, ErrorCluster *error)
{
	Tree *treePtr = NULL;
	TreeNode *treeNodePtrOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treePtr = reinterpret_cast<Tree *>(const_cast<void *>(lvTreePtr));
		String *stringPtrIn = reinterpret_cast<String *>(const_cast<void *>(lvStringPtrIn));
		treeNodePtrOut = treePtr->getNode(stringPtrIn);
		*lvTreeNodePtrOut = reinterpret_cast<void *>(treeNodePtrOut);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_tree_getNode_treepath(const void *lvTreePtr, void **lvTreeNodePtrOut, const void *lvTreePathPtrIn, ErrorCluster *error)
{
	Tree *treePtr = NULL;
	TreeNode *treeNodePtrOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treePtr = reinterpret_cast<Tree *>(const_cast<void *>(lvTreePtr));
		TreePath *treePathPtrIn = reinterpret_cast<TreePath *>(const_cast<void *>(lvTreePathPtrIn));
		treeNodePtrOut = treePtr->getNode(treePathPtrIn);
		*lvTreeNodePtrOut = reinterpret_cast<void *>(treeNodePtrOut);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_tree_getNodeWild(const void *lvTreePtr, void **lvTreeNodeArrayPtrOut, const char *pathIn, ErrorCluster *error)
{
	Tree *treePtr = NULL;
	TreeNodeArray *treeNodeArrayPtrOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treePtr = reinterpret_cast<Tree *>(const_cast<void *>(lvTreePtr));
		treeNodeArrayPtrOut = treePtr->getNodeWild(const_cast<char *>(pathIn));
		*lvTreeNodeArrayPtrOut = reinterpret_cast<void *>(treeNodeArrayPtrOut);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_tree_getNodeWild_usageMask(const void *lvTreePtr, void **lvTreeNodeArrayPtrOut, const char *pathIn, int usageMaskIn, ErrorCluster *error)
{
	Tree *treePtr = NULL;
	TreeNodeArray *treeNodeArrayPtrOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treePtr = reinterpret_cast<Tree *>(const_cast<void *>(lvTreePtr));
		treeNodeArrayPtrOut = treePtr->getNodeWild(const_cast<char *>(pathIn), usageMaskIn);
		*lvTreeNodeArrayPtrOut = reinterpret_cast<void *>(treeNodeArrayPtrOut);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_tree_isModified(const void *lvTreePtr, LVBoolean *lvIsModifiedOut, ErrorCluster *error)
{
	Tree *treePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treePtr = reinterpret_cast<Tree *>(const_cast<void *>(lvTreePtr));
		*lvIsModifiedOut = (treePtr->isModified() == true) ? LVBooleanTrue : LVBooleanFalse;
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_tree_isOpenForEdit(const void *lvTreePtr, LVBoolean *lvIsOpenForEditOut, ErrorCluster *error)
{
	Tree *treePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treePtr = reinterpret_cast<Tree *>(const_cast<void *>(lvTreePtr));
		*lvIsOpenForEditOut = (treePtr->isOpenForEdit() == true) ? LVBooleanTrue : LVBooleanFalse;
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_tree_isReadOnly(const void *lvTreePtr, LVBoolean *lvIsReadOnlyOut, ErrorCluster *error)
{
	Tree *treePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treePtr = reinterpret_cast<Tree *>(const_cast<void *>(lvTreePtr));
		*lvIsReadOnlyOut = (treePtr->isReadOnly() == true) ? LVBooleanTrue : LVBooleanFalse;
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_tree_quit(const void *lvTreePtr, ErrorCluster *error)
{
	Tree *treePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treePtr = reinterpret_cast<Tree *>(const_cast<void *>(lvTreePtr));
		treePtr->quit();
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_tree_remove(const void *lvTreePtr, const char *nameIn, ErrorCluster *error)
{
	Tree *treePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treePtr = reinterpret_cast<Tree *>(const_cast<void *>(lvTreePtr));
		treePtr->remove(const_cast<char *>(nameIn));
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_tree_removeTag(const void *lvTreePtr, const char *tagNameIn, ErrorCluster *error)
{
	Tree *treePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treePtr = reinterpret_cast<Tree *>(const_cast<void *>(lvTreePtr));
		treePtr->removeTag(const_cast<char *>(tagNameIn));
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_tree_setActiveTree(const void *lvTreePtrIn, ErrorCluster *error)
{
	Tree *treePtrIn = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treePtrIn = reinterpret_cast<Tree *>(const_cast<void *>(lvTreePtrIn));
		setActiveTree(treePtrIn);
	}
	catch (const MdsException &e)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(e.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_tree_setCurrent(const char *treeNameIn, int shotIn, ErrorCluster *error)
{
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		Tree::setCurrent(const_cast<char *>(treeNameIn), shotIn);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_tree_setDefault(const void *lvTreePtr, const void *lvTreeNodePtrIn, ErrorCluster *error)
{
	Tree *treePtr = NULL;
	TreeNode *treeNodePtrIn = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treePtr = reinterpret_cast<Tree *>(const_cast<void *>(lvTreePtr));
		treeNodePtrIn = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtrIn));
		treePtr->setDefault(treeNodePtrIn);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_tree_setTimeContext(const void *lvTreePtr, const void *lvStartDataPtrIn, const void *lvEndDataPtrIn, const void *lvDeltaDataPtrIn, ErrorCluster *error)
{
	Tree *treePtr = NULL;
	Data *startDataPtrIn = NULL;
	Data *endDataPtrIn = NULL;
	Data *deltaDataPtrIn = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treePtr = reinterpret_cast<Tree *>(const_cast<void *>(lvTreePtr));
		startDataPtrIn = reinterpret_cast<Data *>(const_cast<void *>(lvStartDataPtrIn));
		endDataPtrIn = reinterpret_cast<Data *>(const_cast<void *>(lvEndDataPtrIn));
		deltaDataPtrIn = reinterpret_cast<Data *>(const_cast<void *>(lvDeltaDataPtrIn));
		treePtr->setTimeContext(startDataPtrIn, endDataPtrIn, deltaDataPtrIn);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_tree_setVersionsInModel(const void *lvTreePtr, LVBoolean *lvVerEnabledIn, ErrorCluster *error)
{
	Tree *treePtr = NULL;
	bool verEnabledIn;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treePtr = reinterpret_cast<Tree *>(const_cast<void *>(lvTreePtr));
		verEnabledIn = (*lvVerEnabledIn == LVBooleanTrue) ? true : false;
		treePtr->setVersionsInModel(verEnabledIn);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_tree_setVersionsInPulse(const void *lvTreePtr, LVBoolean *lvVerEnabledIn, ErrorCluster *error)
{
	Tree *treePtr = NULL;
	bool verEnabledIn;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treePtr = reinterpret_cast<Tree *>(const_cast<void *>(lvTreePtr));
		verEnabledIn = (*lvVerEnabledIn == LVBooleanTrue) ? true : false;
		treePtr->setVersionsInPulse(verEnabledIn);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_tree_setViewDate(const void *lvTreePtr, const char *dateIn, ErrorCluster *error)
{
	Tree *treePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treePtr = reinterpret_cast<Tree *>(const_cast<void *>(lvTreePtr));
		treePtr->setViewDate(const_cast<char *>(dateIn));
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_tree_versionsInModelEnabled(const void *lvTreePtr, LVBoolean *lvVerEnabledOut, ErrorCluster *error)
{
	Tree *treePtr = NULL;
	bool verEnabledOut;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treePtr = reinterpret_cast<Tree *>(const_cast<void *>(lvTreePtr));
		verEnabledOut = treePtr->versionsInModelEnabled();
		*lvVerEnabledOut = (verEnabledOut == true) ? LVBooleanTrue : LVBooleanFalse;
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_tree_versionsInPulseEnabled(const void *lvTreePtr, LVBoolean *lvVerEnabledOut, ErrorCluster *error)
{
	Tree *treePtr = NULL;
	bool verEnabledOut;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treePtr = reinterpret_cast<Tree *>(const_cast<void *>(lvTreePtr));
		verEnabledOut = treePtr->versionsInPulseEnabled();
		*lvVerEnabledOut = (verEnabledOut == true) ? LVBooleanTrue : LVBooleanFalse;
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_tree_write(const void *lvTreePtr, ErrorCluster *error)
{
	Tree *treePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treePtr = reinterpret_cast<Tree *>(const_cast<void *>(lvTreePtr));
		treePtr->write();
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

/********************************************************************************************************
												TREENODE
 ********************************************************************************************************/

EXPORT void mdsplus_treenode_addDevice(const void *lvTreeNodePtr, void **lvTreeNodePtrOut, const char *nameIn, const char *typeIn, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	TreeNode *treeNodePtrOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		treeNodePtrOut = treeNodePtr->addDevice(const_cast<char *>(nameIn), const_cast<char *>(typeIn));
		*lvTreeNodePtrOut = reinterpret_cast<void *>(treeNodePtrOut);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_addNode(const void *lvTreeNodePtr, void **lvTreeNodePtrOut, const char *nameIn, const char *usageIn, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	TreeNode *treeNodePtrOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		treeNodePtrOut = treeNodePtr->addNode(const_cast<char *>(nameIn), const_cast<char *>(usageIn));
		*lvTreeNodePtrOut = reinterpret_cast<void *>(treeNodePtrOut);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_addTag(const void *lvTreeNodePtr, const char *tagNameIn, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		treeNodePtr->addTag(const_cast<char *>(tagNameIn));
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_beginSegment(const void *lvTreeNodePtr, const void *lvStartDataPtrIn, const void *lvEndDataPtrIn, const void *lvTimeDataPtrIn, const void *lvArrayPtrIn, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	Data *startDataPtrIn = NULL;
	Data *endDataPtrIn = NULL;
	Data *timeDataPtrIn = NULL;
	Array *initialDataArrayPtrIn = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		startDataPtrIn = reinterpret_cast<Data *>(const_cast<void *>(lvStartDataPtrIn));
		endDataPtrIn = reinterpret_cast<Data *>(const_cast<void *>(lvEndDataPtrIn));
		timeDataPtrIn = reinterpret_cast<Data *>(const_cast<void *>(lvTimeDataPtrIn));
		initialDataArrayPtrIn = reinterpret_cast<Array *>(const_cast<void *>(lvArrayPtrIn));
		treeNodePtr->beginSegment(startDataPtrIn, endDataPtrIn, timeDataPtrIn, initialDataArrayPtrIn);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_beginTimestampedSegment(const void *lvTreeNodePtr, const void *lvArrayPtrIn, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	Array *initDataArrayPtrIn = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		initDataArrayPtrIn = reinterpret_cast<Array *>(const_cast<void *>(lvArrayPtrIn));
		treeNodePtr->beginTimestampedSegment(initDataArrayPtrIn);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_containsVersions(const void *lvTreeNodePtr, LVBoolean *lvContainsVersionsOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		*lvContainsVersionsOut = (treeNodePtr->containsVersions() == true) ? LVBooleanTrue : LVBooleanFalse;
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_deleteData(const void *lvTreeNodePtr, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		treeNodePtr->deleteData();
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_doMethod(const void *lvTreeNodePtr, const char *methodIn, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		treeNodePtr->doMethod(const_cast<char *>(methodIn));
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_destructor(void **lvTreeNodePtr)
{
	TreeNode *treeNodePtr = reinterpret_cast<TreeNode *>(*lvTreeNodePtr);
	delete treeNodePtr;
	*lvTreeNodePtr = NULL;
}

EXPORT void mdsplus_treenode_findTags(const void *lvTreeNodePtr, void **lvStringArrayPtrOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	StringArray *stringArrayPtrOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		stringArrayPtrOut = treeNodePtr->findTags();
		*lvStringArrayPtrOut = reinterpret_cast<void *>(stringArrayPtrOut);
	}
	catch (const MdsException &e)
	{
		delete stringArrayPtrOut;
		errorCode = bogusError;
		errorMessage = const_cast<char *>(e.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getBrother(const void *lvTreeNodePtr, void **lvTreeNodePtrOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	TreeNode *treeNodePtrOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		treeNodePtrOut = treeNodePtr->getBrother();
		*lvTreeNodePtrOut = reinterpret_cast<void *>(treeNodePtrOut);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getChild(const void *lvTreeNodePtr, void **lvTreeNodePtrOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	TreeNode *treeNodePtrOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		treeNodePtrOut = treeNodePtr->getChild();
		*lvTreeNodePtrOut = reinterpret_cast<void *>(treeNodePtrOut);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getChildren(const void *lvTreeNodePtr, LPtrArrHdl lvPtrArrHdlOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	TreeNode **childrenArrOut;
	int childrenArrLen = 0;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		childrenArrOut = treeNodePtr->getChildren(&childrenArrLen);
		// checks whether the size of a pointer is 32 bit or 64 bit depending upon the system architecture
		int32 typeCode = (sizeof(void *) > sizeof(int32)) ? iQ : iL;
		// resizes treenode array
		errorCode = NumericArrayResize(typeCode, 1, reinterpret_cast<UHandle *>(&lvPtrArrHdlOut), static_cast<int32>(childrenArrLen));
		if (!errorCode)
		{
			for (int i = 0; i < childrenArrLen; i++)
				(*lvPtrArrHdlOut)->elt[i] = static_cast<void *>(childrenArrOut[i]);
			(*lvPtrArrHdlOut)->dimSize = static_cast<int32>(childrenArrLen);
		}
		else
			errorMessage = (char *)"NumericArrayResize error";
		deleteNativeArray(reinterpret_cast<Data **>(childrenArrOut));
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getClass(const void *lvTreeNodePtr, LStrHandle lvStrHdlOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	char *strOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		strOut = const_cast<char *>(treeNodePtr->getClass());
		std::size_t strOutLen = std::strlen(strOut);
		errorCode = NumericArrayResize(uB, 1, reinterpret_cast<UHandle *>(&lvStrHdlOut), strOutLen + sizeof(int32));
		if (!errorCode)
		{
			MoveBlock(reinterpret_cast<uChar *>(strOut), LStrBuf(*lvStrHdlOut), strOutLen);
			(*lvStrHdlOut)->cnt = strOutLen;
		}
		else
			errorMessage = (char *)"NumericArrayResize error";
		//deleteNativeArray(strOut);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getCompressedLength(const void *lvTreeNodePtr, int *compressedLengthOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		*compressedLengthOut = treeNodePtr->getCompressedLength();
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getConglomerateElt(const void *lvTreeNodePtr, int *conglomerateEltOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		*conglomerateEltOut = treeNodePtr->getConglomerateElt();
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getConglomerateNodes(const void *lvTreeNodePtr, void **lvTreeNodeArrayPtrOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	TreeNodeArray *treeNodeArrayPtrOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		treeNodeArrayPtrOut = treeNodePtr->getConglomerateNodes();
		*lvTreeNodeArrayPtrOut = reinterpret_cast<void *>(treeNodeArrayPtrOut);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getDepth(const void *lvTreeNodePtr, int *depthOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		*depthOut = treeNodePtr->getDepth();
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getData(const void *lvTreeNodePtr, void **lvDataPtrOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	Data *dataPtrOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		dataPtrOut = treeNodePtr->getData();
		*lvDataPtrOut = reinterpret_cast<void *>(dataPtrOut);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getDescendants(const void *lvTreeNodePtr, LPtrArrHdl lvPtrArrHdlOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	TreeNode **descendantsArrOut;
	int descendantsArrLen = 0;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		descendantsArrOut = treeNodePtr->getDescendants(&descendantsArrLen);
		// checks whether the size of a pointer is 32 bit or 64 bit depending upon the system architecture
		int32 typeCode = (sizeof(void *) > sizeof(int32)) ? iQ : iL;
		// resizes treenode array
		errorCode = NumericArrayResize(typeCode, 1, reinterpret_cast<UHandle *>(&lvPtrArrHdlOut), static_cast<int32>(descendantsArrLen));
		if (!errorCode)
		{
			for (int i = 0; i < descendantsArrLen; i++)
				(*lvPtrArrHdlOut)->elt[i] = static_cast<void *>(descendantsArrOut[i]);
			(*lvPtrArrHdlOut)->dimSize = static_cast<int32>(descendantsArrLen);
		}
		else
			errorMessage = (char *)"NumericArrayResize error";
		deleteNativeArray(reinterpret_cast<Data **>(descendantsArrOut));
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getDType(const void *lvTreeNodePtr, LStrHandle lvStrHdlOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	char *strOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		strOut = const_cast<char *>(treeNodePtr->getDType());
		std::size_t strOutLen = std::strlen(strOut);
//		errorCode = NumericArrayResize(uB, 1, reinterpret_cast<UHandle *>(&lvStrHdlOut), strOutLen);
		errorCode = NumericArrayResize(uB, 1, reinterpret_cast<UHandle *>(&lvStrHdlOut), strOutLen + sizeof(int32));
		if (!errorCode)
		{
			MoveBlock(reinterpret_cast<uChar *>(strOut), LStrBuf(*lvStrHdlOut), strOutLen);
			(*lvStrHdlOut)->cnt = strOutLen;
		}
		else
			errorMessage = (char *)"NumericArrayResize error";
		//deleteNativeArray(strOut);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getFullPath(const void *lvTreeNodePtr, LStrHandle lvStrHdlOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	char *strOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		strOut = treeNodePtr->getFullPath();
		std::size_t strOutLen = std::strlen(strOut);
//		errorCode = NumericArrayResize(uB, 1, reinterpret_cast<UHandle *>(&lvStrHdlOut), strOutLen);
		errorCode = NumericArrayResize(uB, 1, reinterpret_cast<UHandle *>(&lvStrHdlOut), strOutLen + sizeof(int32));
		if (!errorCode)
		{
			MoveBlock(reinterpret_cast<uChar *>(strOut), LStrBuf(*lvStrHdlOut), strOutLen);
			(*lvStrHdlOut)->cnt = strOutLen;
		}
		else
			errorMessage = (char *)"NumericArrayResize error";
		deleteNativeArray(strOut);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getLength(const void *lvTreeNodePtr, int *lengthOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		*lengthOut = treeNodePtr->getLength();
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getMember(const void *lvTreeNodePtr, void **lvTreeNodePtrOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	TreeNode *treeNodePtrOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		treeNodePtrOut = treeNodePtr->getMember();
		*lvTreeNodePtrOut = reinterpret_cast<void *>(treeNodePtrOut);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getMembers(const void *lvTreeNodePtr, LPtrArrHdl lvPtrArrHdlOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	TreeNode **membersArrOut;
	int membersArrLen = 0;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		membersArrOut = treeNodePtr->getMembers(&membersArrLen);
		// checks whether the size of a pointer is 32 bit or 64 bit depending upon the system architecture
		int32 typeCode = (sizeof(void *) > sizeof(int32)) ? iQ : iL;
		// resizes treenode array
		errorCode = NumericArrayResize(typeCode, 1, reinterpret_cast<UHandle *>(&lvPtrArrHdlOut), static_cast<int32>(membersArrLen));
		if (!errorCode)
		{
			for (int i = 0; i < membersArrLen; i++)
				(*lvPtrArrHdlOut)->elt[i] = static_cast<void *>(membersArrOut[i]);
			(*lvPtrArrHdlOut)->dimSize = static_cast<int32>(membersArrLen);
		}
		else
			errorMessage = (char *)"NumericArrayResize error";
		deleteNativeArray(reinterpret_cast<Data **>(membersArrOut));
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getMinPath(const void *lvTreeNodePtr, LStrHandle lvStrHdlOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	char *strOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		strOut = treeNodePtr->getMinPath();
		std::size_t strOutLen = std::strlen(strOut);
//		errorCode = NumericArrayResize(uB, 1, reinterpret_cast<UHandle *>(&lvStrHdlOut), strOutLen);
		errorCode = NumericArrayResize(uB, 1, reinterpret_cast<UHandle *>(&lvStrHdlOut), strOutLen + sizeof(int32));
		if (!errorCode)
		{
			MoveBlock(reinterpret_cast<uChar *>(strOut), LStrBuf(*lvStrHdlOut), strOutLen);
			(*lvStrHdlOut)->cnt = strOutLen;
		}
		else
			errorMessage = (char *)"NumericArrayResize error";
		deleteNativeArray(strOut);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}



EXPORT void mdsplus_treenode_getNid(const void *lvTreeNodePtr, int *nidOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		*nidOut = treeNodePtr->getNid();
	}
	catch (const MdsException &e)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(e.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getNode(const void *lvTreeNodePtr, void **lvTreeNodePtrOut, const char *relPathIn, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	TreeNode *treeNodePtrOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		treeNodePtrOut = treeNodePtr->getNode(const_cast<char *>(relPathIn));
		*lvTreeNodePtrOut = reinterpret_cast<void *>(treeNodePtrOut);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getNode_string(const void *lvTreeNodePtr, void **lvTreeNodePtrOut, const void *lvStringPtrIn, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	String *stringPtrIn = NULL;
	TreeNode *treeNodePtrOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		stringPtrIn = reinterpret_cast<String *>(const_cast<void *>(lvStringPtrIn));
		treeNodePtrOut = treeNodePtr->getNode(stringPtrIn);
		*lvTreeNodePtrOut = reinterpret_cast<void *>(treeNodePtrOut);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getNodeName(const void *lvTreeNodePtr, LStrHandle lvStrHdlOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	char *strOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		strOut = treeNodePtr->getNodeName();
		std::size_t strOutLen = std::strlen(strOut);
//		errorCode = NumericArrayResize(uB, 1, reinterpret_cast<UHandle *>(&lvStrHdlOut), strOutLen);
		errorCode = NumericArrayResize(uB, 1, reinterpret_cast<UHandle *>(&lvStrHdlOut), strOutLen + sizeof(int32));
		if (!errorCode)
		{
			MoveBlock(reinterpret_cast<uChar *>(strOut), LStrBuf(*lvStrHdlOut), strOutLen);
			(*lvStrHdlOut)->cnt = strOutLen;
		}
		else
			errorMessage = (char *)"NumericArrayResize error";
		deleteNativeArray(strOut);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getNumChildren(const void *lvTreeNodePtr, int *numChildrenOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		*numChildrenOut = treeNodePtr->getNumChildren();
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getNumDescendants(const void *lvTreeNodePtr, int *numDescendantsOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		*numDescendantsOut = treeNodePtr->getNumDescendants();
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getNumElts(const void *lvTreeNodePtr, int *numEltsOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		*numEltsOut = treeNodePtr->getNumElts();
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getNumMembers(const void *lvTreeNodePtr, int *numMembersOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		*numMembersOut = treeNodePtr->getNumMembers();
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getNumSegments(const void *lvTreeNodePtr, int *numSegmentsOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		*numSegmentsOut = treeNodePtr->getNumSegments();
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getOriginalPartName(const void *lvTreeNodePtr, LStrHandle lvStrHdlOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	char *strOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		strOut = treeNodePtr->getOriginalPartName();
		std::size_t strOutLen = std::strlen(strOut);
//		errorCode = NumericArrayResize(uB, 1, reinterpret_cast<UHandle *>(&lvStrHdlOut), strOutLen);
		errorCode = NumericArrayResize(uB, 1, reinterpret_cast<UHandle *>(&lvStrHdlOut), strOutLen+sizeof(int32));
		if (!errorCode)
		{
			MoveBlock(reinterpret_cast<uChar *>(strOut), LStrBuf(*lvStrHdlOut), strOutLen);
			(*lvStrHdlOut)->cnt = strOutLen;
		}
		else
			errorMessage = (char *)"NumericArrayResize error";
		deleteNativeArray(strOut);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getOwnerId(const void *lvTreeNodePtr, int *ownerIdOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		*ownerIdOut = treeNodePtr->getOwnerId();
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getParent(const void *lvTreeNodePtr, void **lvTreeNodePtrOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	TreeNode *treeNodePtrOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		treeNodePtrOut = treeNodePtr->getParent();
		*lvTreeNodePtrOut = reinterpret_cast<void *>(treeNodePtrOut);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getPath(const void *lvTreeNodePtr, LStrHandle lvStrHdlOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	char *strOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		strOut = treeNodePtr->getPath();
		std::size_t strOutLen = std::strlen(strOut);
		errorCode = NumericArrayResize(uB, 1, reinterpret_cast<UHandle *>(&lvStrHdlOut), strOutLen+sizeof(int32));
		if (!errorCode)
		{
			MoveBlock(reinterpret_cast<uChar *>(strOut), LStrBuf(*lvStrHdlOut), strOutLen);
			(*lvStrHdlOut)->cnt = strOutLen;
		}
		else
			errorMessage = (char *)"NumericArrayResize error";
		deleteNativeArray(strOut);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}
EXPORT void mdsplus_treenode_getSegment(const void *lvTreeNodePtr, void **lvArrayPtrOut, int segIdxIn, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	Array *arrayPtrOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		arrayPtrOut = treeNodePtr->getSegment(segIdxIn);
		*lvArrayPtrOut = reinterpret_cast<void *>(arrayPtrOut);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getSegmentLimits(const void *lvTreeNodePtr, int segmentIdxIn, void **lvStartDataPtrOut, void **lvEndDataPtrOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	Data *startDataPtrOut = NULL;
	Data *endDataPtrOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		treeNodePtr->getSegmentLimits(segmentIdxIn, &startDataPtrOut, &endDataPtrOut);
		*lvStartDataPtrOut = reinterpret_cast<void *>(startDataPtrOut);
		*lvEndDataPtrOut = reinterpret_cast<void *>(endDataPtrOut);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getStatus(const void *lvTreeNodePtr, int *statusOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		*statusOut = treeNodePtr->getStatus();
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getTimeInserted(const void *lvTreeNodePtr, int64_t *timeInsertedOut, LStrHandle lvStrHdlOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		*timeInsertedOut = treeNodePtr->getTimeInserted();
		Int64 *timeInsertedData = new Int64(*timeInsertedOut);
		Data *retTimeStr = executeWithArgs("date_time($)", 1, timeInsertedData);
		char *retStr = retTimeStr->getString();
		int32 retLen = static_cast<int32>(strlen(retStr));

//GAB Oct 2014
		if(NumericArrayResize(uB, 1, reinterpret_cast<UHandle *>(&lvStrHdlOut), sizeof(int32) + retLen) == noErr)
		{
			MoveBlock(reinterpret_cast<uChar *>(retStr), LStrBuf(*lvStrHdlOut), retLen);
			(*lvStrHdlOut)->cnt = retLen;
		}
		deleteNativeArray(retStr);
		deleteData(retTimeStr);
		deleteData(timeInsertedData);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getTree(const void *lvTreeNodePtr, void **lvTreePtrOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	Tree *treePtrOut = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		treePtrOut = treeNodePtr->getTree();
		*lvTreePtrOut = reinterpret_cast<void *>(treePtrOut);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_getUsage(const void *lvTreeNodePtr, LStrHandle lvStrHdlOut, ErrorCluster *error)
{
	MgErr errorCode = noErr;
	char const * errorSource = __func__;
	char const * errorMessage = (char *)"";
	try {
		TreeNode * treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		char const * strOut = treeNodePtr->getUsage();
		std::size_t strOutLen = strlen(strOut);
//		errorCode = NumericArrayResize(uB, 1, reinterpret_cast<UHandle *>(&lvStrHdlOut), strOutLen);
		errorCode = NumericArrayResize(uB, 1, reinterpret_cast<UHandle *>(&lvStrHdlOut), strOutLen + sizeof(int32));
		if (!errorCode) {
			MoveBlock(strOut, LStrBuf(*lvStrHdlOut), strOutLen);
			(*lvStrHdlOut)->cnt = strOutLen;
		} else
			errorMessage = (char *)"NumericArrayResize error";
	} catch (const MdsException &e) {
		errorCode = bogusError;
		errorMessage = e.what();
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_isChild(const void *lvTreeNodePtr, LVBoolean *lvIsChildOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		*lvIsChildOut = (treeNodePtr->isChild() == true) ? LVBooleanTrue : LVBooleanFalse;
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_isCompressOnPut(const void *lvTreeNodePtr, LVBoolean *lvIsCompressOnPutOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		*lvIsCompressOnPutOut = (treeNodePtr->isCompressOnPut() == true) ? LVBooleanTrue : LVBooleanFalse;
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_isEssential(const void *lvTreeNodePtr, LVBoolean *lvIsEssentialOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		*lvIsEssentialOut = (treeNodePtr->isEssential() == true) ? LVBooleanTrue : LVBooleanFalse;
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}

	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_isIncludedInPulse(const void *lvTreeNodePtr, LVBoolean *lvisIncludeInPulseOut, ErrorCluster *error)
{
    mdsplus_treenode_isIncludeInPulse(lvTreeNodePtr, lvisIncludeInPulseOut, error);
}

EXPORT void mdsplus_treenode_isIncludeInPulse(const void *lvTreeNodePtr, LVBoolean *lvisIncludeInPulseOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		*lvisIncludeInPulseOut = (treeNodePtr->isIncludeInPulse() == true) ? LVBooleanTrue : LVBooleanFalse;
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_isMember(const void *lvTreeNodePtr, LVBoolean *lvIsMemberOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		*lvIsMemberOut = (treeNodePtr->isMember() == true) ? LVBooleanTrue : LVBooleanFalse;
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_isNoWriteModel(const void *lvTreeNodePtr, LVBoolean *lvIsNoWriteModelOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		*lvIsNoWriteModelOut = (treeNodePtr->isNoWriteModel() == true) ? LVBooleanTrue : LVBooleanFalse;
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_isNoWriteShot(const void *lvTreeNodePtr, LVBoolean *lvIsNoWriteShotOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		*lvIsNoWriteShotOut = (treeNodePtr->isNoWriteShot() == true) ? LVBooleanTrue : LVBooleanFalse;
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_isOn(const void *lvTreeNodePtr, LVBoolean *lvIsOnOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		*lvIsOnOut = (treeNodePtr->isOn() == true) ? LVBooleanTrue : LVBooleanFalse;
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_isSetup(const void *lvTreeNodePtr, LVBoolean *lvIsSetupOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		*lvIsSetupOut = (treeNodePtr->isSetup() == true) ? LVBooleanTrue : LVBooleanFalse;
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_isWriteOnce(const void *lvTreeNodePtr, LVBoolean *lvIsWriteOnceOut, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		*lvIsWriteOnceOut = (treeNodePtr->isWriteOnce() == true) ? LVBooleanTrue : LVBooleanFalse;
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_makeSegment(const void *lvTreeNodePtr, const void *lvStartDataPtrIn, const void *lvEndDataPtrIn, const void *lvTimeDataPtrIn, const void *lvArrayPtrIn, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	Data *startDataPtrIn = NULL;
	Data *endDataPtrIn = NULL;
	Data *timeDataPtrIn = NULL;
	Array *initialDataArrayPtrIn = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		startDataPtrIn = reinterpret_cast<Data *>(const_cast<void *>(lvStartDataPtrIn));
		endDataPtrIn = reinterpret_cast<Data *>(const_cast<void *>(lvEndDataPtrIn));
		timeDataPtrIn = reinterpret_cast<Data *>(const_cast<void *>(lvTimeDataPtrIn));
		initialDataArrayPtrIn = reinterpret_cast<Array *>(const_cast<void *>(lvArrayPtrIn));
		if(!treeNodePtr)
		  throw MdsException("treeNodePtr NULL");
		if(!startDataPtrIn)
		  throw MdsException("startDataPtrIn NULL");
		if(!endDataPtrIn)
		  throw MdsException("startDataPtrIn NULL");
		if(!timeDataPtrIn)  
		  throw MdsException("timeDataPtrIn NULL");
		if(!initialDataArrayPtrIn)
		  throw MdsException("initialDataArrayPtrIn NULL");
  		treeNodePtr->makeSegment(startDataPtrIn, endDataPtrIn, timeDataPtrIn, initialDataArrayPtrIn);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_makeTimestampedSegment(const void *lvTreeNodePtr, const void *lvArrayPtrIn, const LLngArrHdl lvLngArrHdlIn, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	Array *arrayPtrIn = NULL;
	int64_t *timesArrIn = NULL;	
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		arrayPtrIn = reinterpret_cast<Array *>(const_cast<void *>(lvArrayPtrIn));
		int timesArrLen = static_cast<int>((*lvLngArrHdlIn)->dimSize);
		timesArrIn = new int64_t[timesArrLen];
		for (int i = 0; i < timesArrLen; i++)
			timesArrIn[i] = static_cast<int64_t>((*lvLngArrHdlIn)->elt[i]);
		treeNodePtr->makeTimestampedSegment(arrayPtrIn, timesArrIn);
		delete[] timesArrIn;
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_move(const void *lvTreeNodePtr, const void *lvTreeNodePtrIn, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	TreeNode *treeNodeParentPtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		treeNodeParentPtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtrIn));
		treeNodePtr->move(treeNodeParentPtr);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_move_newName(const void *lvTreeNodePtr, const void *lvTreeNodePtrIn, const char *newNameIn, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	TreeNode *treeNodeParentPtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		treeNodeParentPtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtrIn));
		treeNodePtr->move(treeNodeParentPtr, const_cast<char *>(newNameIn));
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_putData(const void *lvTreeNodePtr, const void *lvDataPtrIn, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	Data *dataPtrIn = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		dataPtrIn = reinterpret_cast<Data *>(const_cast<void *>(lvDataPtrIn));
		treeNodePtr->putData(dataPtrIn);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}


EXPORT void mdsplus_treenode_putRow(const void *lvTreeNodePtr, const void *lvDataPtrIn, int64_t *timeIn, int sizeIn, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	Data *dataPtrIn = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		dataPtrIn = reinterpret_cast<Data *>(const_cast<void *>(lvDataPtrIn));
		treeNodePtr->putRow(dataPtrIn, timeIn, sizeIn);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_putSegment(const void *lvTreeNodePtr, const void *lvArrayPtrIn, int ofsIn, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	Array *arrayPtrIn = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		arrayPtrIn = reinterpret_cast<Array *>(const_cast<void *>(lvArrayPtrIn));
		treeNodePtr->putSegment(arrayPtrIn, ofsIn);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_putTimestampedSegment(const void *lvTreeNodePtr, const void *lvArrayPtrIn, const LLngArrHdl lvLngArrHdlIn ,ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	Array *arrayPtrIn = NULL;
	int64_t *timesArrIn = NULL;	
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		arrayPtrIn = reinterpret_cast<Array *>(const_cast<void *>(lvArrayPtrIn));
		int timesArrLen = static_cast<int>((*lvLngArrHdlIn)->dimSize);
		timesArrIn = new int64_t[timesArrLen];
		for (int i = 0; i < timesArrLen; i++)
			timesArrIn[i] = static_cast<int64_t>((*lvLngArrHdlIn)->elt[i]);
		treeNodePtr->putTimestampedSegment(arrayPtrIn, timesArrIn);
		delete[] timesArrIn;
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_remove(const void *lvTreeNodePtr, const char *nameIn, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		treeNodePtr->remove(const_cast<char *>(nameIn));
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_removeTag(const void *lvTreeNodePtr, const char *tagNameIn, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		treeNodePtr->removeTag(const_cast<char *>(tagNameIn));
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_rename(const void *lvTreeNodePtr, const char *newNameIn, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		treeNodePtr->rename(const_cast<char *>(newNameIn));
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_setCompressOnPut(const void *lvTreeNodePtr, LVBoolean *lvCompressOnPutIn, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		if (*lvCompressOnPutIn == LVBooleanTrue)
			treeNodePtr->setCompressOnPut(true);
		else
			treeNodePtr->setCompressOnPut(false);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_setEssential(const void *lvTreeNodePtr, LVBoolean *lvEssentialIn, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		if (*lvEssentialIn == LVBooleanTrue)
			treeNodePtr->setEssential(true);
		else
			treeNodePtr->setEssential(false);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_setIncludedInPulse(const void *lvTreeNodePtr, LVBoolean *lvIncludeInPulseIn, ErrorCluster *error)
{
    mdsplus_treenode_setIncludeInPulse(lvTreeNodePtr, lvIncludeInPulseIn, error);
}

EXPORT void mdsplus_treenode_setIncludeInPulse(const void *lvTreeNodePtr, LVBoolean *lvIncludeInPulseIn, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		if (*lvIncludeInPulseIn == LVBooleanTrue)
			treeNodePtr->setIncludeInPulse(true);
		else
			treeNodePtr->setIncludeInPulse(false);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_setNoWriteModel(const void *lvTreeNodePtr, LVBoolean *lvSetNoWriteModelIn, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		if (*lvSetNoWriteModelIn == LVBooleanTrue)
			treeNodePtr->setNoWriteModel(true);
		else
			treeNodePtr->setNoWriteModel(false);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_setNoWriteShot(const void *lvTreeNodePtr, LVBoolean *lvNoWriteShotIn, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		if (*lvNoWriteShotIn == LVBooleanTrue)
			treeNodePtr->setNoWriteShot(true);
		else
			treeNodePtr->setNoWriteShot(false);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_teenode_setOn(const void *lvTreeNodePtr, LVBoolean *lvOnIn, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		if (*lvOnIn == LVBooleanTrue)
			treeNodePtr->setOn(true);
		else
			treeNodePtr->setOn(false);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_setSubTree(const void *lvTreeNodePtr, LVBoolean *lvSubTreeIn, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		if (*lvSubTreeIn == LVBooleanTrue)
			treeNodePtr->setSubtree(true);
		else
			treeNodePtr->setSubtree(false);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_setTree(const void *lvTreeNodePtr, const void *lvTreePtrIn, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	Tree *treePtrIn = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		treePtrIn = reinterpret_cast<Tree *>(const_cast<void *>(lvTreePtrIn));
		treeNodePtr->setTree(treePtrIn);
	}
	catch (const MdsException &e)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(e.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_setWriteOnce(const void *lvTreeNodePtr, LVBoolean *lvWriteOnceIn, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		if (*lvWriteOnceIn == LVBooleanTrue)
			treeNodePtr->setWriteOnce(true);
		else
			treeNodePtr->setWriteOnce(false);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

EXPORT void mdsplus_treenode_updateSegment(const void *lvTreeNodePtr, const void *lvStartDataPtrIn, const void *lvEndDataPtrIn, const void *lvTimeDataPtrIn, ErrorCluster *error)
{
	TreeNode *treeNodePtr = NULL;
	Data *startDataPtrIn = NULL;
	Data *endDataPtrIn = NULL;
	Data *timeDataPtrIn = NULL;
	MgErr errorCode = noErr;
	const char *errorSource = __FUNCTION__;
	char const * errorMessage = "";
	try
	{
		treeNodePtr = reinterpret_cast<TreeNode *>(const_cast<void *>(lvTreeNodePtr));
		startDataPtrIn = reinterpret_cast<Data *>(const_cast<void *>(lvStartDataPtrIn));
		endDataPtrIn = reinterpret_cast<Data *>(const_cast<void *>(lvEndDataPtrIn));
		timeDataPtrIn = reinterpret_cast<Data *>(const_cast<void *>(lvTimeDataPtrIn));
		treeNodePtr->updateSegment(startDataPtrIn, endDataPtrIn, timeDataPtrIn);
	}
	catch (const MdsException &mdsE)
	{
		errorCode = bogusError;
		errorMessage = const_cast<char *>(mdsE.what());
		fillErrorCluster(errorCode, errorSource, errorMessage, error);
		return;
	}
	fillErrorCluster(errorCode, errorSource, errorMessage, error);
}

/********************************************************************************************************
												TREENODEARRAY
 ********************************************************************************************************/

EXPORT void mdsplus_treenodearray_destructor(void **lvTreeNodeArrayPtr)
{
	TreeNodeArray *treeNodeArrayPtr = reinterpret_cast<TreeNodeArray *>(*lvTreeNodeArrayPtr);
	delete treeNodeArrayPtr;
	*lvTreeNodeArrayPtr = NULL;
}

EXPORT TreeNode *mdsplus_treenodearray_getItem(void **lvTreeNodeArrayPtr, int n)
{
	TreeNodeArray *treeNodeArrayPtr = reinterpret_cast<TreeNodeArray *>(*lvTreeNodeArrayPtr);
	return (*treeNodeArrayPtr)[n];
}

EXPORT Int32Array *mdsplus_treenodearray_getCompressedLength(void **lvTreeNodeArrayPtr)
{
	TreeNodeArray *treeNodeArrayPtr = reinterpret_cast<TreeNodeArray *>(*lvTreeNodeArrayPtr);
	return treeNodeArrayPtr->getCompressedLength();
}

EXPORT StringArray *mdsplus_treenodearray_getFullPath(void **lvTreeNodeArrayPtr)
{
	TreeNodeArray *treeNodeArrayPtr = reinterpret_cast<TreeNodeArray *>(*lvTreeNodeArrayPtr);
	return treeNodeArrayPtr->getFullPath();
}

EXPORT Int32Array *mdsplus_treenodearray_getLength(void **lvTreeNodeArrayPtr)
{
	TreeNodeArray *treeNodeArrayPtr = reinterpret_cast<TreeNodeArray *>(*lvTreeNodeArrayPtr);
	return treeNodeArrayPtr->getLength();
}

EXPORT Int32Array *mdsplus_treenodearray_getNid(void **lvTreeNodeArrayPtr)
{
	TreeNodeArray *treeNodeArrayPtr = reinterpret_cast<TreeNodeArray *>(*lvTreeNodeArrayPtr);
	return treeNodeArrayPtr->getNid();
}

EXPORT StringArray *mdsplus_treenodearray_getPath(void **lvTreeNodeArrayPtr)
{
	TreeNodeArray *treeNodeArrayPtr = reinterpret_cast<TreeNodeArray *>(*lvTreeNodeArrayPtr);
	return treeNodeArrayPtr->getPath();
}

EXPORT StringArray *mdsplus_treenodearray_getUsage(void **lvTreeNodeArrayPtr)
{
	TreeNodeArray *treeNodeArrayPtr = reinterpret_cast<TreeNodeArray *>(*lvTreeNodeArrayPtr);
	return treeNodeArrayPtr->getUsage();
}

EXPORT Int8Array *mdsplus_treenodearray_isCompressOnPut(void **lvTreeNodeArrayPtr)
{
	TreeNodeArray *treeNodeArrayPtr = reinterpret_cast<TreeNodeArray *>(*lvTreeNodeArrayPtr);
	return treeNodeArrayPtr->isCompressOnPut();
}
EXPORT Int8Array *mdsplus_treenodearray_isNoWriteModel(void **lvTreeNodeArrayPtr)
{
	TreeNodeArray *treeNodeArrayPtr = reinterpret_cast<TreeNodeArray *>(*lvTreeNodeArrayPtr);
	return treeNodeArrayPtr->isNoWriteModel();
}
EXPORT Int8Array *mdsplus_treenodearray_isNoWriteShot(void **lvTreeNodeArrayPtr)
{
	TreeNodeArray *treeNodeArrayPtr = reinterpret_cast<TreeNodeArray *>(*lvTreeNodeArrayPtr);
	return treeNodeArrayPtr->isNoWriteShot();
}
EXPORT Int8Array *mdsplus_treenodearray_isOn(void **lvTreeNodeArrayPtr)
{
	TreeNodeArray *treeNodeArrayPtr = reinterpret_cast<TreeNodeArray *>(*lvTreeNodeArrayPtr);
	return treeNodeArrayPtr->isOn();
}
EXPORT Int8Array *mdsplus_treenodearray_isSetup(void **lvTreeNodeArrayPtr)
{
	TreeNodeArray *treeNodeArrayPtr = reinterpret_cast<TreeNodeArray *>(*lvTreeNodeArrayPtr);
	return treeNodeArrayPtr->isSetup();
}
EXPORT Int8Array *mdsplus_treenodearray_isWriteOnce(void **lvTreeNodeArrayPtr)
{
	TreeNodeArray *treeNodeArrayPtr = reinterpret_cast<TreeNodeArray *>(*lvTreeNodeArrayPtr);
	return treeNodeArrayPtr->isWriteOnce();
}





/********************************************************************************************************
												TREEPATH
 ********************************************************************************************************/

EXPORT void mdsplus_treepath_destructor(void **lvTreePathPtr)
{
	TreePath *treePathPtr = reinterpret_cast<TreePath *>(*lvTreePathPtr);
	deleteData(treePathPtr);
	*lvTreePathPtr = NULL;
}
