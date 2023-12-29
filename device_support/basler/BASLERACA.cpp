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
#include <vector>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <mdsobjects.h>
#include <string>
#include <iostream>
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
     memset(error, 0, sizeof(error));
     memcpy(this->ipAddress, ipAddress, strlen(ipAddress)+1);
     this->pCamera = NULL; 
     this->trigEvent = NULL;

     //printf("\n Init COUNT = %d %d\n", this->getCount());
     if( this->getCount() == 1)
     {     
        printf("\n\n%s:Pylon Inizialize.\n\n",this->ipAddress);
        PylonInitialize();
     }

     CTlFactory& TlFactory = CTlFactory::GetInstance();
     CDeviceInfo di;

     char *pos;
     pos = strchr((char *)ipAddress, '.'); 		//if dots are present -> ipAddr -> ipCamera. ELSE usb camera.
     if (pos!=NULL)
     {
        printf("Try to open IP camera %s\n", ipAddress);
        di.SetIpAddress(ipAddress);
        di.SetDeviceClass(BaslerGigEDeviceClass);
     }
     else
     {     
        printf("Try to open USB camera %s\n", ipAddress);  //ip address used as the serial number in USB cameras
        di.SetSerialNumber(ipAddress);
        di.SetDeviceClass(BaslerUsbDeviceClass);
     }

     this->pDevice = TlFactory.CreateDevice(di);
     this->pCamera = new CBaslerUniversalInstantCamera(this->pDevice); 

     pCamera->Open();
     printf("%s: Device Connected.\n", this->ipAddress); 
     lastOpRes=SUCCESS;

     if(pCamera->IsGigE())  //20231201: after adding USB cameras support, GIGE do NOT work anymore! This seems to fix the problem...
     {
        printf("%s: Stream Grabber Transmission set to Multicast.\n", this->ipAddress); 
        pCamera->GetStreamGrabberParams().TransmissionType = TransmissionType_Multicast;
        //pCamera->GetStreamGrabberParams().DestinationAddr = "192.168.1.X";
     }

     printf("EVENTO abilitato \n");
     //CT MDSplus acquisition Triggered on MDSevent 
     eventTrigger = 0;
     trigEvent = new TriggerMdsEvent("__CCD_TRIGGER__", this);
     trigEvent->start();

   }
   catch (const GenericException & e)
   {
     cerr << this->ipAddress << ": Error Device connection. Reason: " << e.GetDescription() << endl;
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
//   printf("COUNT = %d %d\n", this->getCount(), pCamera);
/*
   if ( pCamera != NULL && pCamera->IsOpen() )
        pCamera->Close();
*/
   if( this->getCount() == 1)
   {
      printf("PylonTerminate.\n");
      PylonTerminate();
   }
   printf("%s: Device Disconnected.\n", this->ipAddress);

   if(this->trigEvent != NULL)
      delete trigEvent;
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
  double t;
  INodeMap& nodeMap = pCamera->GetNodeMap();

  try
  {
   if(IsAvailable(nodeMap.GetNode("TemperatureSelector")))  // 20231128: GIGE cameras
   {
    pCamera->TemperatureSelector.SetValue(TemperatureSelector_Coreboard);
    t = pCamera->TemperatureAbs.GetValue();
   }
   else if(IsAvailable(nodeMap.GetNode("DeviceTemperatureSelector")))  // 20231128: USB cameras	
   {
    pCamera->DeviceTemperatureSelector.SetValue(DeviceTemperatureSelector_Coreboard);
    t = pCamera->DeviceTemperature.GetValue();
   }
   else
   {
    printf("%s: readInternalTemperature error.\n", this->ipAddress);
   }
  }
  catch (const GenericException & e)
  {
     cerr << "Error in readInternalTemperature(). Reason: " << e.GetDescription() << endl;
     lastOpRes=ERROR;
     return ERROR;
  }
  
  printf("%s: Camera Temperature is now %3.2f°C\n", this->ipAddress, t); 
  this->internalTemperature=t;

  return SUCCESS;
}



int BASLER_ACA::setExposure(double exposure)
{  
   if (IsWritable(pCamera->ExposureTimeAbs)) // 20231128: GIGE cameras
   {
     pCamera->ExposureTimeAbs.SetValue(exposure);
     cout << this->ipAddress << ": Exposure set to: " << exposure << endl;  
   }  

   if (IsWritable(pCamera->ExposureTime)) // 20231128: USB cameras
   {
     pCamera->ExposureTime.SetValue(exposure);
     cout << this->ipAddress << ": Exposure set to: " << exposure << endl;  
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
     cout << this->ipAddress << ": Exposure Auto set to : " << pFormat->ToString() << endl;
  }
 
  return SUCCESS;
}


int BASLER_ACA::setGainAuto(char *gainAuto)
{
  INodeMap& nodeMap = pCamera->GetNodeMap();
  CEnumerationPtr pFormat(nodeMap.GetNode("GainAuto"));

  if(IsAvailable(pFormat->GetEntryByName(gainAuto)))             //OFF Once Continuous
  {
     pFormat->FromString(gainAuto);
     cout << this->ipAddress << ": Gain Auto set to : " << pFormat->ToString() << endl;
  }
 
  return SUCCESS;
}

int BASLER_ACA::setGain(int gain)
{
    if (IsWritable(pCamera->GainRaw)) //20231130: GIGE cameras has GainRaw
    {
     pCamera->GainRaw.SetValue(gain);
     cout << this->ipAddress << ": Gain set to: " << gain << endl;
    }   

   if (IsWritable(pCamera->Gain))  //20231130: USB cameras has Gain [dB]
   {
     pCamera->Gain.SetValue(gain);
     cout << this->ipAddress << ": Gain set to: " << gain << endl;
   }   

    this->gain = gain;

   return SUCCESS;
}

int BASLER_ACA::setGammaEnable(char *gammaEnable)   
{  
  INodeMap& nodeMap = pCamera->GetNodeMap();
  INode *node;
  node = nodeMap.GetNode("GammaEnable");

  if (IsImplemented(node) && IsAvailable(node))   //20231030 fede: in usb cameras GammaEnable do NOT exists
  {
     CEnumParameter gammaEn(node);
     gammaEn.TrySetValue(gammaEnable);
     cout << this->ipAddress << ": Gamma Enable set to: " << gammaEnable << endl; 
  }
  else
  {
     cout << this->ipAddress << ": Gamma Enable is NOT available" << endl;
  }

  return SUCCESS;
}

int BASLER_ACA::setFrameRate(double frameRate)
{
   INodeMap& nodeMap = pCamera->GetNodeMap();  
   INode *node;

   node = nodeMap.GetNode("AcquisitionFrameRateEnable");
   if (IsImplemented(node) && IsAvailable(node))
   {
     pCamera->AcquisitionFrameRateEnable.SetValue(true); 
   }
   else
   {
     cout << this->ipAddress << ": AcquisitionFrameRateEnable is NOT available" << endl;
   }

   node = nodeMap.GetNode("AcquisitionFrameRateAbs");  //20231030 fede: GIGE cameras
   if (IsImplemented(node) && IsAvailable(node))
   {
     pCamera->AcquisitionFrameRateAbs.SetValue(frameRate); 
     cout << this->ipAddress << ": Frame Rate Abs set to: " << frameRate << endl;  
   }
   else
   {
     node = nodeMap.GetNode("AcquisitionFrameRate");  //20231030 fede: USB cameras
     if (IsImplemented(node) && IsAvailable(node))
     {
       pCamera->AcquisitionFrameRate.SetValue(frameRate); 
       cout << this->ipAddress << ": Frame Rate set to: " << frameRate << endl; 
     }
     else
     {
       cout << this->ipAddress << ": AcquisitionFrameRate is NOT available" << endl;
     }
   }

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
     printf("%s: ERROR in setReadoutArea: OffsetX + Width exceed maximum allowed.", this->ipAddress);
     offsetX->SetValue( oldOffsetX );
     lastOpRes=ERROR;
     return ERROR;
    }
    if((y+height) > heightCI->GetMax())
    {
     printf("%s: ERROR in setReadoutArea: OffsetY + Height exceed maximum allowed.", this->ipAddress);
     offsetY->SetValue( oldOffsetY );
     lastOpRes=ERROR;
     return ERROR;
    }

    widthCI->SetValue(width);
    heightCI->SetValue(height);
    offsetX->SetValue( x );
    offsetY->SetValue( y );

    cout << this->ipAddress << ": OffsetX          : " << offsetX->GetValue() << endl;
    cout << this->ipAddress << ": OffsetY          : " << offsetY->GetValue() << endl;

    cout << this->ipAddress << ": Width            : " << widthCI->GetValue() << endl;
    cout << this->ipAddress << ": Height           : " << heightCI->GetValue() << endl;

    return SUCCESS;
  }
  catch (const GenericException & e)
  {
     cerr << "Error in setReadoutArea. Reason: " << e.GetDescription() << endl;
     lastOpRes=ERROR;
  }
  return ERROR;
}


int BASLER_ACA::setPixelFormat(char *pixelFormat)
{
  INodeMap& nodeMap = pCamera->GetNodeMap();
  CEnumerationPtr pFormat(nodeMap.GetNode("PixelFormat"));
  try
  {
  if(IsAvailable( pFormat->GetEntryByName(pixelFormat))==false) //if not available set to Mono12 or Mono8
  {
    if(IsAvailable( pFormat->GetEntryByName("Mono12")))	
    {
      pFormat->FromString("Mono12");
      cout << this->ipAddress << ": (WARNING) Selected PixelFormat is NOT available. Pixel format set to Mono12" << endl; 
      this->pixelFormat = CSU_PIX_FMT_GRAY16;
      this->Bpp = 2;
      return SUCCESS;
    }
    else
    {
      pFormat->FromString("Mono8");
      cout << this->ipAddress << ": (WARNING) Selected PixelFormat is NOT available. PixelFormat set to Mono8" << endl; 
      this->pixelFormat = CSU_PIX_FMT_GRAY8;
      this->Bpp = 1;
      return SUCCESS;
    }    
  }
  else
  {
     pFormat->FromString(pixelFormat);
     cout << this->ipAddress << ": PixelFormat set to : " << pFormat->ToString() << endl; 
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
 }catch (const GenericException &e)
     {
       cout << this->ipAddress << ": DEBUG TO REMOVE exception in pixelFormat " << endl;
     }
 return ERROR;
}


int BASLER_ACA::startAcquisition(int *width, int *height, int *payloadSize)
{
   INodeMap& nodeMap = pCamera->GetNodeMap();
   INode *node;

   //get width & height
   this->width = *width = CIntegerPtr(nodeMap.GetNode("Width"))->GetValue();
   this->height = *height= CIntegerPtr(nodeMap.GetNode("Height"))->GetValue();  
			
   *payloadSize=this->width*this->height*this->Bpp;  //no metadata

   printf("%s: width=%d height=%d Bpp=%d\n", this->ipAddress, this->width, this->height, this->Bpp); 

   if(pCamera->IsGigE()) // 20231030 fede: added USB support
   {
     node = nodeMap.GetNode("GevStreamChannelSelector");  
     if (IsImplemented(node) && IsAvailable(node))
     {
       //fede new 30/06/2017 due to incomplete grabbed frame error. The error advice to change iter-packed delay
       pCamera->GevStreamChannelSelector.SetValue(GevStreamChannelSelector_StreamChannel0);
       pCamera->GevSCPD.SetValue(10000);  //1 tick should be 8ns from manual. So inter packed delay is now 80us.
     }
   }  
  
   if(pCamera->IsUsb()) // 20231201 fede: added USB support
   {
    node = nodeMap.GetNode("DeviceLinkSelector");  //disable throughput limit in USB camera to manage high fps
    if (IsImplemented(node) && IsAvailable(node))
    {
      pCamera->DeviceLinkSelector.SetValue(0);
    }
    node = nodeMap.GetNode("DeviceLinkThroughputLimitMode"); 
    if (IsImplemented(node) && IsAvailable(node))
    {
      pCamera->DeviceLinkThroughputLimitMode.SetValue(DeviceLinkThroughputLimitMode_Off);
      printf("%s: Disabling DeviceLinkThroughputLimit... OK!\n", this->ipAddress);
    }
   }
 
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

   CEnumerationPtr ChunkSelector(nodeMap.GetNode("ChunkSelector"));
   if(IsAvailable(ChunkSelector->GetEntryByName("Timestamp")))
   {
     ChunkSelector->FromString("Timestamp");
     cout << this->ipAddress << ": Enabled ChunkSelector_Timestamp. " << endl;
     node = nodeMap.GetNode("ChunkEnable");
     CBooleanPtr(node)->SetValue(true);
   }

   if(IsAvailable(ChunkSelector->GetEntryByName("PayloadCRC16")))
   {
     ChunkSelector->FromString("PayloadCRC16");
     cout << this->ipAddress << ": Enabled ChunkSelector_PayloadCRC16. " << endl;
     node = nodeMap.GetNode("ChunkEnable");
     CBooleanPtr(node)->SetValue(true);
   }

   if(IsAvailable(ChunkSelector->GetEntryByName("ExposureTime")))
   {
     ChunkSelector->FromString("ExposureTime");
     cout << this->ipAddress << ": Enabled ChunkSelector_ExposureTime. " << endl;
     node = nodeMap.GetNode("ChunkEnable");
     CBooleanPtr(node)->SetValue(true);
   }

   if(IsAvailable(ChunkSelector->GetEntryByName("GainAll"))) //gain used in GIGE cameras
   {
     ChunkSelector->FromString("GainAll");
     cout << this->ipAddress << ": Enabled ChunkSelector_GainAll. "<< endl;
     node = nodeMap.GetNode("ChunkEnable");
     CBooleanPtr(node)->SetValue(true);
   }

   if(IsAvailable(ChunkSelector->GetEntryByName("Gain"))) //gain used in USB cameras
   {
     ChunkSelector->FromString("Gain");
     cout << this->ipAddress << ": Enabled ChunkSelector_Gain. " << endl;
     node = nodeMap.GetNode("ChunkEnable");
     CBooleanPtr(node)->SetValue(true);
   }


   //  CInstantCamera camera( device );
   //  static const uint32_t c_countOfImagesToGrab = 100;

   // The parameter MaxNumBuffer can be used to control the count of buffers
   // allocated for grabbing. The default value of this parameter is 10.
   pCamera->MaxNumBuffer = 20;
//   pCamera->MaxNumBuffer = 50;

   if(IsAvailable(nodeMap.GetNode("SensorReadoutTime")))  //available on USB cameras
   {
     double sRoT = pCamera->SensorReadoutTime.GetValue();
     printf("%s: calculated sensor readout time [us]: %f", this->ipAddress, sRoT); //useful to check when going on high speed
   }   

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
    printf("%s: Stop Acquisition\n", this->ipAddress);
    return SUCCESS;
}


int BASLER_ACA::getFrame(int *status, void *frame, void *metaData)
{
   // This smart pointer will receive the grab result data.
   CGrabResultPtr ptrGrabResult;

   if(pCamera->IsGrabbing())
   {
      // Wait for an image and then retrieve it. A timeout of 5000 ms is used.
      try{
         pCamera->RetrieveResult( 5000, ptrGrabResult, TimeoutHandling_ThrowException);
      }catch (const GenericException &e) 
      {
        // Error handling.
        cerr << this->ipAddress << "An exception occurred." << endl << e.GetDescription() << endl;
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

         // cout << this->ipAddress << ": Gray value of first pixel: " << (uint32_t) dataPtr[0] << endl << endl;

            // Check to see if a buffer containing chunk data has been received.
            if (PayloadType_ChunkData != ptrGrabResult->GetPayloadType())
            {
                throw RUNTIME_EXCEPTION( "Unexpected payload type received.");
            }
         
            //20231207 fede: CRC check slow down the acquisition and cannot be used at high fps rate
            if(pCamera->IsUsb())
            {
             if(this->frameRate<100)  //check CRC only below defined fps
             {
              if (ptrGrabResult->HasCRC() && ptrGrabResult->CheckCRC() == false)
              {
                throw RUNTIME_EXCEPTION( "Image was damaged!");
              }
             }
            }

           int64_t ts=0;
           double exp=0;
           double gain=0;
           int gainInteger=0;

           if(ptrGrabResult->IsChunkDataAvailable())
           {
             INodeMap& nodeMap = ptrGrabResult->GetChunkDataNodeMap();
             INode *nodeChunk;

             nodeChunk = nodeMap.GetNode("ChunkTimestamp");
             if (IsImplemented(nodeChunk) && IsAvailable(nodeChunk))
             {
              ts = CIntegerPtr(nodeChunk)->GetValue();
              //printf("chunk timestamp:%ld\n", ts); 
             }
         
             nodeChunk = nodeMap.GetNode("ChunkGainAll");    //GainAll is integer
             if (IsImplemented(nodeChunk) && IsAvailable(nodeChunk))  
             {
               gainInteger = CIntegerPtr(nodeChunk)->GetValue();
               //printf("chunk gainAll:%d\n", gainInteger); 
             }
             else
             {
               nodeChunk = nodeMap.GetNode("ChunkGain");    //Gain is double
               if (IsImplemented(nodeChunk) && IsAvailable(nodeChunk))  
               {
                 gain = CFloatPtr(nodeChunk)->GetValue();
                 gainInteger = (int)gain;
                 //printf("chunk gain:%f\n", gain); 
                 //printf("chunk gain integer:%d\n", gainInteger); 
               }
               else
               {
                 gainInteger=this->gain; 
                 //printf("gain chunk not available on this camera:%d\n", gainInteger); 
               }
             }

             nodeChunk = nodeMap.GetNode("ChunkExposureTime");
             if (IsImplemented(nodeChunk) && IsAvailable(nodeChunk)) 
             {
               exp = CFloatPtr(nodeChunk)->GetValue();
               //printf("chunk exposure:%f\n", exp);
             }
             else
             {
               exp=this->exposure; 
               //printf("exposure chunk not available on this camera:%f\n", exp); 
             }
           }

        //printf("metadata size: %d\n", sizeof(BASLERMETADATA));
        BASLERMETADATA bMeta;
        bMeta.gain=gainInteger;
        bMeta.exposure=exp;
        bMeta.internalTemperature=this->internalTemperature;
        bMeta.timestamp=ts;
        memcpy( metaData , (unsigned char *)&bMeta, sizeof(BASLERMETADATA));

       }// if (ptrGrabResult->GrabSucceeded()
       else
       {
         cout << this->ipAddress << ": Grab Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << endl;
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
	sprintf(msg, "%s:%s", this->ipAddress, (error[0]==0) ? "" : error);
}

int BASLER_ACA::stopFramesAcquisition()
{
	int count = 0;

	if (acqFlag == 0)
		return SUCCESS;
 
	acqStopped = 0;
	acqFlag = 0;
//	while( !acqStopped & count < 20 )
	while( !acqStopped & count < 100 ) //to stop fast USB camera acquisition (500 fps) bigger timeout is required
	{
		count++;
		usleep(50000);
	}

	if(count == 20)
	{
		sprintf(error, "%s: Cannot stop acquisition loop\n", this->ipAddress);
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
        float timeOffset = 0.0; //20180605

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
            sprintf(error, "%s: Error getting frame0 time\n", this->ipAddress);
         }
  
        //if ( triggerMode != 1 ) //in internal mode use the timebaseNid as T0 offset (ex. T_START_SPIDER)
        {                         //20210325: In external trigger, triggered on event must be set timeOffest
         TreeNode *tStartOffset;
         try{
             tStartOffset = new TreeNode(timebaseNid, (Tree *)treePtr);
             Data *nodeData = tStartOffset->getData();
             timeOffset = (float)nodeData->getFloatArray()[0];
         }catch(MdsException *exc)
          {
            sprintf(error, "%s: Error getting timebaseNid (offset time set to 0.0s)\n", this->ipAddress);
            timeOffset=0.0;
          }
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
	burstNframe = (int)(burstDuration * frameRate + 1. + 0.5);//CT 2021 03 20 
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

           	if ( (frameStatus == 4 || eventTrigger == 1 ) && (startStoreTrg == 0) )       //start data storing @ 1st trigger seen (trigger is on image header!)
		{                                                                             //CT In External Trigger Mode acquisitio is also triggered on MDSevent 
            	  startStoreTrg = 1;

                  if( eventTrigger == 1 )//For debug in Event trigger mode is disable time base and use local time frame time stamp
                  {
                      timebaseNid = -1;
                      if(NtriggerCount == 0 ) timeStamp0 = 0;
                  }

                  eventTrigger = 0;                                                            //CT Reset MDSplus trigger event flag
            	  printf("%s: TRIGGERED:\n", this->ipAddress);	
		}

/* Time stamp for acquisition in Event trigger */
                if(startStoreTrg == 1)
                {
                    gettimeofday(&tv, NULL); 				  
                    timeStamp = ((tv.tv_sec)*1000) + ((tv.tv_usec)/1000); // timeStamp [ms]

                    if(timeStamp0==0)
                    {           
                        Int64 *tsMDS = new Int64(timeStamp);
                        t0Node->putData(tsMDS);
                        timeStamp0=timeStamp;
                        printf("frameTime stamp 0: %ld", timeStamp0); 
                    }
                    else
                    {   
                        frameTime = (float)((timeStamp-timeStamp0)/1000.0); //interval from first frame [s]  
                        //printf("frameTime: %f", frameTime);      
                    }
                }//if startStoreTrg == 1 

/*************************/

           	if (frameTriggerCounter == burstNframe) 
		{
		  triggered = 0;
		  startStoreTrg   = 0;   //disable storing                  
		  NtriggerCount++; 
    
                  printf("%s: ACQUIRED ALL FRAMES %d FOR TRIGGER : %d\n", this->ipAddress, frameTriggerCounter,  NtriggerCount );	
                  frameTriggerCounter = 0;

	          if ( NtriggerCount == numTrigger ) //stop store when all trigger will be received
		  { 
	            printf("%s: ACQUIRED ALL FRAME BURST: %d\n", this->ipAddress, numTrigger );
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
                    //printf("frameTime: %f", frameTime);      
                  }
              }//if startStoreTrg == 1 

       	      if ( frameTriggerCounter == burstNframe )
              {
                   startStoreTrg   = 0;   //disable storing   
                   frameTriggerCounter = 0;
                   NtriggerCount++; 
            	   printf("%s: Stop Internal trigger acquisition time:%f dur:%f fps:%f\n", this->ipAddress, frameTime, burstDuration, frameRate);
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
	
	  // CT: routine camSaveFrame uses frame index in acquisition. Index is used to determine the frame timestamp using the base time array

	  // In internal trigger, frameTime is the relative time to the t0 (time of first frame); timebaseNid must be -1
//          camSaveFrame((void *)frameBuffer, width, height, frameTime, 14, (void *)treePtr, framesNid, timebaseNid, frameTimeBaseIdx, (void *)metaData, metaSize, framesMetadNid, saveList); 
          //printf("%s: SAVE FRAME: %d of %d frame time %f\n", this->ipAddress, frameTriggerCounter+1, burstNframe, frameTime+timeOffset);                              

          camSaveFrame((void *)frameBuffer, width, height, frameTime+timeOffset, 8*this->Bpp, (void *)treePtr, framesNid, timebaseNid, frameTimeBaseIdx, (void *)metaData, metaSize, framesMetadNid, saveList); 
	  enqueueFrameNumber++;

	} 

        if( streamingEnabled )
	{
           if( tcpStreamHandle == -1) 
	   {                             
            	rstatus = camOpenTcpConnection(streamingServer, streamingPort, &tcpStreamHandle, width, height, pixelFormat);
            	if( rstatus !=-1 )
                {
            	  printf( "%s: Connected to FFMPEG on %s : %d\n", this->ipAddress, streamingServer, streamingPort);
                }
		else
		{
            	  printf( "%s, Cannot connect to FFMPEG on %s : %d. Disable streaming\n", this->ipAddress, streamingServer, streamingPort);
		  streamingEnabled = 0;
		}
	    }

	    //if ( (streamingSkipFrameNumber - 1 <= 0) || (frameCounter % ( streamingSkipFrameNumber - 1)) == 0 )  //20170327 - ORIGINAL
            else if((this->frameRate<10) || (frameCounter % int(this->frameRate/10.0))==0)  //send frame @ 10Hz. Reduce CPU usage.
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


    int numSavedFrame = camSavedFrame(saveList);
    int numBlackFrame = camBlackFrame(saveList);


    camStopSave(saveList); // Stop asynhronous store stream
    camStopStreaming(streamingList); // Stop asynhronous frame streaming

    if( tcpStreamHandle != -1 )
      camCloseTcpConnection(&tcpStreamHandle);  

    rstatus = stopAcquisition();  //stop camera acquisition
    if (rstatus < 0)
	sprintf(error,"%s: Cannot stop camera acquisition\n", this->ipAddress);
   
    free(frameBuffer);
    free(frame8bit);
    free(metaData);

    //printf("%s: Acquisition Statistics : Total frames read %d, \n\t\t\t\t\tTotal frames stored %d (expected %d), \n\t\t\t\t\tNumber of trigger %d (expected %d), \n\t\t\t\t\tIncomplete frame %d\n", this->ipAddress, frameCounter, enqueueFrameNumber, numTrigger * ( (int)( burstDuration * (frameRate - acqSkipFrameNumber)) + 1), NtriggerCount + startStoreTrg, numTrigger, incompleteFrame );

    printf("%s: Acquisition Statistics : Total frames read %d, \n\t\t\t\t\tTotal frames stored %d (expected %d), \n\t\t\t\t\tTotal black frame %d, \n\t\t\t\t\tNumber of trigger %d (expected %d), \n\t\t\t\t\tIncomplete frame %d\n", this->ipAddress, frameCounter, numSavedFrame, numTrigger * ( (int)( burstDuration * (frameRate - acqSkipFrameNumber)) + 1), numBlackFrame, NtriggerCount + startStoreTrg, numTrigger, incompleteFrame );


    acqStopped = 1;

    return rstatus;
}


