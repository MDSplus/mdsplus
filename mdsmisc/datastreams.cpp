#include <pthread.h>
#include <mdsobjects.h>
#include <time.h>

extern "C" int registerListener(char *experiment, char *tree, int shot);
extern "C" void unregisterListener(int listenerId);
extern "C" void *getNewSamplesSerialized(int id);

static 	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static bool daemonStarted = false;
static pthread_t thread;
#define MAX_DIM 64
extern  "C" void *monitorStreamInfo(void *);
static MDSplus::TreeNode *getSegmentedNode(MDSplus::Data *data);


class StreamInfo
{
	MDSplus::Tree *tree;
	MDSplus::TreeNode *segNode;
	MDSplus::Data *compiledExpr;
	int lastSegment, lastNextRow;
	pthread_cond_t availCond;
	bool dataAvailable;
	MDSplus::Data *lastTime;

public:
	StreamInfo(MDSplus::Tree *tree, MDSplus::TreeNode *segNode, MDSplus::Data *compiledExpr)
	{
		this->tree = tree;
		this->segNode = segNode;
		this->compiledExpr = compiledExpr;
		lastSegment = 0;
		lastNextRow = 0;
		pthread_cond_init(&availCond, NULL);
		dataAvailable = false;
		lastTime = NULL;
	}
	~StreamInfo()
	{
		deleteData(compiledExpr);
		pthread_cond_destroy(&availCond);
		if(lastTime)
			deleteData(lastTime);
		delete tree;
	}

	void checkDataAvailable()
	{
		char dtype, dimct;
		int dims[MAX_DIMS], nextRow;
	    if(dataAvailable) return;
		int numSegments = segNode->getNumSegments();
		if(numSegments == 0)
			return;
		if(numSegments != lastSegment)
		{
			lastSegment = numSegments;
			segNode->getSegmentInfo(lastSegment-1, &dtype, &dimct, dims, &nextRow);
			lastNextRow = nextRow;
			dataAvailable = true;
			pthread_cond_signal(&availCond);
			return;
		}
		segNode->getSegmentInfo(lastSegment-1, &dtype, &dimct, dims, &nextRow);
		if(nextRow > lastNextRow)
		{
			lastNextRow = nextRow;
			dataAvailable = true;
			pthread_cond_signal(&availCond);
		}
	}

	MDSplus::Data *getNewSamplesSerialized()
	{
		while(!dataAvailable)
			pthread_cond_wait(&availCond, &mutex);
		MDSplus::Tree *tree = segNode->getTree();
		tree->setTimeContext(lastTime, NULL, NULL);
		MDSplus::Data *segData = segNode->getData();
		unsigned char clazz, dtype;
		if(!segData)
			return NULL;
    	segData->getInfo((char *)&clazz, (char *)&dtype);
		if(clazz != CLASS_R || dtype != DTYPE_SIGNAL)
		{
			std::cout << "Internal error: non signal returned from segmented node" << std::endl;
		    MDSplus::deleteData(segData);
			return NULL;
		}
		MDSplus::Signal *sig = (MDSplus::Signal *)segData;
		MDSplus::Data *sigData;
		MDSplus::Data *dimension = sig->getDimension();
		MDSplus::Data *evalDimension = dimension->data();
		MDSplus::deleteData(dimension);
		if(compiledExpr != NULL) //If an expression is defined and not only the npde reference
			sigData = compiledExpr->data();
		else
			sigData = segData->data();
		MDSplus::deleteData(segData);
		sigData->getInfo((char *)&clazz, (char *)&dtype);
		if(clazz != CLASS_A )
		{
			std::cout << "Internal error: not an array of samples returned" << std::endl;
		    MDSplus::deleteData(sigData);
			MDSplus::deleteData(evalDimension);
			return NULL;
		}
		evalDimension->getInfo((char *)&clazz, (char *)&dtype);
		if(clazz != CLASS_A)
		{
			std::cout << "Internal error: not an array of times returned" << std::endl;
		    MDSplus::deleteData(sigData);
			MDSplus::deleteData(evalDimension);
			return NULL;
		}
		//Throw first useless sample if not the first chunk
		MDSplus::Data *retData, *retDim;
		if(lastTime)
		{
			retData = ((MDSplus::Array *)sigData)->getSubArray(1, sigData->getSize() - 1); 
			retDim = ((MDSplus::Array *)evalDimension)->getSubArray(1, evalDimension->getSize() - 1); 
			deleteData(sigData);
			deleteData(evalDimension);
		}
		else
		{
			retData = sigData;
			retDim = evalDimension;
		}
		if(lastTime)
			MDSplus::deleteData(lastTime);
		lastTime = ((MDSplus::Array *)retDim)->getElementAt(retDim->getSize() - 1);
		MDSplus::Signal *retSig = new MDSplus::Signal(retData,NULL, retDim);
		int numBytes;
		char *serialized = retSig->serialize(&numBytes);
		deleteData(retSig);
		dataAvailable = false;
		MDSplus::Int8Array *retArr = new MDSplus::Int8Array(serialized, numBytes);
		delete[] serialized;
		return retArr;
	}
};  //End class StreamInfo

static MDSplus::TreeNode *getSegmentedNode(MDSplus::Data *data)
{
	unsigned char dtype, clazz;
	if(!data) return NULL;
	data->getInfo((char *)&clazz, (char *)&dtype);
	switch(clazz)
	{
		case CLASS_S:
			if(dtype == DTYPE_NID)
				return (MDSplus::TreeNode *)data;
			if(dtype == DTYPE_PATH)
			{
				try {
					MDSplus::Tree *tree = MDSplus::getActiveTree();
					char *pathName = ((MDSplus::TreePath *)data)->getString();
					MDSplus::TreeNode *node = tree->getNode(pathName);
					delete [] pathName;
					delete tree;
					return node;
				}catch(MDSplus::MdsException &exc){return NULL;}
			}
			return NULL;
		case CLASS_A:
		case CLASS_D:
			return NULL;
		case CLASS_R:
			MDSplus::Compound *compound = (MDSplus::Compound *)data;
			for(int i = 0; i < compound->getNumDescs(); i++)
			{
				MDSplus::TreeNode *retNode = getSegmentedNode(compound->getDescAt(i));
				if(retNode) return retNode;
			}
			return NULL;
	}
	return NULL;
}

static std::vector<StreamInfo *> streamInfoV;

int registerListener(char *expr, char *experiment, int shot)
{
	pthread_mutex_lock(&mutex);
	try {
		MDSplus::Tree *tree = new MDSplus::Tree(experiment, shot);
		MDSplus::Data *exprData = MDSplus::compile(expr, tree);
		MDSplus::TreeNode *segNode = getSegmentedNode(exprData);
		if(!segNode)
		{
			MDSplus::deleteData(exprData);
			delete tree;
			pthread_mutex_unlock(&mutex);
			return -1;
		}
		StreamInfo *si = new StreamInfo(tree, segNode, exprData);
		streamInfoV.push_back(si);
		if(!daemonStarted)
		{
			daemonStarted = true;
			pthread_create(&thread, NULL, monitorStreamInfo, NULL);
		}
		pthread_mutex_unlock(&mutex);
		return streamInfoV.size() - 1;
	}catch(MDSplus::MdsException &exc)
	{
		pthread_mutex_unlock(&mutex);
		return -1;
	}
}

void unregisterListener(int listenerId)
{
	if(listenerId < 0 || listenerId >= streamInfoV.size())
		return;
	pthread_mutex_lock(&mutex);
	StreamInfo *si = streamInfoV[listenerId];
	delete si;
	streamInfoV.erase(streamInfoV.begin()+listenerId);
	pthread_mutex_unlock(&mutex);
}
//Returns a XD pointer
void *getNewSamplesSerialized(int id)
{
	if(id < 0 || id >= streamInfoV.size())
		return NULL;
	pthread_mutex_lock(&mutex);
	StreamInfo *si = streamInfoV[id];
	MDSplus::Data *serialized = si->getNewSamplesSerialized();
	pthread_mutex_unlock(&mutex);
	void *descrPtr = serialized->convertToDsc();
	MDSplus::deleteData(serialized);
	return descrPtr;
}

void *monitorStreamInfo(void *par)
{
	struct timespec waitTime;


	waitTime.tv_sec = 0;
	waitTime.tv_nsec = 100000000; //100 ms
	std::cout << "PARTE MONITOR\n";
	while(true)
	{
		pthread_mutex_lock(&mutex);
		for(int i = 0; i < streamInfoV.size(); i++)
			streamInfoV[i]->checkDataAvailable();
		pthread_mutex_unlock(&mutex);
		nanosleep(&waitTime, NULL);
	}
}
	


