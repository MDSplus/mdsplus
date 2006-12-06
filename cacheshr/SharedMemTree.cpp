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
void SharedMemTree::initialize(FreeSpaceManager *freeSpaceManager, void **headerPtr, void **nullNodePtr)
{
	SharedMemNodeData dummyData;
	this->freeSpaceManager = freeSpaceManager;
	header = 0;
	nullNode = allocateMemNode();
	nullNode->setLeftChild(nullNode);
	nullNode->setRightChild(nullNode);
	nullNode->color = BLACK;
	header = allocateMemNode();
	header->setData(&dummyData);   
	header->setLeftChild(nullNode);
	header->setRightChild(nullNode);
	header->color = BLACK;
	*headerPtr = header;
	*nullNodePtr = nullNode;
}

//
void SharedMemTree::map(FreeSpaceManager *freeSpaceManager, void *header, void *nullNode)
{
	this->freeSpaceManager = freeSpaceManager;
	this->header = (SharedMemNode *)header;
	this->nullNode = (SharedMemNode *)nullNode;
}



SharedMemNode   *SharedMemTree::allocateMemNode()
{
	return (SharedMemNode *)freeSpaceManager->allocateShared(sizeof(SharedMemNode)); 
}

int SharedMemTree::compare(SharedMemNode*n1, SharedMemNodeData *n2)
{
	if(!n1)return 1;
	return n1->data.compare(n2);
}

int SharedMemTree::compare(SharedMemNode*n1, int nid)
{
	if(!n1)return 1;
	return n1->data.compare(nid);
}



void SharedMemTree::insert(SharedMemNodeData *nodeData)
{
	current = parent = grand = header;
	nullNode->setData(nodeData);
	while(compare(current, nodeData)!= 0)
	{
		great = grand;
		grand = parent;
		parent = current;
		if(compare(current, nodeData) < 0)
			current = current->leftChild();
		else
			current = current->rightChild();

	//	if(current->leftChild()->color == RED && current->rightChild()->color == RED)
	//		handleReorient(nodeData);
	}
	if(current != nullNode)
	{
		current->setData(nodeData);
		current->isValid = true;
		//Adjust offsets in current->data
		current->data.adjustOffsets(nodeData);
	}
	else
	{
		current = allocateMemNode();
		current->setLeftChild(nullNode);
		current->setRightChild(nullNode);
		current->setCallbackManager(NULL);
		current->isValid = true;
		current->color = RED;
		current->setData(nodeData);
		//Adjust offsets in current->data
		current->data.adjustOffsets(nodeData);
		if(compare(parent, nodeData) < 0)
			parent->setLeftChild(current);
		else
			parent->setRightChild(current);

		//handleReorient(nodeData);
	}
}

void SharedMemTree::handleReorient(SharedMemNodeData *nodeData)
{
	current->color = RED;
	current->leftChild()->color = BLACK;
	current->rightChild()->color = BLACK;
	if(parent->color == RED)
	{
		grand->color = RED;
		if((compare(grand, nodeData) < 0) != (compare(parent, nodeData) < 0))
			parent = rotate(nodeData, grand);
		current = rotate(nodeData, great);

		current->color = BLACK;
	}
	header->rightChild()->color = BLACK;
}

SharedMemNode * SharedMemTree::rotate(SharedMemNodeData *nodeData, SharedMemNode *parent)
{
	if(compare(parent, nodeData) < 0)
	{
		if(compare(parent->leftChild(), nodeData) < 0)
			return parent->setLeftChild(rotateWithLeftChild(parent->leftChild()));
		else
			return parent->setLeftChild(rotateWithRightChild(parent->leftChild()));
	}
	else
	{
		if(compare(parent->rightChild(), nodeData) < 0)
			return parent->setRightChild(rotateWithLeftChild(parent->rightChild()));
		else
			return parent->setRightChild(rotateWithRightChild(parent->rightChild()));
	}
}

SharedMemNode * SharedMemTree::rotateWithLeftChild(SharedMemNode *k2 )
{
        SharedMemNode *k1 = k2->leftChild();
        k2->setLeftChild(k1->rightChild());
        k1->setRightChild(k2);
        return k1;
}

SharedMemNode * SharedMemTree:: rotateWithRightChild( SharedMemNode *k1 )
{
    SharedMemNode *k2 = k1->rightChild();
    k1->setRightChild(k2->leftChild());
    k2->setLeftChild(k1);
    return k2;
}


SharedMemNode * SharedMemTree::find(int nid)
{
	SharedMemNodeData currData(nid);
	nullNode->setData(&currData);
	current = header;

	for(;;)
	{
		if(compare(current, nid) < 0)
			current = current->leftChild();
		else if(compare(current, nid) > 0)
			current = current->rightChild();
		else if(current != nullNode)
			return current;
		else return 0;
	}

}

void SharedMemTree::remove(int nid)
{
	SharedMemNode *retNode = find(nid);
	if(retNode)
		retNode->invalidate();
}




void SharedMemTree::printTree()
{
	printTree(header);
}

void SharedMemTree::printTree(SharedMemNode *node)
{
	if(node != nullNode)
	{
		printTree(node->leftChild());
		if(node != header) node->print();
		printTree(node->rightChild());
	}
}

