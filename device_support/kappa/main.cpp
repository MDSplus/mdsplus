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
#include <mdsobjects.h>
using namespace MDSplus;
#include <sys/time.h>

#include <signal.h>

#include "kappazelos.h"
#include <cammdsutils.h>
#include <camstreamutils.h>

void *cameraHandle;
int kSockHandle=-1;  //MUST BE -1 THE FIRST TIME !!!
void *hBuffer;

void signal_callback_handler(int signum)
{
   printf("Caught signal %d\n",signum);
   // Cleanup and close up stuff here
 
   if(kSockHandle!=-1) camCloseTcpConnection(&kSockHandle); //close streaming

   kappaStopAcquisition(cameraHandle, hBuffer);
   kappaClose(cameraHandle);

   // Terminate program
   exit(signum);
}


int main(int argc, char **argv )
{
  int streamingPort = 0;
  int numFrame = 100;

  if((argv[1]==NULL) || (argv[2]==NULL) || (argv[3]==NULL))   // argv[4] is OPTIONAL
  { 
	printf("Please insert: 1)tree name 2)shot number 3)number of frame to acquire 4) Streaming Port (OPTIONAL)\n"); 
	exit(0); 
  }

  numFrame = atoi(argv[3]);

  if(argv[4])
  { 
	streamingPort = atoi(argv[4]);
  }


//MDSPLUS
  int res;
  void *treePtr;
  res=camOpenTree(argv[1], atoi( argv[2] ), &treePtr);
  if(res==-1)
  {
    printf("Error opening tree...\n"); 
	exit(0); 
  }

  Tree *tree; 
  TreeNode *node;
  int dataNid;

  tree = (Tree *)treePtr;
  node=tree->getNode("\\KAPPA:FRAMES");  	//search for KAPPA tag (the device)
  dataNid=node->getNid();					//Node id to save the acquired frames

  printf("frame node path: %s\n", node->getPath());
  printf("data node id: %d\n", dataNid); 
//MDSPLUS END


//INIT STREAMING
  int i=1;
  int canStream=-1;

if( streamingPort )
{

  while(canStream==-1 && i<=1) //try 5 times to open the connection
  { 
      canStream=camOpenTcpConnection( streamingPort, &kSockHandle, 1920, 1080);
      sleep(1);
      i++;
  }
  if(canStream==0)
  { 
    printf("Streaming OK!\n");  
  }
  else
  { 
    printf("CANNOT Streaming!\n"); 
  }

}    
//END STREAMING


//KAPPA

   if(kappaOpen("KappaIPstatico", &cameraHandle)==-1) exit(0);  //open camera with its name

   kappaSetColorCoding(cameraHandle, SDK4_ENUM_COLORCODING_Y14);

   kappaPrintInfo(cameraHandle);
   char ip[24];
   kappaGetIp(cameraHandle, &ip[0]);

   kappaSetExposureMode(cameraHandle, ZELOS_ENUM_EXPOSUREMODE_FREERUNNINGSEQUENTIAL); 
	//internal trigger: ZELOS_ENUM_EXPOSUREMODE_FREERUNNINGSEQUENTIAL
	//external trigger: ZELOS_ENUM_EXPOSUREMODE_RESETRESTART
   kappaSetExposure(cameraHandle, 0.07); //set exposure [s]

   int roaX=11;
   int roaY=11;
   int roaWidth=1920;
   int roaHeight=1080;

   kappaSetReadoutArea(cameraHandle, roaX, roaY, roaWidth, roaHeight);  
   kappaSetMeasureWindow(cameraHandle, roaX, roaY, roaWidth, roaHeight);

   uint32_t payloadSize=0;
   int32_t width;
   int32_t height; 
   kappaStartAcquisition(cameraHandle, &hBuffer, &width, &height, &payloadSize);

   void *frame;
   void *frame8bit;
   int32_t status;
   struct timeval tv;
   int64_t frameTime;
   int64_t prevFrameTime;
   int totFrameTime;
   int deltaT;
   float timeStamp;
 
   frame=malloc(payloadSize);
   frame8bit=malloc(width*height*sizeof(char));

   int32_t frameNumber = 0;

//   signal(SIGINT, signal_callback_handler);  //force exit

   for(int i=1; i<=numFrame; i++)  //acquire i=argv[3] frames
   { 
     printf("\n");
     frameNumber++;

     kappaGetFrame(cameraHandle, &status, frame);	//get the frame
	
     gettimeofday(&tv, NULL); 				  //get the timestamp
     frameTime=((tv.tv_sec)*1000) + ((tv.tv_usec)/1000);  //ms                        
     if (i==1)
     {
       prevFrameTime=frameTime;
       totFrameTime=0;
     }
     else
     {
       deltaT=int(frameTime-prevFrameTime);
       prevFrameTime=frameTime;
       totFrameTime=totFrameTime+deltaT;
     }
     timeStamp = float(totFrameTime)/float(1000.0);

     switch(status)
     {		 
	 	case 1: printf("get frame %d complete @ %.3f\n", frameNumber, timeStamp); break;
		case 2: printf("get frame %d incomplete\n", frameNumber); break;
		case 3: printf("get frame %d timeout\n", frameNumber); break;	
		default: printf("NEVER!\n"); break;
     }

      if(status==1)  
      {  
	 //SAVE FRAME IN MDSPLUS
	 //camSaveFrame(frame, width, height, timeStamp, 14, treePtr, dataNid, -1, frameNumber, 0, 0, 0, void *saveListPtr);

	 //STREAMING
	 if(canStream==0)
   	 {
	   unsigned int lowLim = 0;
	   unsigned int highLim = 20000;
	   camFrameTo8bit((unsigned short *)frame, 1920, 1080, (unsigned char *)frame8bit, 1, &lowLim, &highLim, 0, 32767);  //32767=max on Y14
	   //  printf("LowLim:%d HighLim:%d",lowLim, highLim);
	   camSendFrameOnTcp(&kSockHandle, width, height, frame8bit);
      	  }      		 

      }// if(status==1)  
		

   }//for	

   if(kSockHandle!=-1) camCloseTcpConnection(&kSockHandle); //close streaming

   kappaStopAcquisition(cameraHandle, hBuffer);
   kappaClose(cameraHandle);
}
