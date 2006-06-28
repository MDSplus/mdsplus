#ifndef SHARED_MEM_TREE_H
#define SHARED_MEM_TREE_H
#include "SharedmemManager.h"
#include "SharedMemNode.h"
#include "FreeSpaceManager.h"
#include "LockManager.h"

//Class ShaedMemInfo handles tree organization ifor the Shared Memory info tree

class SharedMemTree
{
private:
	FreeSpaceManager *freeSpaceManager;
	bool isEmpty;
	SharedMemNode *header;
	SharedMemNode *nullNode;
	
	SharedMemNode *current, *parent, *grand, *great;

	SharedMemNode *allocateMemNode();
	int compare(SharedMemNode *n, SharedMemNodeData *d);
	int compare(SharedMemNode *n, int nid);
	void handleReorient(SharedMemNodeData *data);
	SharedMemNode * rotate(SharedMemNodeData *data, SharedMemNode *node);
	SharedMemNode * rotateWithLeftChild(SharedMemNode *node);
	SharedMemNode * rotateWithRightChild(SharedMemNode *node);
	void printTree(SharedMemNode *);


public:
	void initialize(FreeSpaceManager *freeSpaceManager, void **header, void **nullNode);
	void map(FreeSpaceManager *freeSpaceManager, void *header, void *nullNode);
	void  insert(SharedMemNode *node);
	void insert(SharedMemNodeData *nodeData);
	void  remove(int nid);
	SharedMemNode  *find(int nid);
	void  printTree();


};



#endif