#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
using namespace std;
#include <iostream>
//TCP SENDER
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

//FFMPEG TEXT OVERLAY
#include <time.h>
#include <fcntl.h>

#include "camstreamutils.h"

//debug mode if defined
//#define debug



//Support class for enqueueing Frame streaming requests
class StreamingFrame {

    void *frame;
    void *frameMetadata;
    int width;
    int height;
    int irFrameFormat;
    bool adjLimit;
    unsigned int *lowLim; 
    unsigned int *highLim;
    unsigned int minLim; 
    unsigned int maxLim;
    int tcpStreamHandle;
    const char *deviceName;

    StreamingFrame *nxt;

 public:
    StreamingFrame(int tcpStreamHandle, void *frame, void *frameMetadata, int width, int height, int irFrameFormat, bool adjLimit, unsigned int *lowLim, unsigned int *highLim, unsigned int minLim, unsigned int maxLim, const char *deviceName)
    {
		this->tcpStreamHandle = tcpStreamHandle; 
		this->frame = frame;
		this->frameMetadata = frameMetadata;
		this->width = width;
		this->height = height;
		this->irFrameFormat = irFrameFormat;
		this->adjLimit = adjLimit;
		this->lowLim = lowLim;
		this->highLim = highLim;
		this->minLim = minLim;
		this->maxLim = maxLim;
                this->deviceName = deviceName;
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

    void streaming()
    {
        //printf("lowLim: %d highLim: %d minLim: %d maxLim: %d\n", *lowLim, *highLim, minLim, maxLim);

	unsigned char *frame8bit = (unsigned char *) calloc(1, width * height * sizeof(char));

    	//if ( irFrameFormat == radiometric )
    	if ( irFrameFormat == 0 )
        	flirRadiometricConv(frame, width, height, frameMetadata) ;

    	camFrameTo8bit((unsigned short *) frame, width, height, frame8bit, adjLimit, lowLim, highLim, minLim, maxLim);

        struct tm *tm;    //date & time used in FFMPEG_OVERLAY_***DEVICE-NAME*** file
        time_t t;
        char str_datetime[25];
        t = time(NULL);
        tm = localtime(&t);
        strcpy(str_datetime,"");
        strftime(str_datetime, sizeof(str_datetime), "%d-%m-%Y  %H:%M:%S", tm);

        char textString[100];
        sprintf(textString, "%s - %s - Min.:%d Max.:%d",this->deviceName, str_datetime, *lowLim, *highLim);  //text overlay example: IRCAM01 - 2017-02-06 10:08:00 - Min.:10 Max.:50

        char filename[40];
        sprintf(filename, "FFMPEG_OVERLAY_%s.txt", this->deviceName);
       
        camFFMPEGoverlay(filename, textString);
        
    	camSendFrameOnTcp(&tcpStreamHandle, width, height, frame8bit);

	free(frame8bit);
/*
	if(pixelSize<=8)
	    delete (char *) frame;
	else if(pixelSize<=16)
	    delete (short *)frame;
	else if(pixelSize<=32)
	    delete (int *) frame;
*/
        delete (short *)frame;
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

    void addStreamingFrame(int tcpStreamHandle, void *frame, void *frameMetadata, int width, int height, int irFrameFormat, bool adjLimit, unsigned int *lowLim, unsigned int *highLim, unsigned int minLim, unsigned int maxLim, const char *deviceName)
    {
		StreamingFrame *newItem = new StreamingFrame(tcpStreamHandle, frame, frameMetadata,  width,height,  irFrameFormat,  adjLimit, lowLim, highLim, minLim,  maxLim, deviceName);
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
			currItem->streaming();  
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

void camStartStreaming(void **retList)
{
    StreamingFrameList *streamingFrameList = new StreamingFrameList;
    streamingFrameList->start();
    *retList = (void *)streamingFrameList;
}

void camStopStreaming(void *listPtr)
{
    if(listPtr) 
    {
        StreamingFrameList *list = (StreamingFrameList *)listPtr;
        list->stop();
        delete list;
    }
}

void camStreamingFrame(int tcpStreamHandle, void *frame, void *frameMetadata, int width, int height, int pixelSize, int irFrameFormat, bool adjLimit, unsigned int *lowLim, unsigned int *highLim, unsigned int minLim, unsigned int maxLim, const char *deviceName, void *streamingListPtr)
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
    streamingList->addStreamingFrame(tcpStreamHandle, bufFrame, bufMdata,  width, height,  irFrameFormat,  adjLimit, lowLim, highLim, minLim, maxLim, deviceName);

}


//***********************************************


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
     //when adjLimits==0, lowLim and highLim are used to adjust the frame. minLim and maxLim are NOT used.
     //minLim & maxLim depend on specific camera and are used only if adjLimits==1. In this case lowLim and highLim are calculated in the frame but cannot exceed the minLim and maxLim.
     //frame8bit is the 8 bit resized version of frame using the passed or calculated min and max limits

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


//IMPORTANT: the file used by drawtext filter in ffmpeg is reload every frame processed and so any operation on it MUST BE an atomic operation!!!
int camFFMPEGoverlay(const char *filename, const char *textString)
{
  char pathname[100];
  strcpy(pathname,"");  	      // Clear local filename 
  strcat(pathname,getenv("HOME"));    // get home dir 
  strcat(pathname, "/");
  strcat(pathname, filename);         // append my filename 

  char tmp_pathname[strlen(pathname)+2];				//temp file to modify let the operation be atomic
  snprintf(tmp_pathname,sizeof(tmp_pathname),"%s~",pathname);

  mode_t default_mode=S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH;
  int fd=open(tmp_pathname,O_RDWR|O_CREAT|O_TRUNC,default_mode);
  if (fd==-1) 
  {
    printf("Failed to open new file for writing\n");
    return -1;
  }

  size_t count = write(fd, textString, strlen(textString));

  if(count!=strlen(textString))
  {
     printf("Failed to write new content on file\n");
     return -1;
  }
  if (fsync(fd)==-1) 
  {
     printf("Failed to flush new file content to disc\n");
     return -1;
  }
  if (close(fd)==-1) 
  {
    printf("Failed to close new file\n");
    return -1;
  }
  if (rename(tmp_pathname,pathname)==-1) 
  {
    printf("Failed to move new file to final location\n");
    return -1;
  }

 return 0;
}

