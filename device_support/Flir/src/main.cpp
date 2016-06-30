#include <mdsobjects.h>
using namespace MDSplus;
#include <sys/time.h>

#include "FLIRSC65X.h"
#include <libcammdsutils.h>
#include <libcamstreamutils.h>

int main(int argc, char **argv )
{
  if((argv[1]==NULL) || (argv[2]==NULL) || (argv[3]==NULL))   // argv[4] is OPTIONAL
  { 
	printf("Please insert: 1)tree name 2)shot number 3)number of frame to acquire 4) Streaming Port (OPTIONAL)\n"); 
	exit(0); 
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
  node=tree->getNode("\\FLIR:FRAMES");  	//search for FLIR tag (the device)
  dataNid=node->getNid();					//Node id to save the acquired frames

  printf("frame node path: %s\n", node->getPath());
  printf("data node id: %d\n", dataNid); 
//MDSPLUS END


//INIT STREAMING
  int kSockHandle=-1;  //MUST BE -1 THE FIRST TIME !!!
  int i=1;
  int canStream=-1;

if(argv[4]!=NULL)
{

  while(canStream==-1 && i<=5) //try 5 times to open the connection
  { 
    canStream=camOpenTcpConnection( atoi( argv[4] ), &kSockHandle, 640, 480);
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


//FLIR

	FLIR_SC65X *FlirCam;
    FlirCam = new FLIR_SC65X("192.168.50.20");
    if(!FlirCam->checkLastOp())
    {
		printf("Unable to connect!!!\n");
		exit(0);
	}
	else
    {
		printf("Successfully connected...\n");
	}	

	int skipFrame = 0;
	int width=0;
	int height=0;
	unsigned int payloadSize=0;
	int x,y=0;

	FlirCam->setObjectParameters(291.15, 292.15, 1.0, 0.95, 0.5, 294.15, 1.0, 0.0);
	FlirCam->setExposureMode(external_mode);	 //internal_mode  external_mode
	FlirCam->setFrameRate(fps_25, &skipFrame);  //200 100 50 25 12 6 3
	FlirCam->setIrFormat(radiometric);  		 //radiometric linear10mK linear100mK
	FlirCam->setMeasurementRange(2);
	//FlirCam->setReadoutArea(0, 0, 400, 200);  //NOT TO USE. setFrameRate set also ReadoutArea! 
	FlirCam->getReadoutArea(&x, &y, &width, &height);
	printf("Start x:%d Start y:%d Width:%d Height:%d\n", x, y, width, height);

	printf("\nGETTING ALL CAMERA PARAMETERS: start\n");
	FlirCam->printAllParameters();
	printf("\nGETTING ALL CAMERA PARAMETERS: end\n\n");

	FlirCam->startAcquisition(&width, &height, &payloadSize); 

   	int status;
	void *metaData;
	void *frame;
    void *frame8bit;
    int frameNumber = 0;
    struct timeval tv;
    int64_t timeStamp;

   	frame=malloc(width*height*sizeof(short));
	frame8bit=malloc(width*height*sizeof(char));
	metaData=malloc(payloadSize-(width*height*sizeof(short)));

	printf("Executing auto calibration and auto focus...\n");
	FlirCam->setCalibMode(0);
	FlirCam->executeAutoCalib();
    FlirCam->executeAutoFocus();

    for(int i=1; i<=atoi(argv[3]); i++)  //acquire i=argv[3] frames
    { 
      frameNumber++;

	  FlirCam->getFrame(&status, frame, metaData);
      
      gettimeofday(&tv, NULL); 				  
      timeStamp = ((tv.tv_sec)*1000) + ((tv.tv_usec)/1000); // timeStamp [ms]
     
      switch(status)
      {		 
	 	case 1: printf("get frame %d complete @ %ld\n", frameNumber, timeStamp); break;
	 	case 2: printf("get frame %d incomplete @ %ld\n", frameNumber, timeStamp); break;
	 	case 3: printf("get frame %d timeout @ %ld\n", frameNumber, timeStamp); break;
	 	case 4: printf("get frame %d complete+triggered @ %ld\n", frameNumber, timeStamp); break;
      }

      if(status==1 or status==4)  
      {  
		 //SAVE FRAME IN MDSPLUS
/*
	     res=camSaveFrame(frame, width, height, &timeStamp, 14, treePtr, dataNid, -1, frameNumber);
    	 if(res==-1)
   		 {
       		printf("Error in 'camSaveFrame'...\n"); 
     	 }
		 else
   		 {
       		printf("Frame saved...\n"); 
     	 }
*/
		 //STREAMING
		 if(skipFrame==0) printf("ERROR SKIPFRAME=0\n");
		 int sendFrame = i % skipFrame;
		 if( canStream==0 and sendFrame==0 )
   		 {
		   //streaming is set to 640 x 480 in "camOpenTcpConnection". 
		   //It must be changed to prevent strange image transmission!
		   unsigned int lowLim = 0;
		   unsigned int highLim = 32000;
		   camFrameTo8bit((unsigned short *)frame, 640, 480, (unsigned char *)frame8bit, 1, &lowLim, &highLim, 2000, 62000);
	//	   printf("LowLim:%d HighLim:%d\n",lowLim, highLim);
		   camSendFrameOnTcp(&kSockHandle, width, height, frame8bit);
     	 }      		 

	  }		

   }//for
	
   if(kSockHandle!=-1) camCloseTcpConnection(&kSockHandle); //close streaming

   FlirCam->setCalibMode(1); //auto calib
   FlirCam->stopAcquisition();
   delete FlirCam;
}
