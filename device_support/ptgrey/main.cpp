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
#include <sys/time.h>
#include <unistd.h>

#include "PTGREY.h"
#include <mdsobjects.h>

#include <cammdsutils.h>
#include <camstreamutils.h>

//using namespace MDSplus;
using namespace std;

#define USETHECAMERA //this let to use the camera or if not defined to read data from a pulse file.

int main(int argc, char **argv)
{
  if ((argv[1] == NULL) || (argv[2] == NULL) || (argv[3] == NULL)) // argv[4] is OPTIONAL
  {
    printf("Please insert: 1)tree name 2)shot number 3)number of frame to acquire 4) Streaming Port (OPTIONAL)\n");
    exit(0);
  }

  //MDSPLUS
  int res;
  void *treePtr;
  res = camOpenTree(argv[1], atoi(argv[2]), &treePtr);
  if (res == -1)
  {
    printf("Error opening tree...\n");
    exit(0);
  }

  MDSplus::Tree *tree;
  MDSplus::TreeNode *node;
  MDSplus::TreeNode *nodeMeta;
  int dataNid;

  try
  {
    tree = (MDSplus::Tree *)treePtr;
    node = tree->getNode((char *)"\\CAMERATEST::TOP:POINTGREY:FRAMES");
    nodeMeta = tree->getNode((char *)"\\CAMERATEST::TOP:POINTGREY:FRAMES_METAD");
    dataNid = node->getNid(); //Node id to save the acquired frames
  }
  catch (const MDSplus::MdsException &exc)
  {
    std::cout << "ERROR reading data" << exc.what() << "\n";
  }

  printf("frame node path: %s\n", node->getPath());
  //MDSPLUS END

#ifndef USETHECAMERA
  /*read frame and metadata from stored pulse file and use Radiometric Conversion*/

  Array *frameArray;
  Array *frameArrayMeta;
  int *shape;
  int *shapeMeta;
  int nEle = 3;

  Data *frameData;
  Data *frameDataMeta;
  short *framePtr;
  char *framePtrMeta;

  try
  {
    frameArray = node->getSegment(0);         //idx
    frameArrayMeta = nodeMeta->getSegment(0); //idx

    shape = (frameArray)->getShape(&nEle);         //shape[0]=numframe=1 shape[1]=numbyte
    shapeMeta = (frameArrayMeta)->getShape(&nEle); //shape[0]=numframe=1 shape[1]=numbyte

    int numbytes = shape[1];
    int numframes = shape[0];

    frameData = (frameArray)->getElementAt(0);
    frameDataMeta = (frameArrayMeta)->getElementAt(0);

    int dataDims[3];
    int dataDimsMeta[3];
    framePtr = (frameData)->getShortArray(dataDims);
    framePtrMeta = (frameDataMeta)->getByteArray(dataDimsMeta);
  }
  catch (const MDSplus::MdsException &exc)
  {
    std::cout << "ERROR reading data" << exc.what() << "\n";
  }

  PTGREY *PtgreyCam;
  PtgreyCam = new PTGREY(); //open without ip adress because we not use a phisical camera!
#endif

#ifdef USETHECAMERA

  //INIT STREAMING
  int kSockHandle = -1; //MUST BE -1 THE FIRST TIME !!!
  int i = 1;
  int canStream = -1;

  if (argv[4] != NULL)
  {

    while (canStream == -1 && i <= 5) //try 5 times to open the connection
    {
      canStream = camOpenTcpConnection((const char *)"127.0.0.1", atoi(argv[4]), &kSockHandle, 640, 480, CSU_PIX_FMT_GRAY16);
      sleep(1);
      i++;
    }
    if (canStream == 0)
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

  PTGREY *PtgreyCam;
  PtgreyCam = new PTGREY("192.168.100.18");
  if (!PtgreyCam->checkLastOp())
  {
    printf("Unable to connect!!!\n");
    exit(0);
  }
  else
  {
    printf("Successfully connected...\n");
  }

  int skipFrame = 0;
  int width = 0;
  int height = 0;
  int payloadSize = 0;
  int x, y = 0;

  printf("\nGETTING ALL CAMERA PARAMETERS: start\n");
  PtgreyCam->printAllParameters();
  printf("\nGETTING ALL CAMERA PARAMETERS: end\n\n");

  PtgreyCam->startAcquisition(&width, &height, &payloadSize);
  // printf( "2019-09-17: CODE COMMENTED FOR POINT GRAY TESTS \n");
  int status;
  void *metaData;
  void *frame;
  void *frame8bit;
  int frameNumber = 0;
  struct timeval tv;
  int64_t timeStamp;
  frame = malloc(width * height * sizeof(short));
  frame8bit = malloc(width * height * sizeof(char));

  //	metaData=malloc(payloadSize-(width*height*sizeof(short)));

  for (int i = 1; i <= atoi(argv[3]); i++) //acquire i=argv[3] frames
  {
    frameNumber++;
    PtgreyCam->getFrame(&status, frame, metaData);
    gettimeofday(&tv, NULL);
    timeStamp = ((tv.tv_sec) * 1000) + ((tv.tv_usec) / 1000); // timeStamp [ms]
    switch (status)
    {
    case 1:
      printf("get frame %d complete @ %ld\n", frameNumber, timeStamp);
      break;
    case 2:
      printf("get frame %d incomplete @ %ld\n", frameNumber, timeStamp);
      break;
    case 3:
      printf("get frame %d timeout @ %ld\n", frameNumber, timeStamp);
      break;
    case 4:
      printf("get frame %d complete+triggered @ %ld\n", frameNumber, timeStamp);
      break;
    }

    if (status == 1 or status == 4)
    {
      //SAVE FRAME IN MDSPLUS

      /*	     res=camSaveFrame(frame, width, height, &timeStamp, 14, treePtr, dataNid, -1, frameNumber);
    	 if(res==-1)
   		 {
       		printf("Error in 'camSaveFrame'...\n"); 
     	 }
		 else
   		 {
       		printf("Frame saved...\n"); 
     	 }
*/

      if (canStream == 0)
      {
        //STREAMING
        if (skipFrame == 0)
          printf("ERROR SKIPFRAME=0\n");
        int sendFrame = i % skipFrame;
        if (canStream == 0 and sendFrame == 0)
        {
          //streaming is set to 640 x 480 in "camOpenTcpConnection".
          //It must be changed to prevent strange image transmission!
          unsigned int lowLim = 0;
          unsigned int highLim = 32000;

          camFrameTo8bit((unsigned short *)frame, 640, 480, CSU_PIX_FMT_GRAY16, (unsigned char *)frame8bit, 1, &lowLim, &highLim, 2000, 62000, 0, 0, 640, 480);
          //	   printf("LowLim:%d HighLim:%d\n",lowLim, highLim);
          camSendFrameOnTcp(&kSockHandle, width, height, frame8bit);
        }
      } //if canStream

    } //if status=1 or 4

  } //for

  if (kSockHandle != -1)
    camCloseTcpConnection(&kSockHandle); //close streaming

  // PtgreyCam->setCalibMode(1); //auto calib
  //   PtgreyCam->stopAcquisition();
  delete PtgreyCam;

#endif
}
