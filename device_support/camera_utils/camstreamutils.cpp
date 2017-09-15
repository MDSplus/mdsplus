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
#include <cerrno>        //tcp error enumeration

//FFMPEG TEXT OVERLAY
#include <time.h>
#include <fcntl.h>
#include <sys/time.h>   //provvisorio per debug

#include "camstreamutils.h"

//debug mode if defined
//#define debug


//support to get the pixel size of the selected pixel format.
int getPixelSize(int pixelFormat)    //return the number of bit used for the format
{
   if(pixelFormat==CSU_PIX_FMT_NONE)
     return 0;
   if(pixelFormat==CSU_PIX_FMT_GRAY8)
     return 8;
   if(pixelFormat==CSU_PIX_FMT_GRAY16)
     return 16;
   if(pixelFormat==CSU_PIX_FMT_BAYER_RGGB8)
     return 8;
   if(pixelFormat==CSU_PIX_FMT_YUV422_Packed)
     return 16;
}


//Support class for enqueueing Frame streaming requests
class StreamingFrame {

    void *frame;
    int width;
    int height;
    int pixelSize;
    int pixelFormat;    //according to GENICAM Pixel Format Naming Convention (PFNC 2.1)
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
    StreamingFrame(int tcpStreamHandle, void *frame, int width, int height, int pixelFormat, int irFrameFormat, bool adjLimit, unsigned int *lowLim, unsigned int *highLim, unsigned int minLim, unsigned int maxLim, const char *deviceName)
    {
		this->tcpStreamHandle = tcpStreamHandle; 
		this->frame = frame;
		this->width = width;
		this->height = height;
                this->pixelSize = getPixelSize(pixelFormat);
		this->pixelFormat = pixelFormat;
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
    	camFrameTo8bit(frame, width, height, pixelFormat, frame8bit, adjLimit, lowLim, highLim, minLim, maxLim);

        struct tm *tm;    //date & time used in FFMPEG_OVERLAY_***DEVICE-NAME*** file
        time_t t;
        char str_datetime[25];
        t = time(NULL);
        tm = localtime(&t);
        strcpy(str_datetime,"");
        strftime(str_datetime, sizeof(str_datetime), "%d-%m-%Y  %H:%M:%S", tm);

        char textString[100];
        sprintf(textString, "%s\n%s\nMin.:%d Max.:%d",this->deviceName, str_datetime, *lowLim, *highLim);  //text overlay example: IRCAM01 - 2017-02-06 10:08:00 - Min.:10 Max.:50

        char filename[40];
        sprintf(filename, "FFMPEG_OVERLAY_%s.txt", this->deviceName);
       
        camFFMPEGoverlay(filename, textString);
        
    	camSendFrameOnTcp(&tcpStreamHandle, width, height, frame8bit);
	free(frame8bit);

	if(pixelSize<=8)
	    delete (char *) frame;
	else if(pixelSize<=16)
	    delete (short *)frame;
	else if(pixelSize<=32)
	    delete (int *) frame;

                  struct timeval tv;  //frame timestamp
                  gettimeofday(&tv, NULL); 				  
                  int64_t timeStamp = ((tv.tv_sec)*1000) + ((tv.tv_usec)/1000); // timeStamp [ms]
//                  printf("Frame sent @: %ld\n", timeStamp);    

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

    void addStreamingFrame(int tcpStreamHandle, void *frame, int width, int height, int pixelFormat, int irFrameFormat, bool adjLimit, unsigned int *lowLim, unsigned int *highLim, unsigned int minLim, unsigned int maxLim, const char *deviceName)
    {
                //printf("add streaming frame\n");
		StreamingFrame *newItem = new StreamingFrame(tcpStreamHandle, frame, width, height, pixelFormat, irFrameFormat, adjLimit, lowLim, highLim, minLim,  maxLim, deviceName);
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
//fede			pthread_mutex_lock(&mutex);
			if(stopReq && streamingHead == NULL)
			{
//fede			    pthread_mutex_unlock(&mutex);
                            pthread_mutex_destroy(&mutex);    //fede 20170616
			    pthread_exit(NULL);
			}

			while(streamingHead == NULL)
			{
			    //pthread_cond_wait(&itemAvailable, &mutex);
			    if(stopReq && streamingHead == NULL)
			    {
//fede				    pthread_mutex_unlock(&mutex);
                                    pthread_mutex_destroy(&mutex);    //fede 20170616
				    pthread_exit(NULL);
			    }
	    
			    int nItems = 0;
			    for(StreamingFrame *itm = streamingHead; itm; itm = itm->getNext(), nItems++);
			    //if(nItems > 2) printf("THREAD ACTIVATED: %d streaming items pending\n", nItems);
	    
			    if(nItems > 0  &&  (nItems % 20 ) == 0) printf("THREAD ACTIVATED NULL HEAD: %d streaming items pending\n", nItems);
                            //printf("Streaming buffer queue empty...\n");
			}
pthread_mutex_lock(&mutex);
			StreamingFrame *currItem = streamingHead;
			streamingHead = streamingHead->getNext();
pthread_mutex_unlock(&mutex);
			int nItems = 0;
			for(StreamingFrame *itm = streamingHead; itm; itm = itm->getNext(), nItems++);
			if(nItems > 2) printf("THREAD ACTIVATED: %d streaming items pending\n", nItems);
			if(nItems > 0  &&  (nItems % 20 ) == 0) printf("THREAD ACTIVATED: %d streaming items pending\n", nItems);
	
//fede			pthread_mutex_unlock(&mutex);
			currItem->streaming();  
			delete currItem;
                        usleep(10000);  //10ms
//                        printf("end usleep\n");
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

void camStreamingFrame(int tcpStreamHandle, void *frame, int width, int height, int pixelFormat, int irFrameFormat, bool adjLimit, unsigned int *lowLim, unsigned int *highLim, unsigned int minLim, unsigned int maxLim, const char *deviceName, void *streamingListPtr)
{
    void *bufFrame;

    int pixelSize = getPixelSize(pixelFormat);
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

    StreamingFrameList *streamingList = (StreamingFrameList *)streamingListPtr;
    streamingList->addStreamingFrame(tcpStreamHandle, bufFrame, width, height, pixelFormat, irFrameFormat,  adjLimit, lowLim, highLim, minLim, maxLim, deviceName);    
}


//***********************************************

/*open TCP connection (localhost on specified StreamingPort) and init streaming (connection to FFMPEG)*/                       
int camOpenTcpConnection(const char *streamingServer, int StreamingPort, int *kSockHandle, int width, int height, int pixelFormat)  
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



//20170615 fede new

int n;
unsigned int m = sizeof(n);


// Set buffer size
 //n = 500000;
 //setsockopt(*kSockHandle, SOL_SOCKET, SO_SNDBUF, &n, sizeof(n));
 getsockopt(*kSockHandle,SOL_SOCKET,SO_RCVBUF,(void *)&n, &m);   //SO_RCVBUF
 printf("Buffer Receive size in bytes: %d\n", n);
 getsockopt(*kSockHandle,SOL_SOCKET,SO_SNDBUF,(void *)&n, &m);   //SO_RCVBUF
 printf("Buffer Send size in bytes: %d\n", n);

  // where socketfd is the socket you want to make non-blocking
/*
  int status = fcntl(*kSockHandle, F_SETFL, fcntl(*kSockHandle, F_GETFL, 0) | O_NONBLOCK); 
  if (status == -1){
  printf("calling fcntl");// handle the error.  By the way, I've never seen fcntl fail in this way
  }

  int newMaxBuff=512000;
  setsockopt(*kSockHandle, SOL_SOCKET, SO_SNDBUF, &newMaxBuff, sizeof(newMaxBuff));

  int flags = fcntl(*kSockHandle, F_GETFD);
  if ((flags & O_NONBLOCK) == O_NONBLOCK) {
    printf("Yup, it's nonblocking");
  }
  else {
    printf("Nope, it's blocking.");
  }
*/
//20170615 fede end new


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
  int netPixelFormat = htonl(pixelFormat);

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

  if(send(*kSockHandle, &netPixelFormat, sizeof(netPixelFormat), 0) == -1)
  {
    //perror("send pixel format");
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
//printf("before sending-->");
  if(*kSockHandle!=-1)
  {


    int total = 0;        // how many bytes we've sent
    int bytesleft = width*height; // how many we have left to send
    int n;

    while(total < width*height) {
        n=send(*kSockHandle, (void *)((char *)frame8bit+total), bytesleft, 0);  //flags: 0, MSG_DONTWAIT
        if (n == -1)
        {
           printf("TCP send failed. Code: %d |  %s\n",errno,strerror(errno));
           if(errno==11) //Resource temporarily unavailable
           {
             return -1;
           }  //return without close the connection
           camCloseTcpConnection(kSockHandle); //if send fail -> close socket
           return -1;
        }
        else
        {
           total += n;
           bytesleft -= n;
//         printf("Sent %d bytes. Bytesleft %d.\n", n, bytesleft);  //%zu
           if(bytesleft!=0)
           { 
              printf("Wait for sending remaining bytes...\n");
             // usleep(100000);//100ms
           }
        }
    }


        /* Send frame */
        //ssize_t ret=send(*kSockHandle, frame8bit, width*height, MSG_DONTWAIT | MSG_MORE);  //flags: 0, MSG_DONTWAIT
	//ssize_t ret=send(*kSockHandle, frame8bit, width*height, 0);  //flags: 0, MSG_DONTWAIT
/*
        if (ret == -1)
        {
           printf("TCP send failed. Code: %d |  %s\n",errno,strerror(errno));
           if(errno==11) //Resource temporarily unavailable
           {
             usleep(100000);//100ms
             return -1;
           }  //return without close the connection
           camCloseTcpConnection(kSockHandle); //if send fail -> close socket
           return -1;
        }
        else
        {
           printf("send %zu bytes\n", ret);
        }
*/
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


int camFrameTo8bit(void *frame, int width, int height, int pixelFormat, unsigned char *frame8bit, bool adjLimits, unsigned int *lowLim, unsigned int *highLim, unsigned int minLim, unsigned int maxLim)
{
     //when adjLimits==0, lowLim and highLim are used to adjust the frame. minLim and maxLim are NOT used.
     //minLim & maxLim depend on specific camera and are used only if adjLimits==1. In this case lowLim and highLim are calculated in the frame but cannot exceed the minLim and maxLim.
     //frame8bit is the 8 bit resized version of frame using the passed or calculated min and max limits
   
     int pixelSize = getPixelSize(pixelFormat);

       unsigned int minpix, maxpix;

	if (adjLimits)
	{
                maxpix = 0;

	        if(pixelSize<=8)
		   minpix = UCHAR_MAX;
	        else if(pixelSize<=16)
		   minpix = USHRT_MAX;
	        else if(pixelSize<=32)
		   minpix = UINT_MAX;
		
		for(int i=0; i<width*height; i++) 
		{
                     if(pixelSize<=8)
                     {
                        char *framePtr = (char *)frame;
			if ((framePtr[i] > minLim) && (framePtr[i] < minpix))
			{
				minpix = framePtr[i];
			}
			if ((framePtr[i] < maxLim) && (framePtr[i] > maxpix))
			{
				maxpix = framePtr[i];
			}
                     }
                     else if(pixelSize<=16)
                     {
                        short *framePtr = (short *)frame;
			if ((framePtr[i] > minLim) && (framePtr[i] < minpix))
			{
				minpix = framePtr[i];
			}
			if ((framePtr[i] < maxLim) && (framePtr[i] > maxpix))
			{
				maxpix = framePtr[i];
			}
                     }
                     else if(pixelSize<=32)
                     {
                        int *framePtr = (int *)frame;
			if ((framePtr[i] > minLim) && (framePtr[i] < minpix))
			{
				minpix = framePtr[i];
			}
			if ((framePtr[i] < maxLim) && (framePtr[i] > maxpix))
			{
				maxpix = framePtr[i];
			}
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

	unsigned char nsample = 0; // Normalized sample value (between 0 and 255)
	float span = (float)(maxpix - minpix + 1);

	for(int i=0; i<width*height; i++) 
	{
                     if(pixelSize<=8)                       //no need to normalize. already 8 bits!!!
                     {
                        char *framePtr = (char *)frame;
                        frame8bit[i] = framePtr[i];
                     }
                     else if(pixelSize<=16)
                     {
                        short *framePtr = (short *)frame;
                        unsigned short sample = framePtr[i];
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
                     else if(pixelSize<=32)
                     {
                        int *framePtr = (int *)frame;
                        unsigned int sample = framePtr[i];
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

