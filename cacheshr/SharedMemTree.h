#ifndef SHARED_MEM_TREE_H
#define SHARED_MEM_TREE_H
#include "SimpleAllocationManager.h"
#include "SharedMemNode.h"

//Class ShardMemInfo handles tree organization ifor the Shared Memory info tree

class SharedMemTree
{
private:
	SimpleAllocationManager *freeSpaceManager;
	bool isEmpty;
	SharedMemNode *header;
	SharedMemNode *current, *parent, *grand, *great;

	SharedMemNode *allocateMemNode();
	int compare(SharedMemNode *n, SharedMemNodeData *d);
	int compare(SharedMemNode *n, TreeDescriptor treeIdx, int nid);
	void printTree(SharedMemNode *);


public:
	void initialize(SimpleAllocationManager *freeSpaceManager, void **header);
	void map(SimpleAllocationManager *freeSpaceManager, void *header);
	void  insert(SharedMemNode *node);
	void insert(SharedMemNodeData *nodeData);
	void  remove(TreeDescriptor treeIdx, int nid);
	SharedMemNode  *find(TreeDescriptor treeIdx, int nid);
	void  printTree();


};



#endif
