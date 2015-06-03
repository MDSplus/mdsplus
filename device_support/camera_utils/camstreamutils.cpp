#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
using namespace std;
#include <iostream>
//tcp sender
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include "camstreamutils.h"

//***********************************************



//Support class for enqueueing Frame streaming requests
class StreamingFrame {

    void *frame;
    void *frameMetadata;
    int width;
    int height;
	int irFrameFormat;
	bool adjLimit;
	unsigned int minLim; 
	unsigned int maxLim;
	int tcpStreamHandle;

    StreamingFrame *nxt;

 public:
    StreamingFrame(int tcpStreamHandle, void *frame, void *frameMetadata, int width, int height, int irFrameFormat, bool adjLimit, unsigned int minLim, unsigned int maxLim)
    {
		this->tcpStreamHandle = tcpStreamHandle; 
		this->frame = frame;
		this->frameMetadata = frameMetadata;
		this->width = width;
		this->height = height;
		this->irFrameFormat = irFrameFormat;

		this->adjLimit = adjLimit;
		this->minLim = minLim;
		this->maxLim = maxLim;

		nxt = 0;
    }

    void setNext(StreamingFrame *itm)
    {
		nxt = itm;
    }

    StreamingFrame *getNext()
    {
		return nxt;
    }

    void streaming( unsigned int *lowLim, unsigned int *highLim )
    {


	    unsigned char *frame8bit = (unsigned char *) calloc(1, width * height * sizeof(char));

    	//if ( irFrameFormat == radiometric )
    	if ( irFrameFormat == 0 )
        	flirRadiometricConv(frame, width, height, frameMetadata) ;

    	camFrameTo8bit((unsigned short *) frame, width, height, frame8bit, adjLimit, lowLim, highLim, minLim, maxLim);
    	camSendFrameOnTcp(&tcpStreamHandle, width, height, frame8bit);

		free(frame8bit);
    }
};

class StreamingFrameList
{
	public:
		pthread_cond_t itemAvailable;
		pthread_t thread;
		bool threadCreated;
		StreamingFrame *streamingHead, *streamingTail;
		bool stopReq;
		pthread_mutex_t mutex;
	public:

    StreamingFrameList()
    {
		int status = pthread_mutex_init(&mutex, NULL);
		pthread_cond_init(&itemAvailable, NULL);
		streamingHead = streamingTail = NULL;
		stopReq = false;
		threadCreated = false;
    }

    void addStreamingFrame(int tcpStreamHandle, void *frame, void *frameMetadata, int width, int height, int irFrameFormat, bool adjLimit, unsigned int minLim, unsigned int maxLim)
    {
		StreamingFrame *newItem = new StreamingFrame(tcpStreamHandle, frame, frameMetadata,  width,height,  irFrameFormat,  adjLimit, minLim,  maxLim);

		pthread_mutex_lock(&mutex);
		if(streamingHead == NULL)
		{
			streamingHead = streamingTail = newItem;
		}
		else
		{
			streamingTail->setNext(newItem);
			streamingTail = newItem;
		}
		pthread_cond_signal(&itemAvailable);
		pthread_mutex_unlock(&mutex);

    }


    void executeItems()
    {
		unsigned int lowLim; 
		unsigned int highLim;

		while(true)
		{
			pthread_mutex_lock(&mutex);
			if(stopReq && streamingHead == NULL)
			{
			    pthread_mutex_unlock(&mutex);
			    pthread_exit(NULL);
			}

			while(streamingHead == NULL)
			{
			    pthread_cond_wait(&itemAvailable, &mutex);
			    if(stopReq && streamingHead == NULL)
			    {
				    pthread_mutex_unlock(&mutex);
				    pthread_exit(NULL);
			    }
	    
			    int nItems = 0;
			    for(StreamingFrame *itm = streamingHead; itm; itm = itm->getNext(), nItems++);
			    //if(nItems > 2) printf("THREAD ACTIVATED: %d streaming items pending\n", nItems);
	    
			    if(nItems > 0  &&  (nItems % 20 ) == 0) printf("THREAD ACTIVATED: %d streaming items pending\n", nItems);

			}
			StreamingFrame *currItem = streamingHead;
			streamingHead = streamingHead->getNext();
	
			int nItems = 0;
			for(StreamingFrame *itm = streamingHead; itm; itm = itm->getNext(), nItems++);
			//if(nItems > 2) printf("THREAD ACTIVATED: %d streaming items pending\n", nItems);
			if(nItems > 0  &&  (nItems % 20 ) == 0) printf("THREAD ACTIVATED: %d streaming items pending\n", nItems);
	
			pthread_mutex_unlock(&mutex);
			currItem->streaming(&lowLim, &highLim);
			delete currItem;
		}
    }

    void start()
    {
		pthread_create(&thread, NULL, handleStreaming, (void *)this);
		threadCreated = true;
    }

    void stop()
    {
		stopReq = true;
		pthread_cond_signal(&itemAvailable);
		if(threadCreated)
		{	
			pthread_join(thread, NULL);
			printf("Streaming THREAD TERMINATED\n");
		}
    }
};


void *handleStreaming(void *listPtr)
{
    StreamingFrameList *list = (StreamingFrameList *)listPtr;
    list->executeItems();
    return NULL;
}

void startStreaming(void **retList)
{
    StreamingFrameList *streamingFrameList = new StreamingFrameList;
    streamingFrameList->start();
    *retList = (void *)streamingFrameList;
}

void stopStreaming(void *listPtr)
{
    if(listPtr) 
    {
        StreamingFrameList *list = (StreamingFrameList *)listPtr;
        list->stop();
        delete list;
    }
}

void camStreamingFrame(int tcpStreamHandle, void *frame, void *frameMetadata, int width, int height, int pixelSize, int irFrameFormat, bool adjLimit, unsigned int minLim, unsigned int maxLim, void *streamingListPtr)
{
    void *bufFrame;
    void *bufMdata;
    int frameSize = width * height;
	int metaSize = 3 * width;

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

    StreamingFrameList *streamingList = (StreamingFrameList *)streamingListPtr;

    streamingList->addStreamingFrame(tcpStreamHandle, bufFrame, bufMdata,  width, height,  irFrameFormat,  adjLimit, minLim,  maxLim);

}


//***********************************************



//debug mode if defined
//#define debug

int camOpenTcpConnection(int StreamingPort, int *kSockHandle, int width, int height)  
{
	return camOpenTcpConnectionNew("localhost",  StreamingPort, kSockHandle, width, height);
}

/*open TCP connection (localhost on specified StreamingPort) and init streaming (connection to FFMPEG)*/
int camOpenTcpConnectionNew(const char *streamingServer, int StreamingPort, int *kSockHandle, int width, int height)  
{
  struct sockaddr_in sin;
  struct hostent *hp;

#ifdef debug
  cout << "Open TCP connection..." << endl;
#endif


/* Resolve the passed name and store the resulting long representation in the struct hostent variable */
  if ((hp = gethostbyname(streamingServer)) == 0)
  {
   	//perror("gethostbyname");
    return -1;
  }

/* fill in the socket structure with host information */
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
  sin.sin_port = htons(StreamingPort);

/* create a new socket */
  if(*kSockHandle==-1)
  { 
    if ((*kSockHandle = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
      //perror("socket");
      return -1;
    }
  }

/* connect the socket to the port and host specified in struct sockaddr_in */
  if (connect(*kSockHandle,(struct sockaddr *)&sin, sizeof(sin)) == -1)
  {
    //perror("connect");
    //char lDoodle[] = "-+*";
    //printf("%c Error: connection refused!\r", lDoodle[rand()%3]);
    //cout << "kSockHandle: " <<  *kSockHandle << endl;
    return -1;
  }

#ifdef debug
  cout << "TCP opened and connected. kSockHandle=" << *kSockHandle << endl;
#endif

  unsigned int netWidth = htonl(width);
  unsigned int netHeight = htonl(height);

  /* init frame size streaming */
  if(send(*kSockHandle, &netWidth, sizeof(netWidth), 0) == -1)
  {
    //perror("send width");
    return -1;
  }
  
  if(send(*kSockHandle, &netHeight, sizeof(netHeight), 0) == -1)
  {
    //perror("send height");
    return -1;
  }

  return 0;
}



//close TCP connection
int camCloseTcpConnection(int *kSockHandle)
{
  if(*kSockHandle!=-1)
  {
    if(close(*kSockHandle)==-1)	
    {      perror("close");
      return -1;	
    }
    *kSockHandle=-1;
  }
  return 0;
}



//send 8bit frame over the opened TCP connection
int camSendFrameOnTcp(int *kSockHandle, int width, int height, void *frame8bit)
{
#ifdef debug
  cout << "camSendFrameOnTcp. Handle:" << *kSockHandle << endl; 
#endif

  if(*kSockHandle!=-1)
  {
        /* Send frame */
         if (send(*kSockHandle, frame8bit, width*height, 0) == -1)
         {
           camCloseTcpConnection(kSockHandle); //if send fail -> close socket
           return -1;
         }

#ifdef debug
        cout << "Frame spedito su TCP.\n" << endl;
#endif

  } 
  else
  {
       return -1;
  } 
   
  return 0;
}


int camFrameTo8bit(unsigned short *frame, int width, int height, unsigned char *frame8bit, bool adjLimits, unsigned int *lowLim, unsigned int *highLim, unsigned int minLim, unsigned int maxLim)
{
	//for FLIR 655
  	//unsigned int minLim = 2000; // 200 K or -73 deg Celsius
  	//unsigned int maxLim = 62000; // 6200 K
	
    unsigned short minpix, maxpix;
	if (adjLimits)
	{
		minpix = USHRT_MAX;
		maxpix = 0;
		for(int i=0; i<width*height; i++) 
		{
			if ((frame[i] > minLim) && (frame[i] < minpix))
			{
				minpix = frame[i];
			}
			if ((frame[i] < maxLim) && (frame[i] > maxpix))
			{
				maxpix = frame[i];
			}
		}
		*lowLim = minpix;
		*highLim = maxpix;
	}
	else
	{
		minpix = *lowLim;
		maxpix = *highLim;
	}

	unsigned short sample;
	unsigned char nsample = 0; // Normalized sample value (between 0 and 255)
	float span = (float)(maxpix - minpix + 1);

	for(int i=0; i<width*height; i++) 
	{
		sample = frame[i];

		if (sample < minpix)
		{
			nsample = 0;
		}
		else if (sample > maxpix)
		{
			nsample = 255; //127 in origine;
		}
		else 
		{
		    nsample = (unsigned char) (((sample - minpix) / span) * 0xFF);
			//nsample = nsample >> 1;
		}

		frame8bit[i] = nsample;
	}
}



