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
#include <stdio.h>
#include <string.h>
#include <mdsobjects.h>
using namespace MDSplus;
#include "cammdsutils.h"
using namespace std;


//DEBUG mode if defined
//#define DEBUG



//Support class for enqueueing Frame storage requests
class SaveFrame {

    void *frame;
    int width;
    int height;
    int pixelSize;
    int segmentSize;
    int frameIdx;
    int dataNid;
    int timebaseNid;  //-1 means no timebase
    float frameTime;
    void *treePtr;

    bool hasMetadata;
    void *frameMetadata;
    int metaSize;
    int metaNid;


    SaveFrame *nxt;

 public:
    SaveFrame(void *frame, int width, int height, float frameTime, int pixelSize, void *treePtr, int dataNid, int timebaseNid, int frameIdx, void *frameMetadata, int metaSize, int metaNid)
    {
		this->frame = frame;
		this->width = width;
		this->height = height;
		this->pixelSize = pixelSize;
		this->segmentSize = segmentSize;
		this->frameIdx = frameIdx;
		this->dataNid = dataNid;
		this->timebaseNid = timebaseNid;
		this->frameTime = frameTime;

        hasMetadata = true;
		this->frameMetadata = frameMetadata;
		this->metaSize = metaSize;
		this->metaNid = metaNid;

		this->treePtr = treePtr;
		nxt = 0;
    }
    SaveFrame(void *frame, int width, int height, float frameTime, int pixelSize, void *treePtr, int dataNid, int timebaseNid, int frameIdx)
    {
		this->frame = frame;
		this->width = width;
		this->height = height;
		this->pixelSize = pixelSize;
		this->segmentSize = segmentSize;
		this->frameIdx = frameIdx;
		this->dataNid = dataNid;
		this->timebaseNid = timebaseNid;
		this->frameTime = frameTime;

        hasMetadata = false;
		this->frameMetadata = 0;
		this->metaSize = 0;
		this->metaNid = -1;

		this->treePtr = treePtr;
		nxt = 0;
    }

    void setNext(SaveFrame *itm)
    {
		nxt = itm;
    }

    SaveFrame *getNext()
    {
	    return nxt;
    }

    void save()
    {

	    int dataDims[] = {1, height, width};
	    int metaDims[] = {1, metaSize};


	    TreeNode *dataNode = new TreeNode(dataNid, (Tree *)treePtr);
	    TreeNode *metaNode;
        if(hasMetadata)
            metaNode = new TreeNode(metaNid, (Tree *)treePtr);
	    TreeNode *tBase = new TreeNode(timebaseNid, (Tree *)treePtr);

	    Data *data;
	    Data *metaData = 0;
        if(hasMetadata)
	        metaData = new Int8Array((char *)frameMetadata, 2, metaDims);

	//check pixel size format
	    if(pixelSize<=8)
	    {
		    data = new Int8Array((char *)frame, 3, dataDims);
	#ifdef DEBUG
		    printf("Pixel Format: 8bit.\n");
	#endif
	    }
	    else if(pixelSize<=16)
	    {
		    data = new Int16Array((short *)frame, 3, dataDims);
	#ifdef DEBUG
		    printf("Pixel Format: 16bit. %d %d %d\n", dataDims[0], dataDims[1], dataDims[2]);
	#endif
	    }
	    else if(pixelSize<=32)
	    {
		    data = new Int32Array((int *)frame, 3, dataDims);
	#ifdef DEBUG
		    printf("Pixel Format: 32bit.\n");
	#endif
	    }

	    if(timebaseNid != -1)  //timebase defined (ext. trigger)
	    {
		    TreeNode *timebaseNode;
		    Data *idxData;
		    Data *time;
		    Data *dim;

		    try
		    {
			    timebaseNode = new TreeNode(timebaseNid, (Tree *)treePtr);
    #ifdef DEBUG
			    printf("SAVE Frame IDX %d %s\n", frameIdx, dataNode->decompile());
    #endif
			    idxData = new Int32(frameIdx);
			    time = compileWithArgs("fs_float($1[$2])", (Tree *)treePtr, 2, timebaseNode, idxData);
			    dim = compileWithArgs("fs_float([$1[$2]])", (Tree *)treePtr, 2, timebaseNode, idxData);
			    dataNode->makeSegment(time, time, dim, (Array *)data);
			    if(hasMetadata && metaSize>0)
			    {
		  		    metaNode->makeSegment(time, time, dim, (Array *)metaData);
			    }
		    }
		    catch(MdsException *exc)
		    {
			    cout << "ERROR WRITING SEGMENT: " << exc->what() << "\n";
		    }
		    deleteData(data);
		    deleteData(time);
		    deleteData(dim);
		    delete dataNode;
		    delete timebaseNode;
		    if(hasMetadata) delete metaNode;

		    if(pixelSize<=8)
			    delete (char *) frame;
		    else if(pixelSize<=16)
			    delete (short *)frame;
		    else if(pixelSize<=32)
			    delete (int *) frame;

	    }
	    else  //timebase NOT defined (int. trigger)
	    {
		    Data *dim;
		    Data *time = new Float32(frameTime);

		    try
		    {
		        dim = compileWithArgs("[$1]", (Tree *)treePtr, 1, time);
		    }
		    catch(MdsException *exc)
		    {
			    cout << "ERROR CompileWithArgs: " << exc->what() << "\n";
		    }

		    try
		    {
			//cout << "WRITING SEGMENT " << time << dim << data << " " << dataNode->getFullPath() <<"\n";

			dataNode->makeSegment(time, time, dim, (Array *)data);
			if(hasMetadata && metaSize>0)
			{
		  		metaNode->makeSegment(time, time, dim, (Array *)metaData);
			}

		}
		catch(MdsException *exc)
		{
			cout << "ERROR WRITING SEGMENT " << exc->what() << "\n";
		}

		try
		{
			deleteData(data);
			deleteData(time);
			deleteData(dim);
			delete dataNode;
			if(hasMetadata) delete metaNode;

			if(pixelSize<=8)
				delete (char *) frame;
			else if(pixelSize<=16)
				delete (short *)frame;
			else if(pixelSize<=32)
				delete (int *) frame;
		}
		catch(MdsException *exc)
		{
			cout << "ERROR deleting data" << exc->what() << "\n";
		}
	}
   }
};

class SaveFrameList
{
	public:
		pthread_cond_t itemAvailable;
		pthread_t thread;
		bool threadCreated;
		SaveFrame *saveHead, *saveTail;
		bool stopReq;
        bool deferredSave;
		pthread_mutex_t mutex;
	public:
    void setDeferredSave(bool deferredSave)
    {
        this->deferredSave = deferredSave;
    }
    SaveFrameList()
    {
		int status = pthread_mutex_init(&mutex, NULL);
		pthread_cond_init(&itemAvailable, NULL);
		saveHead = saveTail = NULL;
		stopReq = false;
        deferredSave = false;
		threadCreated = false;
    }

    void addFrame(SaveFrame *newItem)
    {
		pthread_mutex_lock(&mutex);
		if(saveHead == NULL)
		{
			saveHead = saveTail = newItem;
		}
		else
		{
			saveTail->setNext(newItem);
			saveTail = newItem;
		}
		pthread_cond_signal(&itemAvailable);
		pthread_mutex_unlock(&mutex);

    }

    void addFrame(void *frame, int width, int height, float frameTime, int pixelSize, void *treePtr, int dataNid, int timebaseNid,
        int frameIdx,    void *frameMetadata, int metaSize, int metaNid)
    {
		SaveFrame *newItem = new SaveFrame(frame,  width,  height,  frameTime,  pixelSize,  treePtr,  dataNid,  timebaseNid,  frameIdx,
             frameMetadata,  metaSize,  metaNid);
        addFrame(newItem);
    }

    void addFrame(void *frame, int width, int height, float frameTime, int pixelSize, void *treePtr, int dataNid, int timebaseNid,
        int frameIdx)
    {
		SaveFrame *newItem = new SaveFrame(frame,  width,  height,  frameTime,  pixelSize,  treePtr,  dataNid,  timebaseNid,  frameIdx);
        addFrame(newItem);
    }



    void executeItems()
    {
		while(true)
		{
			pthread_mutex_lock(&mutex);
			if(stopReq && saveHead == NULL)
			{
			    pthread_mutex_unlock(&mutex);
			    pthread_exit(NULL);
			}
            if(deferredSave)
            {
			    pthread_cond_wait(&itemAvailable, &mutex);
                if(!stopReq)
                {
				    pthread_mutex_unlock(&mutex);
                    continue;
                }
                else
                {
                //Empty pending queue
                    while(saveHead)
                    {
			            SaveFrame *currItem = saveHead;
			            saveHead = saveHead->getNext();

#ifdef DEBUG
			            int nItems = 0;
			            for(SaveFrame *itm = saveHead; itm; itm = itm->getNext(), nItems++);
			            if( nItems > 0 && (nItems % 20 ) == 0 ) printf("THREAD ACTIVATED: %d store frame items pending\n", nItems);
#endif
			            currItem->save();
			            delete currItem;
                    }
			        pthread_mutex_unlock(&mutex);
			        pthread_exit(NULL);
                }
            }



			while(saveHead == NULL)
			{
			    pthread_cond_wait(&itemAvailable, &mutex);
			    if(stopReq && saveHead == NULL)
			    {
				    pthread_mutex_unlock(&mutex);
				    pthread_exit(NULL);
			    }
			}
			SaveFrame *currItem = saveHead;
			saveHead = saveHead->getNext();

#ifdef DEBUG
			int nItems = 0;
			for(SaveFrame *itm = saveHead; itm; itm = itm->getNext(), nItems++);
			if( nItems > 0 && (nItems % 20 ) == 0 ) printf("THREAD ACTIVATED: %d store frame items pending\n", nItems);
#endif
			pthread_mutex_unlock(&mutex);
			currItem->save();
			delete currItem;
		}
    }
    void start()
    {
		pthread_create(&thread, NULL, handleSave, (void *)this);
		threadCreated = true;
    }
    void stop()
    {
		stopReq = true;
		pthread_cond_signal(&itemAvailable);
		if(threadCreated)
		{
			pthread_join(thread, NULL);
#ifdef DEBUG
			printf("SAVE THREAD TERMINATED\n");
#endif
		}
    }
 };


static void *handleSave(void *listPtr)
{
    SaveFrameList *list = (SaveFrameList *)listPtr;
    list->executeItems();
    return NULL;
}

void camStartSave(void **retList)
{
    SaveFrameList *saveFrameList = new SaveFrameList;
    saveFrameList->start();
    *retList = (void *)saveFrameList;
}

void camStartSaveDeferred(void **retList)
{
    SaveFrameList *saveFrameList = new SaveFrameList;
    saveFrameList->setDeferredSave(true);
    saveFrameList->start();
    *retList = (void *)saveFrameList;
}

void camStopSave(void *listPtr)
{
    if(listPtr)
    {
        SaveFrameList *list = (SaveFrameList *)listPtr;
        list->stop();
        delete list;
    }
}






//Open Mdsplus Tree
int camOpenTree(char *treeName, int shot, void **treePtr)
{
    try
    {
	  Tree *tree = new Tree(treeName, shot);
	  *treePtr = (void *)tree;
	  return 0;
    }catch(MdsException *exc)
    {
	  cout << "Error opening Tree " << treeName << ": " << exc->what() << "\n";
      return -1;
    }
}


//Close Mdsplus Tree
int camCloseTree(void *treePtr)
{
	delete (Tree *)treePtr;
}



/*
Save 1 frame in mdsplus

void *frame			frame pointer
int width			frame width
int height			frame height
int64_t *frameTime		frame timestamp (used when 'timebaseNid'=-1)
int pixelSize			pixel depth (es 8 -> gray 8 bit)
void *treePtr			MdsPlus Tree pointer
int dataNid			MdsPlus NodeID for the frames
int timebaseNid			MdsPlus NodeID for the Timebase
int frameIdx			Frame Index (counter)
void *frameMetadata		frame metadata pointer
int metaSize			metadata size
int metaNid			MdsPlus NodeID for the metadata
*/

void camSaveFrame(void *frame, int width, int height, float frameTime, int pixelSize, void *treePtr, int dataNid, int timebaseNid, int frameIdx,
         void *frameMetadata, int metaSize, int metaNid, void *saveListPtr)
{

    void *bufFrame;
    void *bufMdata;
    int frameSize = width * height;

    if(pixelSize<=8)
    {
	     bufFrame = new char[frameSize];
    	 memcpy(bufFrame, frame, frameSize * sizeof(char));
    }
    else if(pixelSize<=16)
    {
	     bufFrame = new short[frameSize];
    	 memcpy(bufFrame, frame, frameSize * sizeof(short));
    }
    else if(pixelSize<=32)
    {
	     bufFrame = new int[frameSize];
    	 memcpy(bufFrame, frame, frameSize * sizeof(int));
    }

    bufMdata = new char[metaSize];
    memcpy(bufMdata, frameMetadata, metaSize);

    SaveFrameList *saveList = (SaveFrameList *)saveListPtr;
    saveList->addFrame(bufFrame,  width,  height,  frameTime,  pixelSize,  treePtr,  dataNid,  timebaseNid,  frameIdx,  bufMdata,  metaSize,  metaNid);
}

void camSaveFrameDirect(void *frame, int width, int height, float frameTime, int pixelSize, void *treePtr, int dataNid, int timebaseNid,
        int frameIdx, void *saveListPtr)
{

    void *bufFrame;
    int frameSize = width * height;
    if(pixelSize<=8)
    {
	     bufFrame = new char[frameSize];
    	 memcpy(bufFrame, frame, frameSize * sizeof(char));
    }
    else if(pixelSize<=16)
    {
	     bufFrame = new short[frameSize];
    	 memcpy(bufFrame, frame, frameSize * sizeof(short));
    }
    else if(pixelSize<=32)
    {
	     bufFrame = new int[frameSize];
    	 memcpy(bufFrame, frame, frameSize * sizeof(int));
    }

    SaveFrameList *saveList = (SaveFrameList *)saveListPtr;
    saveList->addFrame(bufFrame,  width,  height,  frameTime,  pixelSize,  treePtr,  dataNid,  timebaseNid,  frameIdx);
}


