#ifndef SHARED_MEM_NODE_H
#define SHARED_MEM_NODE_H
#include "RealTimeTreeshr.h"
#include "SharedMemNodeData.h"
#include "RealTimeTreeshr.h"
//Class SharedMemNode keeps Tree-related information for the info tree
#define RED 1
#define BLACK 2

class SharedMemNode
{
	friend class SharedMemTree;
//Red-Black Tree related stuff
	bool isValid; 
//	SharedMemNode *left, *right;
	long left, right;
	char  color;

//Used to handle tree balancing
//	SharedMemNode *current, *parent, *grand, *great;


//Shared Memory related stuff
	SharedMemNodeData data;

//Callback list
	long callbackManager;


public:
	
	SharedMemNode(SharedMemNodeData *inData)
	{
		data = *inData;
		isValid = true;
	}
	
	void initialize(SharedMemNodeData *inData)
	{
		data = *inData;
		isValid = true;
	}



    int  compare(SharedMemNodeData &nodeData)
	{
		return data.compare(&nodeData); 
	}

	SharedMemNodeData *  setData(SharedMemNodeData *nodeData)
	{
		data = *nodeData;
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
		right = (long)((char *)node - (char *)this);
		return node;
	}

	SharedMemNode * setLeftChild(SharedMemNode *node)
	{
		left = (long)((char *)node - (char *)this);
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

	void setCallbackManager(CallbackManager *callbackManager)
	{
		if(callbackManager == 0)
			this->callbackManager = 0;
		else
			this->callbackManager = (long)callbackManager - (long)this;
	}

	CallbackManager *getCallbackManager()
	{
		if(callbackManager == 0)
			return 0;
		return (CallbackManager *)((char *)this + callbackManager);
	}

	SharedMemNodeData *getData() { return &data;}


};

#endif
