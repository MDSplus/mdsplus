#ifndef HAVE_WINDOWS_H
typedef unsigned long long _int64;
#endif

#include "SharedMemTree.h"

/*class SharedMemTree
{
private:
	SharedMemManager *memManager;
	SharedMemNode *header;

	SharedMemNode *allocateMemNode();

public:
	SharedMemTree(SharedMemeManager *memManager);
	void insert(SharedMemNode *node);
	void insert(SharedMemNodeData *nodeData);
	void remove(SharedMemNode *node);
	void remove(SharedMemNodeData *nodeData);
	SharedMemNode *find(SharedMemNodeData *nodeData);

};

*/
//Initialize shared data and return header
void SharedMemTree::initialize(FreeSpaceManager *freeSpaceManager, void **headerPtr, LockManager *lock)
{
	SharedMemNodeData dummyData;
	this->freeSpaceManager = freeSpaceManager;
	header = 0;
	header = allocateMemNode(lock);
	header->setData(&dummyData);   
	header->setLeftChild(0);
	header->setRightChild(0);
	*headerPtr = header;
}

//
void SharedMemTree::map(FreeSpaceManager *freeSpaceManager, void *header)
{
	this->freeSpaceManager = freeSpaceManager;
	this->header = (SharedMemNode *)header;
}



SharedMemNode   *SharedMemTree::allocateMemNode(LockManager *lock)
{
	return (SharedMemNode *)freeSpaceManager->allocateShared(sizeof(SharedMemNode), lock); 
}

int SharedMemTree::compare(SharedMemNode*n1, SharedMemNodeData *n2)
{
	if(!n1)return 1;
	return n1->data.compare(n2);
}

int SharedMemTree::compare(SharedMemNode*n1, int treeId, int nid)
{
	if(!n1)return 1;
	return n1->data.compare(treeId, nid);
}



void SharedMemTree::insert(SharedMemNodeData *nodeData, LockManager *lock)
{
	current = parent = grand = header;
	while(current && compare(current, nodeData)!= 0)
	{
		great = grand;
		grand = parent;
		parent = current;
		if(compare(current, nodeData) < 0)
			current = current->leftChild();
		else
			current = current->rightChild();

	}
	if(current)
	{
		current->setData(nodeData);
		current->isValid = true;
	}
	else
	{
		current = allocateMemNode(lock);
		current->setLeftChild(0);
		current->setRightChild(0);
		current->isValid = true;
		current->setData(nodeData);
		if(compare(parent, nodeData) < 0)
			parent->setLeftChild(current);
		else
			parent->setRightChild(current);
	}
}


SharedMemNode * SharedMemTree::find(int treeId, int nid)
{
	current = header;
	for(;;)
	{
		if(current && compare(current, treeId, nid) < 0)
			current = current->leftChild();
		else if(current && compare(current, treeId, nid) > 0)
			current = current->rightChild();
		else 
			return current;
	}

}

void SharedMemTree::remove(int treeId, int nid, LockManager *lock)
{
	SharedMemNode *curr1, *prev1;
	current = parent = grand = header;
	for(;;)
	{
		grand = parent;
		parent = current;
		if(current && compare(current, treeId, nid) < 0)
			current = current->leftChild();
		else if(current && compare(current, treeId, nid) > 0)
			current = current->rightChild();
		else 
			break;
	}
	if(!current)
	{
		printf("PANIC: remove called for non existent tree node\n");
		exit(0);
	}
	if(!current->leftChild() && !current->rightChild())
	{
		if(parent->leftChild() == current)
			parent->setLeftChild(0);
		else
			parent->setRightChild(0);
	}
	else if(!current->leftChild())
	{
		if(parent->leftChild() == current)
			parent->setLeftChild(current->rightChild());
		else
			parent->setRightChild(current->rightChild());
	}
	else if(!current->rightChild())
	{
		if(parent->leftChild() == current)
			parent->setLeftChild(current->leftChild());
		else
			parent->setRightChild(current->leftChild());
	}
	else
	{
		if(parent->rightChild() == current)
		{
			prev1 = curr1 = current->leftChild();
			while(curr1->rightChild())
			{
				prev1 = curr1;
				curr1 = curr1->rightChild();
			}
			if(prev1 != curr1)
				prev1->setRightChild(curr1->leftChild());
			parent->setRightChild(curr1);
			curr1->setRightChild(current->rightChild());
			if(curr1 != prev1)
				curr1->setLeftChild(current->leftChild());
		}
		else
		{
			prev1 = curr1 = current->rightChild();
			while(curr1->leftChild())
			{
				prev1 = curr1;
				curr1 = curr1->leftChild();
			}
			prev1->setLeftChild(curr1->rightChild());
			parent->setLeftChild(curr1);
			if(curr1 != prev1)
				curr1->setRightChild(current->rightChild());
			curr1->setLeftChild(current->leftChild());
		}
		current->free(freeSpaceManager, lock);
	}
}




void SharedMemTree::printTree()
{
	printTree(header);
}

void SharedMemTree::printTree(SharedMemNode *node)
{
	if(node)
	{
		printTree(node->leftChild());
		if(node != header) node->print();
		printTree(node->rightChild());
	}
}

