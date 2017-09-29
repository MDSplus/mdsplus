#include <vector>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <mdsobjects.h>
using namespace MDSplus;
using namespace std;

#include "BASLERACA.h"
#include <cammdsutils.h>
#include <camstreamutils.h>

#include <math.h>
#include <sys/time.h>

#define BUFFER_COUNT 16 

#define ERROR -1
#define SUCCESS 0

//if defined, print to stdout messages.
#define debug


#define MAX_CAM 10
static BASLER_ACA *camPtr[MAX_CAM] = {0};
static char errorOnOpen[512];


#ifdef __cplusplus 
extern "C" 
{ 
#endif 


int baslerOpen(const char *ipAddress, int *camHandle)
{
	BASLER_ACA *cam;
	int cameraHandle;

	errorOnOpen[0] = 0;

	cam = new BASLER_ACA(ipAddress);

	if(cam->checkLastOp()==ERROR)
	{
		cam->getLastError(errorOnOpen); 
		delete(cam);
		return ERROR;
	}

	for( cameraHandle = 0; cameraHandle < MAX_CAM && camPtr[cameraHandle] ; cameraHandle++);

	if( cameraHandle < MAX_CAM )
	{
		camPtr[cameraHandle] = cam;
 		*camHandle = cameraHandle;
		return SUCCESS;
	}
	else
	{
		sprintf(errorOnOpen, "Exceed maximum number (10) of opened cameras ");
		*camHandle = -1;
		return ERROR;
	}
}


int baslerIsConnected( int camHandle )
{
	if( camHandle < 0 || camHandle >= MAX_CAM || camPtr[camHandle] == 0 ) 
	{
		sprintf(errorOnOpen, "Camera does not connect");
		return ERROR;
	}
	return SUCCESS;
}

int baslerClose(int camHandle)
{
	delete(camPtr[camHandle]);
	camPtr[camHandle] = 0;
	
	return SUCCESS;
}


int checkLastOp(int camHandle)
{
	return camPtr[camHandle]->checkLastOp();
}

int readInternalTemperature(int camHandle)
{
	return camPtr[camHandle]->readInternalTemperature();
}

int setExposure(int camHandle, double exposure)
{
	if( baslerIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->setExposure(exposure);
	return ERROR;
}


int setExposureAuto(int camHandle, char *exposureAuto)
{
	if( baslerIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->setExposureAuto(exposureAuto);
	return ERROR;
}


int setGain(int camHandle, int gain)
{
	if( baslerIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->setGain(gain);
	return ERROR;
}


int setGainAuto(int camHandle, char *gainAuto)
{
	if( baslerIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->setGainAuto(gainAuto);
	return ERROR;
}


int setGammaEnable(int camHandle, char *gammaEnable)
{
	if( baslerIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->setGammaEnable(gammaEnable);
	return ERROR;
}


int setFrameRate(int camHandle, double frameRate)
{
	if( baslerIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->setFrameRate(frameRate);
	return ERROR;
}


int setReadoutArea(int camHandle, int x, int y, int width, int height)
{
	if( baslerIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->setReadoutArea(x, y, width, height);
	return ERROR;
}


int setPixelFormat(int camHandle, char *pixelFormat)
{
	if( baslerIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->setPixelFormat(pixelFormat);
	return ERROR;
}


int startAcquisition(int camHandle, int *width, int *height, int *payloadSize)
{
	if( baslerIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->startAcquisition(width, height, payloadSize);
	return ERROR;
}


int stopAcquisition(int camHandle)
{
	if( baslerIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->stopAcquisition();
	return ERROR;
}


int getFrame(int camHandle, int *status, void *frame, void *metaData)
{
	if( baslerIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->getFrame(status, frame, metaData);
	return ERROR;
}

int setAcquisitionMode( int camHandle,  int storeEnabled,  int acqSkipFrameNumber)
{
	if( baslerIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->setAcquisitionMode(  storeEnabled, acqSkipFrameNumber);
	return ERROR;
}

int setTriggerMode( int camHandle, int triggerMode, double burstDuration, int numTrigger )
{
	if( baslerIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->setTriggerMode(  triggerMode,  burstDuration, numTrigger );
	return ERROR;
}

int softwareTrigger(int camHandle)
{
	if( baslerIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->softwareTrigger();
	return ERROR;
}

int setTreeInfo( int camHandle,  void *treePtr, int framesNid, int timebaseNid, int framesMetadNid, int frame0TimeNid)
{
	if( baslerIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->setTreeInfo( treePtr,  framesNid,  timebaseNid,  framesMetadNid, frame0TimeNid );
	return ERROR;
}

int setStreamingMode(int camHandle, int streamingEnabled, bool autoAdjustLimit, 
						const char *streamingServer, int streamingPort, int lowLim, int highLim, int adjRoiX, int adjRoiY, int adjRoiW, int adjRoiH, const char *deviceName)
{
	if( baslerIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->setStreamingMode( streamingEnabled,  autoAdjustLimit, streamingServer, streamingPort, lowLim, highLim, adjRoiX, adjRoiY, adjRoiW, adjRoiH, deviceName);
	return ERROR;

}

int startFramesAcquisition(int camHandle)
{
	if( baslerIsConnected( camHandle ) == SUCCESS )
	    return camPtr[camHandle]->startFramesAcquisition();
	return ERROR;
}

int stopFramesAcquisition(int camHandle)
{
	if( baslerIsConnected( camHandle ) == SUCCESS )
	    return camPtr[camHandle]->stopFramesAcquisition();
	return ERROR;
}

void  getLastError(int camHandle, char *msg)
{
	if(  camHandle < 0 || camHandle >= MAX_CAM || camPtr[camHandle] == 0 )
	{
		sprintf(msg, "%s", (errorOnOpen[0]==0) ? "" : errorOnOpen);
		return;
	}
    camPtr[camHandle]->getLastError(msg);
}


#ifdef __cplusplus 
} 
#endif 




BASLER_ACA::BASLER_ACA(const char *ipAddress)
{ 
   try
   {
     this->ipAddress = ipAddress; 
     PylonInitialize();
     CTlFactory& TlFactory = CTlFactory::GetInstance();
     CBaslerGigEDeviceInfo di;
     di.SetIpAddress(ipAddress);
     this->pDevice = TlFactory.CreateDevice( di);       
     this->pCamera = new Pylon::CBaslerGigEInstantCamera::CBaslerGigEInstantCamera(this->pDevice); 		
     pCamera->Open();
     printf("Device Connected.\n"); 
     lastOpRes=SUCCESS;
   }
   catch (const GenericException & e)
   {
     cerr << "Error Device connection. Reason: " << e.GetDescription() << endl;
     PylonTerminate();
     lastOpRes=ERROR;
   }
}


BASLER_ACA::BASLER_ACA()  //new 25/07/2013: let to use the device without the camera
{
    //this->lDevice = new PvDevice();     //SDK 3
    //SDK 4  09/03/2016  NON sembra possibile creare un PVDEVICE se non ho la telecamera
}


BASLER_ACA::~BASLER_ACA()
{
   PylonTerminate();
   printf("Device Disconnected.\n");
}



int BASLER_ACA::checkLastOp()
{
/*
	printf("Info %d (%s)\n", this->lResult.IsOK(), lResult.GetDescription().GetAscii() ); 
	if ( !this->lResult.IsOK() ) 
		printLastError("(%s)\n", lResult.GetDescription().GetAscii() ); 
*/
    return lastOpRes;
}


int BASLER_ACA::readInternalTemperature()
{  
    // Select the kind of internal temperature as the device temperature
    pCamera->TemperatureSelector.SetValue(TemperatureSelector_Coreboard);
    // Read the device temperature
    double t = pCamera->TemperatureAbs.GetValue();
    printf("Camera Temperature is now %3.2f°C\n", t); 
    this->internalTemperature=t;

   return SUCCESS;
}



int BASLER_ACA::setExposure(double exposure)
{  
   if (IsWritable(pCamera->ExposureTimeAbs))
   {
     pCamera->ExposureTimeAbs.SetValue(exposure);
     //pCamera->ExposureTimeRaw.SetValue(exposure);  //not to use!
     cout << "Exposure set to: " << exposure << endl;  
   }    
   this->exposure=exposure;

   return SUCCESS;
}



int BASLER_ACA::setExposureAuto(char *exposureAuto)
{ 
  INodeMap& nodeMap = pCamera->GetNodeMap();
  CEnumerationPtr pFormat(nodeMap.GetNode("ExposureAuto"));

  if(IsAvailable( pFormat->GetEntryByName(exposureAuto)))             //OFF Once Continuous
  {
     pFormat->FromString(exposureAuto);
     cout << "Exposure Auto set to : " << pFormat->ToString() << endl;
  }
 
  return SUCCESS;
}


int BASLER_ACA::setGainAuto(char *gainAuto)
{
  INodeMap& nodeMap = pCamera->GetNodeMap();
  CEnumerationPtr pFormat(nodeMap.GetNode("GainAuto"));

  if(IsAvailable( pFormat->GetEntryByName(gainAuto)))             //OFF Once Continuous
  {
     pFormat->FromString(gainAuto);
     cout << "Gain Auto set to : " << pFormat->ToString() << endl;
  }
 
  return SUCCESS;
}

int BASLER_ACA::setGain(int gain)
{
    if (IsWritable(pCamera->GainRaw))
    {
     pCamera->GainRaw.SetValue(gain);
     cout << "Gain set to: " << gain << endl;
    }   
    this->gain = gain;

   return SUCCESS;
}

int BASLER_ACA::setGammaEnable(char *gammaEnable)
{
  if(strcmp(gammaEnable, "On")==0)
  {
    pCamera->GammaEnable.SetValue(true);
  }
  if(strcmp(gammaEnable, "Off")==0)
  {
    pCamera->GammaEnable.SetValue(false);
  }
   
  cout << "Gamma Enable set to: " << gammaEnable << endl;  
  return SUCCESS;
}

int BASLER_ACA::setFrameRate(double frameRate)
{
    pCamera->AcquisitionFrameRateEnable.SetValue(true);
    pCamera->AcquisitionFrameRateAbs.SetValue(frameRate);
    cout << "Frame Rate set to: " << frameRate << endl;   
    this->frameRate = frameRate;
    return SUCCESS;
}


int BASLER_ACA::setReadoutArea(int x, int y, int width, int height)
{
   INodeMap& nodeMap = pCamera->GetNodeMap();

   try
   {
    CIntegerPtr offsetX( nodeMap.GetNode("OffsetX") );
    CIntegerPtr offsetY( nodeMap.GetNode("OffsetY") );
    CIntegerPtr widthCI( nodeMap.GetNode("Width") );
    CIntegerPtr heightCI( nodeMap.GetNode("Height") );

    int oldOffsetX = offsetX->GetValue(); //if ERROR recover old value
    int oldOffsetY = offsetY->GetValue();

    
    if (IsWritable(offsetX))   // On some cameras the offsets are read-only,
    {
        offsetX->SetValue( offsetX->GetMin() );
    }
    if (IsWritable(offsetY))
    {
       offsetY->SetValue( offsetY->GetMin() );
    }

    if((x+width) > widthCI->GetMax())   //GetMax depend on previous offset set so now is the maximum
    {
     printf("ERROR in setReadoutArea: OffsetX + Width exceed maximum allowed.");
     offsetX->SetValue( oldOffsetX );
     lastOpRes=ERROR;
     return ERROR;
    }
    if((y+height) > heightCI->GetMax())
    {
     printf("ERROR in setReadoutArea: OffsetY + Height exceed maximum allowed.");
     offsetY->SetValue( oldOffsetY );
     lastOpRes=ERROR;
     return ERROR;
    }

    widthCI->SetValue(width);
    heightCI->SetValue(height);
    offsetX->SetValue( x );
    offsetY->SetValue( y );

    cout << "OffsetX          : " << offsetX->GetValue() << endl;
    cout << "OffsetY          : " << offsetY->GetValue() << endl;

    cout << "Width            : " << widthCI->GetValue() << endl;
    cout << "Height           : " << heightCI->GetValue() << endl;

    return SUCCESS;
  }
  catch (const GenericException & e)
  {
     cerr << "Error in setReadoutArea. Reason: " << e.GetDescription() << endl;
     PylonTerminate();
     lastOpRes=ERROR;
  }
  return ERROR;
}


int BASLER_ACA::setPixelFormat(char *pixelFormat)
{
  INodeMap& nodeMap = pCamera->GetNodeMap();
  CEnumerationPtr pFormat(nodeMap.GetNode("PixelFormat"));
  if(IsAvailable( pFormat->GetEntryByName(pixelFormat)))             //Mono8 - BayerRG8
  {
     pFormat->FromString(pixelFormat);
     cout << "PixelFormat set to : " << pFormat->ToString() << endl; 
     if(strcmp(pixelFormat, "Mono8")==0)
     {
       this->pixelFormat = CSU_PIX_FMT_GRAY8;
       this->Bpp = 1;
     }
     if(strcmp(pixelFormat, "Mono12")==0)
     {
       this->pixelFormat = CSU_PIX_FMT_GRAY16;
       this->Bpp = 2;
     }
     if(strcmp(pixelFormat, "BayerRG8")==0)
     {
       this->pixelFormat = CSU_PIX_FMT_BAYER_RGGB8;
       this->Bpp = 1;
     }
     if(strcmp(pixelFormat, "YUV422Packed")==0)
     {
       this->pixelFormat = CSU_PIX_FMT_YUV422_Packed;
       this->Bpp = 2;
     }
     return SUCCESS;
 }
 else
 {
    printf("The camera does NOT support the selected Pixel Format.\n");
    throw RUNTIME_EXCEPTION("The camera does NOT support the selected Pixel Format.");
 }
 return ERROR;
}


int BASLER_ACA::startAcquisition(int *width, int *height, int *payloadSize)
{
   INodeMap& nodeMap = pCamera->GetNodeMap();
   INode *node;

   //get width & height
   this->width = *width = CIntegerPtr(nodeMap.GetNode("Width"))->GetValue();   this->height = *height= CIntegerPtr(nodeMap.GetNode("Height"))->GetValue();  
			
   *payloadSize=this->width*this->height*this->Bpp;  //no metadata

   printf("width=%d.", this->width); 
   printf("height=%d.", this->height);
   printf("Bpp=%d.\n", this->Bpp);

   //fede new 30/06/2017 due to incomplete grabbed frame error. The error advice to change iter-packed delay
   pCamera->GevStreamChannelSelector.SetValue(GevStreamChannelSelector_StreamChannel0);
   pCamera->GevSCPD.SetValue(10000);  //1 tick should be 8ns from manual. So inter packed delay is now 80us.
   //int64_t i = camera.GevSCPD.GetValue();

   // Enable chunks in general.
   node = nodeMap.GetNode("ChunkModeActive");
   if (IsWritable(node))
   {
     CBooleanPtr(node)->SetValue(true);
   }
   else
   {
     throw RUNTIME_EXCEPTION( "The camera doesn't support chunk features");
   }
   // Enable time stamp chunks.
   CEnumerationPtr ChunkSelector(nodeMap.GetNode("ChunkSelector"));
   if(IsAvailable(ChunkSelector->GetEntryByName("Timestamp")))
   {
     ChunkSelector->FromString("Timestamp");
   }
   node = nodeMap.GetNode("ChunkEnable");
   CBooleanPtr(node)->SetValue(true);

   if(IsAvailable(ChunkSelector->GetEntryByName("PayloadCRC16")))
   {
     ChunkSelector->FromString("PayloadCRC16");
   }
   node = nodeMap.GetNode("ChunkEnable");
   CBooleanPtr(node)->SetValue(true);

   //new fede 20170918
   if(IsAvailable(ChunkSelector->GetEntryByName("ExposureTime")))
   {
     ChunkSelector->FromString("ExposureTime");
     cout << "New ChunkSelector_ExposureTime: " << ChunkSelector->ToString() << endl;
   }
   node = nodeMap.GetNode("ChunkEnable");
   CBooleanPtr(node)->SetValue(true);

   if(IsAvailable(ChunkSelector->GetEntryByName("GainAll")))
   {
     ChunkSelector->FromString("GainAll");
     cout << "New ChunkSelector_GainAll: " << ChunkSelector->ToString() << endl;
   }
   node = nodeMap.GetNode("ChunkEnable");
   CBooleanPtr(node)->SetValue(true);

   //  CInstantCamera camera( device );
   //  static const uint32_t c_countOfImagesToGrab = 100;

   // The parameter MaxNumBuffer can be used to control the count of buffers
   // allocated for grabbing. The default value of this parameter is 10.
   pCamera->MaxNumBuffer = 20;

   // Start the grabbing of c_countOfImagesToGrab images.
   // The camera device is parameterized with a default configuration which
   // sets up free-running continuous acquisition.
   // pCamera->StartGrabbing( c_countOfImagesToGrab);
   pCamera->StartGrabbing();

   return SUCCESS;	
}
  


int BASLER_ACA::stopAcquisition()
{
    pCamera->StopGrabbing();
    printf("Stop Acquisition\n");
    return SUCCESS;
}


int BASLER_ACA::getFrame(int *status, void *frame, void *metaData)
{
   // This smart pointer will receive the grab result data.
   CGrabResultPtr ptrGrabResult;

   if(pCamera->IsGrabbing())
   {
      //printf("getframe is grabbing\n");
      // Wait for an image and then retrieve it. A timeout of 5000 ms is used.
      try{
         pCamera->RetrieveResult( 5000, ptrGrabResult, TimeoutHandling_ThrowException);
      }catch (const GenericException &e) 
      {
        // Error handling.
        cerr << "An exception occurred." << endl << e.GetDescription() << endl;
       // exitCode = 1;
        *status=3; //timeout
        printf("-> 5s timeout reached in getFrame\n");
      }     
      if (ptrGrabResult->GrabSucceeded()) // Image grabbed successfully
      {
          *status=1; //complete
	  unsigned int width = ptrGrabResult->GetWidth();
 	  unsigned int height = ptrGrabResult->GetHeight();
	  const uint8_t *dataPtr = (uint8_t *) ptrGrabResult->GetBuffer();     //use always char* also 4 bigger images
          memcpy( frame , (unsigned char *)dataPtr, width*height*this->Bpp );

         // cout << "Gray value of first pixel: " << (uint32_t) dataPtr[0] << endl << endl;

            // Check to see if a buffer containing chunk data has been received.
            if (PayloadType_ChunkData != ptrGrabResult->GetPayloadType())
            {
                throw RUNTIME_EXCEPTION( "Unexpected payload type received.");
            }
            // Since we have activated the CRC Checksum feature, we can check
            // the integrity of the buffer first.
            // Note: Enabling the CRC Checksum feature is not a prerequisite for using
            // chunks. Chunks can also be handled when the CRC Checksum feature is deactivated.
            if (ptrGrabResult->HasCRC() && ptrGrabResult->CheckCRC() == false)
            {
                throw RUNTIME_EXCEPTION( "Image was damaged!");
            }

           int64_t ts;
           if(ptrGrabResult->IsChunkDataAvailable())
           {
             INodeMap& nodeMap = ptrGrabResult->GetChunkDataNodeMap();
             ts = CIntegerPtr(nodeMap.GetNode("ChunkTimestamp"))->GetValue();
             // printf("%I64d\n", ts);
           }

           int gain=0;
           if(ptrGrabResult->IsChunkDataAvailable())
           {
             INodeMap& nodeMap = ptrGrabResult->GetChunkDataNodeMap();
             gain = CIntegerPtr(nodeMap.GetNode("ChunkGainAll"))->GetValue();
             //printf("fede gainall chunk:%d\n", gain);
           }
           double exp=0;
           if(ptrGrabResult->IsChunkDataAvailable())
           {
             INodeMap& nodeMap = ptrGrabResult->GetChunkDataNodeMap();
             exp = CFloatPtr(nodeMap.GetNode("ChunkExposureTime"))->GetValue();
             //printf("fede exp chunk:%f\n", exp);
           }

        //printf("metadata size: %d\n", sizeof(BASLERMETADATA));
        BASLERMETADATA bMeta;
        bMeta.gain=gain;
        bMeta.exposure=exp;
        bMeta.internalTemperature=this->internalTemperature;
        bMeta.timestamp=ts;
        memcpy( metaData , (unsigned char *)&bMeta, sizeof(BASLERMETADATA));

       }// if (ptrGrabResult->GrabSucceeded()
       else
       {
         cout << "Grab Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << endl;
       }
    }//if(camera.IsGrabbing())
}


int BASLER_ACA::setStreamingMode( int streamingEnabled,  bool autoAdjustLimit, const char *streamingServer, int streamingPort, unsigned int lowLim, unsigned int highLim, int adjRoiX, int adjRoiY, int adjRoiW, int adjRoiH, const char *deviceName)
{
   this->streamingEnabled = streamingEnabled;
	
   if( streamingEnabled )
   {
	memcpy( this->streamingServer, streamingServer, strlen(streamingServer)+1 );
	memcpy( this->deviceName, deviceName, strlen(deviceName)+1 );
	this->streamingPort = streamingPort;
	this->autoAdjustLimit = autoAdjustLimit;

	this->lowLim = lowLim;
	this->highLim = highLim;

	this->adjRoiX = adjRoiX;
	this->adjRoiY = adjRoiY;
	this->adjRoiW = adjRoiW;
	this->adjRoiH = adjRoiH;
          		        
        this->minLim= 0;            
        this->maxLim= 4096; //max value for 12bit resolution of BASLER
   }
   return SUCCESS;
} 


int BASLER_ACA::setAcquisitionMode( int storeEnabled , int acqSkipFrameNumber)
{

   this->storeEnabled = storeEnabled;
   this->acqSkipFrameNumber = acqSkipFrameNumber;
   return SUCCESS;
}


int BASLER_ACA::setTriggerMode( int triggerMode, double burstDuration, int numTrigger )
{
	this->triggerMode = triggerMode;
	this->burstDuration = burstDuration;
	this->numTrigger = numTrigger;

        return SUCCESS; 
}

int BASLER_ACA::softwareTrigger()
{
	this->startStoreTrg = 1; 
	return SUCCESS;
}

int BASLER_ACA::setTreeInfo( void *treePtr, int framesNid, int timebaseNid, int framesMetadNid, int frame0TimeNid)
{
	this->treePtr = treePtr ;
	this->framesNid = framesNid;
	this->timebaseNid = timebaseNid;
	this->framesMetadNid = framesMetadNid;
        this->frame0TimeNid = frame0TimeNid;

	return SUCCESS;
}

/*
void FLIR_SC65X::printLastError(const char *format, const char *msg)
{
	error[0] = 0;
	if(msg)
		sprintf(error, format, msg);
	else
		sprintf(error, format);
}

*/
void BASLER_ACA::getLastError(char *msg)
{
	sprintf(msg, "%s", (error[0]==0) ? "" : error);
}

int BASLER_ACA::stopFramesAcquisition()
{
	int count = 0;

	if (acqFlag == 0)
		return SUCCESS;
 
	acqStopped = 0;
	acqFlag = 0;
	while( !acqStopped & count < 10 )
	{
		count++;
		usleep(5000);
	}

	if(count == 10)
	{
		printf("Cannot stop acquisition loop\n");
		return ERROR;
	}
		
	return SUCCESS;
}

int BASLER_ACA::startFramesAcquisition()
{
	int frameTriggerCounter;
	int frameCounter;
	int frameStatus;

	int NtriggerCount = 0;
	int burstNframe;
	int rstatus;
	int tcpStreamHandle = -1;

	int metaSize;
	int enqueueFrameNumber;
	float frameTime = 0.0;

	void *saveList;
	void *streamingList;

        void *frameBuffer;
	unsigned char *metaData;
	unsigned char *frame8bit;

        struct timeval tv;  //manage frame timestamp in internal mode
        int64_t timeStamp;
        int64_t timeStamp0;     
        TreeNode *t0Node;
        try{
             t0Node = new TreeNode(frame0TimeNid, (Tree *)treePtr);
             Data *nodeData = t0Node->getData();
             timeStamp0 = (int64_t)nodeData->getLong();
        }catch(MdsException *exc)
         {
            printf("Error getting frame0 time\n");
         }

        if(this->Bpp==1)
        {
          frameBuffer = (char *) calloc(1, width * height * sizeof(char));
        }
        if(this->Bpp==2)
        {
          frameBuffer = (short *) calloc(1, width * height * sizeof(short));
        }
	frame8bit = (unsigned char *) calloc(1, width * height * sizeof(char));

        metaSize = sizeof(BASLERMETADATA);
	metaData = (unsigned char *)calloc(1, metaSize);
 
        camStartSave(&saveList); //  # Initialize save frame Linked list reference
   	camStartStreaming(&streamingList); //  # Initialize streaming frame Linked list reference
	burstNframe = (int) ( burstDuration * frameRate + 1 );
	acqFlag = 1;
	frameTriggerCounter = 0;
	frameCounter = 0;
        incompleteFrame = 0;
	enqueueFrameNumber = 0;
	startStoreTrg = 0;  //manage the mdsplus saving process. SAVE always start with a SW or HW trigger. (0=no-save; 1=save)

        while ( acqFlag )
	{      
        getFrame( &frameStatus, frameBuffer, metaData);   //get the frame

        if(storeEnabled)
        {
          if ( triggerMode == 1 )        // External trigger source
	  {

           	if ( (frameStatus == 4) && (startStoreTrg == 0) )       //start data storing @ 1st trigger seen (trigger is on image header!)
		{
            	  startStoreTrg = 1;
            	  printf("TRIGGERED:\n");	
		}

           	if (frameTriggerCounter == burstNframe) 
		{
		  triggered = 0;
		  startStoreTrg   = 0;   //disable storing                  
		  NtriggerCount++; 
    
                  printf("ACQUIRED ALL FRAMES %d FOR TRIGGER : %d\n", frameTriggerCounter,  NtriggerCount );	
                  frameTriggerCounter = 0;

	          if ( NtriggerCount == numTrigger ) //stop store when all trigger will be received
		  { 
	            printf("ACQUIRED ALL FRAME BURST: %d\n", numTrigger );
                    storeEnabled=0;	
	            //break;             
		  }
		 }//if (frameTriggerCounter == burstNframe) 

          } 
          else //( triggerMode == 1 ) 	//Internal trigger source
          { 
               //Multiple trigger acquisition: first trigger save 64bit timestamp
               timebaseNid = -1;  //used in cammdsutils to use internal      
	       triggered = 1; //debug
               if(startStoreTrg == 1)
               {
                  gettimeofday(&tv, NULL); 				  
                  timeStamp = ((tv.tv_sec)*1000) + ((tv.tv_usec)/1000); // timeStamp [ms]

                  if(timeStamp0==0)
                  {           
                    Int64 *tsMDS = new Int64(timeStamp);
                    t0Node->putData(tsMDS);
                    timeStamp0=timeStamp; 
                  }
                  else
                  {   
                    frameTime = (float)((timeStamp-timeStamp0)/1000.0); //interval from first frame [s]
                   // printf("frameTime: %f", frameTime);     
                  }
              }//if startStoreTrg == 1 

       	      if ( frameTriggerCounter == burstNframe )
              {
                   startStoreTrg   = 0;   //disable storing   
                   frameTriggerCounter = 0;
                   NtriggerCount++; 
            	   printf("Stop Internal trigger acquisition time:%f dur:%f fps:%f\n", frameTime, burstDuration, frameRate);
                   //storeEnabled=0;  //infinite trigger until stop acquisition
		   //break;
              }
	  }//else Internal trigger source
        }//if(storeEnabled)

 
	//frameStatus -> status=1 complete # status=2 incomplete # status=3 timeout # status=4 triggered frame + complete
	if( (frameStatus != 3 ) && ( storeEnabled == 1 && startStoreTrg == 1 ) && ( acqSkipFrameNumber <= 0 || (frameTriggerCounter % (acqSkipFrameNumber + 1) ) == 0 ) )
	{
	  int frameTimeBaseIdx;
	  frameTimeBaseIdx = NtriggerCount * burstNframe + frameTriggerCounter;
	  //printf("SAVE Frame : %d timebase Idx : %d\n", frameTriggerCounter,  frameTimeBaseIdx);
	
	  // CT la routine camSaveFrame utilizza il frame index in acquisizione. L'indice viene
    	  // utilizzato per individuare nell'array della base temporale il tempo associato al frame.

	  // Con Trigger interno viene utilizzato frameTime come tempo relativo allo 0; timebaseNid deve essere -1
//          camSaveFrame((void *)frameBuffer, width, height, frameTime, 14, (void *)treePtr, framesNid, timebaseNid, frameTimeBaseIdx, (void *)metaData, metaSize, framesMetadNid, saveList); 
                              
          camSaveFrame((void *)frameBuffer, width, height, frameTime, 8*this->Bpp, (void *)treePtr, framesNid, timebaseNid, frameTimeBaseIdx, (void *)metaData, metaSize, framesMetadNid, saveList); 
	  enqueueFrameNumber++;

	} 

        if( streamingEnabled )
	{
           if( tcpStreamHandle == -1) 
	   {                             
            	rstatus = camOpenTcpConnection(streamingServer, streamingPort, &tcpStreamHandle, width, height, pixelFormat);
            	if( rstatus !=-1 )
                {
            	  printf( "Connected to FFMPEG on %s : %d\n", streamingServer, streamingPort);
                }
		else
		{
            	  printf( "Cannot connect to FFMPEG on %s : %d. Disable streaming\n", streamingServer, streamingPort);
		  streamingEnabled = 0;
		}
	    }

	    //if ( (streamingSkipFrameNumber - 1 <= 0) || (frameCounter % ( streamingSkipFrameNumber - 1)) == 0 )  //20170327 - ORIGINAL
            else if((this->frameRate<10) || (frameCounter % int(this->frameRate/10.0))==0)  //send frame @ 10Hz. Reduce CPU usage when radiometric conversion must be performed.
	    {
 	        camStreamingFrame( tcpStreamHandle, frameBuffer, width, height, pixelFormat, 0, autoAdjustLimit, &lowLim, &highLim, minLim, maxLim, adjRoiX, adjRoiY, adjRoiW, adjRoiH, this->deviceName, streamingList);
	    }             
	} // if( streamingEnabled )
        frameCounter++;           //never resetted, used for frame timestamp     
        if ( startStoreTrg == 1 ) //increment saved frame index only if acquisition has been triggered
        {
          frameTriggerCounter++;     
        }
    }//endwhile

    camStopSave(saveList); // Stop asynhronous store stream
    camStopStreaming(streamingList); // Stop asynhronous frame streaming

    if( tcpStreamHandle != -1 )
      camCloseTcpConnection(&tcpStreamHandle);  

    rstatus = stopAcquisition();  //stop camera acquisition
    if (rstatus < 0)
	printf("Cannot stop camera acquisition\n");
   
    free(frameBuffer);
    free(frame8bit);
    free(metaData);

	printf("Acquisition Statistics : \tTotal frames read %d, \n\t\t\t\tTotal frames stored %d (expected %d), \n\t\t\t\tNumber of trigger %d (expected %d), \n\t\t\t\tIncomplete frame %d\n", frameCounter, enqueueFrameNumber, 1 + numTrigger * ((int)( burstDuration * (frameRate-acqSkipFrameNumber))), NtriggerCount, numTrigger, incompleteFrame );

	acqStopped = 1;

	return rstatus;
}


