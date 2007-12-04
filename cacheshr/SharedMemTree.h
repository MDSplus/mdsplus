#ifndef SHARED_MEM_TREE_H
#define SHARED_MEM_TREE_H
#include "SharedMemManager.h"
#include "SharedMemNode.h"
#include "FreeSpaceManager.h"
#include "LockManager.h"

//Class ShardMemInfo handles tree organization ifor the Shared Memory info tree

class SharedMemTree
{
private:
	FreeSpaceManager *freeSpaceManager;
	bool isEmpty;
	SharedMemNode *header;
	SharedMemNode *current, *parent, *grand, *great;

	SharedMemNode *allocateMemNode(LockManager *lock);
	int compare(SharedMemNode *n, SharedMemNodeData *d);
	int compare(SharedMemNode *n, int treeId, int nid);
	void printTree(SharedMemNode *);


public:
	void initialize(FreeSpaceManager *freeSpaceManager, void **header, LockManager *lock);
	void map(FreeSpaceManager *freeSpaceManager, void *header);
	void  insert(SharedMemNode *node, LockManager *lock);
	void insert(SharedMemNodeData *nodeData, LockManager *lock);
	void  remove(int treeId, int nid, LockManager *lock);
	SharedMemNode  *find(int treeId, int nid);
	void  printTree();


};



#endif
