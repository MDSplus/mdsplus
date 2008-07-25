#ifndef MDSTREE_H
#define MDSTREE_H
#include "mdsdata.h"
#include "mdsstream.h"
#include <exception>
#include <usagedef.h>
#include <iostream.h>
#include <ncidef.h>

extern "C" void RTreeSynch();
extern "C" int TreeOpen(char *tree, int shot, int readOnly);
extern "C" int TreeClose(char *tree, int shot);
extern "C" int RTreeOpen(char *tree, int shot);
extern "C" int RTreeClose(char *tree, int shot);
extern "C" char *RTreeSetCallback(int nid, void *argument, void (*callback)(int, void *));
extern "C" char RTreeSetWarm(int nid, int warm);
extern "C" int RTreeClearCallback(int nid, char *callbackDescr);
extern "C" void RTreeConfigure(int shared, int size);
extern "C" int TreeClose(char *tree, int shot);
extern "C" int TreeFindNode(char *path, int *nid);
extern "C" char *MdsGetMsg(int status);
extern "C" int TreeFindNodeWild(char *path, int *nid, void **ctx, int mask);
extern "C" char *TreeGetPath(int nid);
extern "C" void TreeFree(void *ptr);
extern "C" int TreeGetNci(int nid, struct nci_itm *itmlst);
extern "C" int TreeIsOn(int nid);
extern "C" int TreeTurnOff(int nid);
extern "C" int TreeTurnOn(int nid);
extern "C" void convertTime(int *time, char *retTime);
extern "C" int TreeSetDefaultNid(int nid);
extern "C" int TreeGetDefaultNid(int *nid);

extern "C" int getTreeData(int nid, void **data, int isCached);
extern "C" int putTreeData(int nid, void *data, int isCached);
extern "C" int deleteTreeData(int nid);
extern "C" int beginTreeSegment(int nid, void *dataDsc, void *startDsc, void *endDsc, 
								void *timeDsc, int isCached);
extern "C" int putTreeRow(int nid, void *dataDsc, _int64 *time, int isCached, int isLast);
using namespace std;

namespace mdsplus
{
	class TreeInfo;
	class TreeNode;
	class TreeException: public exception
	{
		int status;
	public:
		TreeException(int status)
		{
			this->status = status;
		}
		virtual const char* what() const 
		{
			return MdsGetMsg(status);
		}
	};



	class TreeNode
	{
	protected:
		int nid;
	public:
		TreeNode(int nid)
		{
			this->nid = nid;
		}

		void setNid(int nid)
		{
			this->nid = nid;
		}


		char *getPath()
		{
			char *currPath = TreeGetPath(nid);
			char *path = new char[strlen(currPath) + 1];
			strcpy(path, currPath);
			TreeFree(currPath);
			return path;
		}


		virtual Data *getData()
		{
			Data *data = 0;
			int status = getTreeData(nid, (void **)&data, false);
			if(!(status & 1))
			{
				throw new TreeException(status);
			}
			return data;
		}

		virtual void putData(Data *data)
		{
			int status = putTreeData(nid, (void *)data, false);
			if(!(status & 1))
			{
				throw new TreeException(status);
			}
		}
		void deleteData()
		{
			int status = deleteTreeData(nid);
			if(!(status & 1))
			{
				throw new TreeException(status);
			}
		}


		int getNid() { return nid;}
		bool isOn()
		{
			return TreeIsOn(nid)?true:false;
		}
		void turnOn()
		{
			TreeTurnOn(nid);
		}
		void turnOff()
		{
			TreeTurnOff(nid);
		}

		char *getFullPath()
		{
			char path[512];
			int pathLen = 512;
			struct nci_itm nciList[] = 
				{{511, NciFULLPATH, path, &pathLen},
				{NciEND_OF_LIST, 0, 0, 0}};

			int status = TreeGetNci(nid, nciList);
			if(!(status & 1))
				throw new TreeException(status);
			char *retPath = new char[strlen(path)+1];
			strcpy(retPath, path);
			return retPath;
		}


		int getDataSize()
		{
			int length;
			int lengthLen = sizeof(int);
			struct nci_itm nciList[] = 
				{{4, NciLENGTH, &length, &lengthLen},
				{NciEND_OF_LIST, 0, 0, 0}};

			int status = TreeGetNci(nid, nciList);
			if(!(status & 1))
				throw new TreeException(status);
			return length;
		}

		char *getInsertionDate()
		{
			int timeInserted[2]; 
			int timeLen;
			struct nci_itm nciList[] = 
				{{8, NciTIME_INSERTED, timeInserted, &timeLen},
				{NciEND_OF_LIST, 0, 0, 0}};
			int status = TreeGetNci(nid, nciList);
			if(!(status & 1))
				throw new TreeException(status);
			char ascTim[512];
			convertTime(timeInserted, ascTim);

			char *retTim = new char[strlen(ascTim) + 1];
			strcpy(retTim, ascTim);
			return retTim;
		}

	};

	ostream &operator<<(ostream &stream, TreeNode *treeNode)
	{
		return stream << treeNode->getPath();
	}
///////////////////End Class TreeNode //////////////////////

/////////////////SegmentedTreeNode//////////////////////////
	class SegmentedTreeNode:public TreeNode, DataStreamConsumer
	{
	public:
		SegmentedTreeNode(int nid):TreeNode(nid){}

		virtual void acceptSegment(ArrayData *data, Data *start, Data *end, Data *times)
		{
			int status = beginTreeSegment(getNid(), data->convertToDsc(), start->convertToDsc(), 
				end->convertToDsc(), times->convertToDsc(), false);
			if(!(status & 1))
				throw new TreeException(status);
		}
		virtual void acceptRow(Data *data, _int64 time, bool isLast)
		{
			int status = putTreeRow(getNid(), data->convertToDsc(), &time, false, false);
			if(!(status & 1))
				throw new TreeException(status);
		}
	};

/////////////////End Class SegmentedTreeNode///////////////

/////////////////CachedTreeNode/////////////////////////////
	class CachedTreeNode: public SegmentedTreeNode
	{
	public:
		CachedTreeNode(int nid):SegmentedTreeNode(nid){}

		virtual void acceptSegment(ArrayData *data, Data *start, Data *end, Data *times)
		{
			int status = beginTreeSegment(getNid(), data->convertToDsc(), start->convertToDsc(), 
				end->convertToDsc(), times->convertToDsc(), true);
			if(!(status & 1))
				throw new TreeException(status);
		}
		virtual void acceptRow(Data *data, _int64 time, bool isLast)
		{
			int status = putTreeRow(getNid(), data->convertToDsc(), &time, true, isLast);
			if(!(status & 1))
				throw new TreeException(status);
		}
	};

////////////////End Class CachedTreeNode///////////////////
	static void callback(int nid, void *notifierPtr);
	
	
	class ActiveTreeNode: public TreeNode, DataStreamConsumer, DataStreamProducer
	{
		struct ConsumerList
		{
			DataStreamConsumer *consumer;
			ConsumerList *nxt;
		};
		ConsumerList *consumerHead;
		char *callbackHandle;

	public:
		ActiveTreeNode(int nid):TreeNode(nid)
		{
			consumerHead = 0;
			callbackHandle = 0;
		}

		virtual Data *getData()
		{
			Data *data = 0;
			int status = getTreeData(nid, (void **)&data, true);
			if(!(status & 1))
			{
				throw new TreeException(status);
			}
			return data;
		}

		virtual void putData(Data *data)
		{
			int status = putTreeData(nid, (void *)data, true);
			if(!(status & 1))
			{
				throw new TreeException(status);
			}
		}


		virtual void addDataStreamConsumer(DataStreamConsumer *consumer)
		{
			if(consumerHead == 0)
			{
				callbackHandle = RTreeSetCallback(nid, (void *)this, callback);
				RTreeSetWarm(nid, 1);
			}
			ConsumerList *item = new ConsumerList;
			item->consumer = consumer;
			item->nxt = consumerHead;
			consumerHead = item;
		}
		
		virtual void removeDataStreamConsumer(DataStreamConsumer *consumer)
		{
			ConsumerList *item = consumerHead, *prevItem = consumerHead;

			while(item)
			{
				if(item->consumer == consumer)
				{
					prevItem->nxt = item->nxt;
					if(item == consumerHead)
						consumerHead = item->nxt;
					delete item;
					break;
				}
				prevItem = item;
				item = item->nxt;
			}
			if(consumerHead == 0)
				RTreeClearCallback(nid, callbackHandle);

		}


		void notifyDataUpdate()
		{
			Data *data = getData();
			if(data->getClass() == CLASS_APD)
			{
				ApdData *apd = (ApdData *)data;
				Data **dscs = apd->getDscArray();
				int nDescs = apd->getDimension();
				if(nDescs == 3) //PutRow
				{
					ConsumerList *currItem = consumerHead;
					while(currItem)
					{
						currItem->consumer->acceptRow(dscs[0], ((ScalarData *)dscs[1])->getLong(), 
							((ByteData *)dscs[2])->getByte());
					
						currItem = currItem->nxt;
					}
				}
				else if(nDescs == 4)
				{
					ConsumerList *currItem = consumerHead;
					while(currItem)
					{
						currItem->consumer->acceptSegment((ArrayData *)dscs[0], dscs[1], dscs[2], dscs[3]);
						currItem = currItem->nxt;
					}
				}
				else 
				{
					printf("PANIC: Unexpected number (%d) of APD descriptors in notify\n", nDescs);
					exit(0);
				}
				delete data;
			}
			else
			{
				printf("PANIC: Unexpected descriptor in notify\n");
				printf("%s\n", data->decompile());
				exit(0);
			}
		}


		virtual void acceptSegment(ArrayData *data, Data *start, Data *end, Data *times)
		{
			Data **ptrs = new Data *[4];
			ptrs[0] = data;
			ptrs[1] = start;
			ptrs[2] = end;
			ptrs[3] = times;

			ApdData *apd = new ApdData(4, ptrs);
			putData(apd);
			delete apd;
		}

		
		virtual void acceptRow(Data *data, _int64 time, bool isLast)
		{
			Data **ptrs = new Data *[3];
			ptrs[0] = data;
			ptrs[1] = new LongData(time);
			ptrs[2] = new ByteData((char)isLast);

			ApdData *apd = new ApdData(3, ptrs);
			putData(apd);
		}
	};




	static void callback(int nid, void *notifierPtr)
	{
		((ActiveTreeNode *)notifierPtr)->notifyDataUpdate();
	}
	




////////////////Class Tree/////////////////////////////////


	class Tree
	{
	protected:
		char *name;
		int shot;
		bool opened;

	public:
		Tree(char *name, int shot)
		{
			this->name = new char[strlen(name) + 1];
			strcpy(this->name, name);
			this->shot = shot;
			opened = false;
		}

		virtual void open() 
		{
			int status;

			status = TreeOpen(name, shot, 0);
			if(!(status & 1))
			{
				throw new TreeException(status);
			}
			opened = true;
		}

		bool isOpen() {return this->opened; }

		virtual void close()
		{
			int status = TreeClose(name, shot);
			if(!(status & 1))
			{
				throw new TreeException(status);
			}
			opened = false;
		}

		TreeNode *getNode(char *path)
		{
			int nid, status;

			status = TreeFindNode(path, &nid);
			if(!(status & 1))
			{
				throw new TreeException(status);
			}
			return new TreeNode(nid);
		}

	
		SegmentedTreeNode *getSegmentedNode(char *path)
		{
			int nid, status;

			status = TreeFindNode(path, &nid);
			if(!(status & 1))
			{
				throw new TreeException(status);
			}
			return new SegmentedTreeNode(nid);
		}

	


		TreeNode **getNodeWild(char *path, int usageMask, int *retNumNodes)
		{
			int currNid, status; 
			int numNids = 0;
			void *ctx = 0;
			while ((status = TreeFindNodeWild(path,&currNid,&ctx, usageMask)) & 1)
				numNids++;

			TreeNode **retNodes = new TreeNode *[numNids];
			ctx = 0;
			for(int i = 0; i < numNids; i++)
			{
				TreeFindNodeWild(path,&currNid,&ctx, usageMask);
				retNodes[i] = new TreeNode(currNid);
			}
			*retNumNodes = numNids;
			return retNodes;
		}

		TreeNode **getNodeWild(char *path, int *retNumNodes)
		{
			return getNodeWild(path, TreeUSAGE_ANY, retNumNodes);
		}

		TreeNode **getMembersOf(TreeNode *node)
		{
			int nid = node->getNid();
			int numNids;
			int numNidsLen = sizeof(int);
			int nidsLen;
			struct nci_itm nciList1[] = 
			  {{4, NciNUMBER_OF_MEMBERS, &numNids, &numNidsLen},
			   {NciEND_OF_LIST, 0, 0, 0}},

			nciList2[] = 
				{{0, NciMEMBER_NIDS, 0, &nidsLen},
				{NciEND_OF_LIST, 0, 0, 0}};

			int status = TreeGetNci(nid, nciList1);
			if(!(status & 1))
				throw new TreeException(status);


			if(numNids > 0)
			{
				int *nids = new int[numNids];
				TreeNode **treeNodes = new TreeNode *[numNids];
				nciList2[0].buffer_length = sizeof(int) * numNids;
				nciList2[0].pointer = nids;
				status = TreeGetNci(nid, nciList2);
				if(!(status & 1))
					throw new TreeException(status);
				for(int i = 0; i < numNids; i++)
					treeNodes[i] = new TreeNode(nids[i]);
				delete[] nids;
				return treeNodes;
			}
			return 0;
		}
		TreeNode **getChildrenOf(TreeNode *node)
		{
			int nid = node->getNid();
			int numNids;
			int numNidsLen = sizeof(int);
			int nidsLen;
			struct nci_itm nciList1[] = 
			  {{4, NciNUMBER_OF_CHILDREN, &numNids, &numNidsLen},
			   {NciEND_OF_LIST, 0, 0, 0}},

			nciList2[] = 
				{{0, NciCHILDREN_NIDS, 0, &nidsLen},
				{NciEND_OF_LIST, 0, 0, 0}};

			int status = TreeGetNci(nid, nciList1);
			if(!(status & 1))
				throw new TreeException(status);


			if(numNids > 0)
			{
				int *nids = new int[numNids];
				TreeNode **treeNodes = new TreeNode *[numNids];
				nciList2[0].buffer_length = sizeof(int) * numNids;
				nciList2[0].pointer = nids;
				status = TreeGetNci(nid, nciList2);
				if(!(status & 1))
					throw new TreeException(status);
				for(int i = 0; i < numNids; i++)
					treeNodes[i] = new TreeNode(nids[i]);
				delete[] nids;
				return treeNodes;
			}
			return 0;
		}

		TreeNode *getParentOf(TreeNode *node)
		{
			int parentNid;
			int nidLen = sizeof(int);
			struct nci_itm nciList[] = 
			  {{4, NciPARENT, &parentNid, &nidLen},
			   {NciEND_OF_LIST, 0, 0, 0}};

			int status = TreeGetNci(node->getNid(), nciList);
			if(!(status & 1))
					throw new TreeException(status);
			return new TreeNode(parentNid);
		}



		void setDefault(TreeNode *treeNode)
		{
			int status = TreeSetDefaultNid(treeNode->getNid());
			if(!(status & 1)) 
				throw new TreeException(status);
		}

		TreeNode *getDeault()
		{
			int nid;

			int status = TreeGetDefaultNid(&nid);
			if(!(status & 1)) 
				throw new TreeException(status);
			return new TreeNode(nid);
		}
	};

/////////////////End Class Tree /////////////////////

/////////////////CachedTree/////////////////////////

#define DEFAULT_CACHE_SIZE 2000000

	class CachedTree: public Tree
	{
		bool isCacheShared;
		int cacheSize;
	public:
		CachedTree(char *name, int shot):Tree(name, shot)
		{
			isCacheShared = false;
			cacheSize = DEFAULT_CACHE_SIZE;
		}

		virtual void open() 
		{
			int status;
			
			RTreeConfigure(1, cacheSize);
			status = RTreeOpen(name, shot);
			if(!(status & 1))
			{
				throw new TreeException(status);
			}
			opened = true;
		}

		bool isOpen() {return this->opened; }

		virtual void close()
		{
			int status = RTreeClose(name, shot);
			if(!(status & 1))
			{
				throw new TreeException(status);
			}
			opened = false;
		}

		void configure(bool isCacheShared, int cacheSize)
		{
			if(opened) return; //Configuration must be done before opening tree

			this->isCacheShared = isCacheShared;
			this->cacheSize = cacheSize;
		}

		CachedTreeNode *getCachedNode(char *path)
		{
			int nid, status;

			status = TreeFindNode(path, &nid);
			if(!(status & 1))
			{
				throw new TreeException(status);
			}
			return new CachedTreeNode(nid);
		}
		ActiveTreeNode *getActiveNode(char *path)
		{
			int nid, status;

			status = TreeFindNode(path, &nid);
			if(!(status & 1))
			{
				throw new TreeException(status);
			}
			return new ActiveTreeNode(nid);
		}
		void synch()
		{
			RTreeSynch();
		}
	};

}
#endif