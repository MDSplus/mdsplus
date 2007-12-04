#ifndef SHARED_MEM_NODE_H
#define SHARED_MEM_NODE_H
#include "SharedMemNodeData.h"
//Class SharedMemNode keeps Tree-related information for the info tree
#define RED 1
#define BLACK 2

class SharedMemNode
{
	friend class SharedMemTree;
	bool isValid; 
	_int64 left, right;

//Shared Memory related stuff
	SharedMemNodeData data;



public:
	
/*	SharedMemNode(SharedMemNodeData *inData)
	{
		data = *inData;
		isValid = true;
	}
	
	void initialize(SharedMemNodeData *inData)
	{
		data = *inData;
		isValid = true;
	}

*/

    int  compare(SharedMemNodeData &nodeData)
	{
		return data.compare(&nodeData); 
	}

	SharedMemNodeData *  setData(SharedMemNodeData *nodeData)
	{
		data = *nodeData;
		data.adjustOffsets(nodeData);
		isValid = true;
		return &data;
	}


	void invalidate()
	{
		isValid = false;
	}


	void  print()
	{
		data.print();
	}


	SharedMemNode * setRightChild(SharedMemNode *node)
	{
		right = (_int64)((char *)node - (char *)this);
		return node;
	}

	SharedMemNode * setLeftChild(SharedMemNode *node)
	{
		left = (_int64)((char *)node - (char *)this);
		return node;
	}


	SharedMemNode *leftChild()
	{
		return (SharedMemNode *)((char *)this + left);
	}

	SharedMemNode *rightChild()
	{
		return (SharedMemNode *)((char *)this + right);
	}

	SharedMemNodeData *getData() { return &data;}

	void free(FreeSpaceManager *fsm, LockManager *lock)
	{
		data.free(fsm, lock);
		fsm->freeShared((char *)this, sizeof(SharedMemNode), lock);
	}


};

#endif
