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
using namespace MDSplus;
//using namespace std;

#include "PTGREY.h"

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
static PTGREY *camPtr[MAX_CAM] = {0};
static char errorOnOpen[512];


#ifdef __cplusplus 
extern "C" 
{ 
#endif 


int PGopen(const char *ipAddress, int *camHandle)
{
	PTGREY *cam;
	int cameraHandle;

	errorOnOpen[0] = 0;

	cam = new PTGREY(ipAddress);
	if(!cam->checkLastOp())
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


int isConnected( int camHandle )
{
	if( camHandle < 0 || camHandle >= MAX_CAM || camPtr[camHandle] == 0 ) 
	{
		sprintf(errorOnOpen, "Camera does not connect");
		return ERROR;
	}
	return SUCCESS;
}

int PGclose(int camHandle)
{
	delete(camPtr[camHandle]);
	camPtr[camHandle] = 0;
	
	return SUCCESS;
}


int checkLastOp(int camHandle)
{
	return camPtr[camHandle]->checkLastOp();
}


int printAllParameters(int camHandle)
{
	if( isConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->printAllParameters();
	return ERROR;
}


int readInternalTemperature(int camHandle)
{
  if( isConnected( camHandle ) == SUCCESS )
    return camPtr[camHandle]->readInternalTemperature();
  return ERROR;
}

int setExposure(int camHandle, float exposure)
{
  if( isConnected( camHandle ) == SUCCESS )
    return camPtr[camHandle]->setExposure(exposure);
  return ERROR;
}


int setExposureAuto(int camHandle, char *exposureAuto)
{
  if( isConnected( camHandle ) == SUCCESS )
    return camPtr[camHandle]->setExposureAuto(exposureAuto);
  return ERROR;
}


int setGain(int camHandle, float gain)
{
  if( isConnected( camHandle ) == SUCCESS )
    return camPtr[camHandle]->setGain(gain);
  return ERROR;
}


int setGainAuto(int camHandle, char *gainAuto)
{
  if( isConnected( camHandle ) == SUCCESS )
    return camPtr[camHandle]->setGainAuto(gainAuto);
  return ERROR;
}


int setGammaEnable(int camHandle, char *gammaEnable)
{
  if( isConnected( camHandle ) == SUCCESS )
    return camPtr[camHandle]->setGammaEnable(gammaEnable);
  return ERROR;
}


int setFrameRate(int camHandle, double *frameRate)
{
  if( isConnected( camHandle ) == SUCCESS )
  {
    double fRate;
    int status;
    fRate = *frameRate;
    status = camPtr[camHandle]->setFrameRate(fRate);
    *frameRate = fRate;
    return status;
  }
  return ERROR;
}


int setReadoutArea(int camHandle, int x, int y, int width, int height)
{
  if( isConnected( camHandle ) == SUCCESS )
    return camPtr[camHandle]->setReadoutArea(x, y, width, height);
  return ERROR;
}


int setPixelFormat(int camHandle, char *pixelFormat)
{
  if( isConnected( camHandle ) == SUCCESS )
    return camPtr[camHandle]->setPixelFormat(pixelFormat);
  return ERROR;
}


int startAcquisition(int camHandle, int *width, int *height, int *payloadSize)
{
	if( isConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->startAcquisition(width, height, payloadSize);
	return ERROR;
}


int stopAcquisition(int camHandle)
{
	if( isConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->stopAcquisition();
	return ERROR;
}


int getFrame(int camHandle, int *status, void *frame, void *metaData)
{
	if( isConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->getFrame(status, frame, metaData);
	return ERROR;
}


int setAcquisitionMode( int camHandle,  int storeEnabled,  int acqSkipFrameNumber)
{
	if( isConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->setAcquisitionMode(  storeEnabled, acqSkipFrameNumber);
	return ERROR;
}

int setTriggerMode( int camHandle, int triggerMode, double burstDuration, int numTrigger )
{
	if( isConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->setTriggerMode(  triggerMode,  burstDuration, numTrigger );
	return ERROR;
}

int softwareTrigger(int camHandle)
{
	if( isConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->softwareTrigger();
	return ERROR;
}

int setTreeInfo( int camHandle,  void *treePtr, int framesNid, int timebaseNid, int framesMetadNid, int frame0TimeNid)
{
	if( isConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->setTreeInfo( treePtr,  framesNid,  timebaseNid,  framesMetadNid, frame0TimeNid );
	return ERROR;
}

int setStreamingMode(int camHandle, int streamingEnabled, bool autoAdjustLimit, 
						const char *streamingServer, int streamingPort, int lowLim, int highLim, int adjRoiX, int adjRoiY, int adjRoiW, int adjRoiH, const char *deviceName)
{
	if( isConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->setStreamingMode(streamingEnabled,  autoAdjustLimit, streamingServer, streamingPort, lowLim, highLim, adjRoiX, adjRoiY, adjRoiW, adjRoiH, deviceName);
	return ERROR;

}
int startFramesAcquisition(int camHandle)
{
	if( isConnected( camHandle ) == SUCCESS )
	    return camPtr[camHandle]->startFramesAcquisition();
	return ERROR;
}

int stopFramesAcquisition(int camHandle)
{
	if( isConnected( camHandle ) == SUCCESS )
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




PTGREY::PTGREY(const char *ipAddress)
{
    this->ipAddress = PvString(ipAddress); 
    trigEvent = NULL;

/* SDK 3 
    this->lDevice = new PvDevice();
    this->lResult = this->lDevice->Connect(this->ipAddress, PvAccessExclusive);	
*/
    try{
         this->lDevice = PvDevice::CreateAndConnect(this->ipAddress, &this->lResult); //20160309 SDK4   //sometimes goes in coredump
    }
    catch(...)
    {
       printf("Core dump detected on PvDevice::CreateAndConnect\n");
    }
    if ( !this->lResult.IsOK() ) 
    {
        printf("--------------------------------- Error Device connection: (%s) %p\n", this->lResult.GetCodeString().GetAscii(),  this->lDevice); 
    	if( this->lDevice != nullptr ) //CT 2019-03-29
        {
        	PvDevice::Free(this->lDevice);
        }
    }
    else
    {    		
	printf("---------------------------- OK Device connection: (%s)\n", this->lResult.GetDescription().GetAscii() ); 
    }

     printf("EVENTO abilitato \n");
     //CT MDSplus acquisition Triggered on MDSevent 
     eventTrigger = 0;
     trigEvent = new TriggerMdsEvent("__CCD_TRIGGER__", this);
     trigEvent->start();

}


PTGREY::PTGREY()  //new 25/07/2013: let to use the device without the camera
{
    //this->lDevice = new PvDevice();     //SDK 3
    //SDK 4  09/03/2016  NON sembra possibile creare un PVDEVICE se non ho la telecamera
}


PTGREY::~PTGREY()
{

    printf("PTGREY destructor %p\n", this->lDevice );
    if( this->lDevice != nullptr ) //CT 2019-03-27
    {
	this->lResult = this->lDevice->Disconnect();
	if ( !this->lResult.IsOK() ) 
	    printLastError("Error Device disconnection !!!\n(%s)\n", lResult.GetDescription().GetAscii() ); 
	else
	    printf("PTGREY destructor class executed\n");
       //delete(this->lDevice); 	//SDK 3
	PvDevice::Free(this->lDevice);  //SDK 4  09/03/2016
    }

    if( trigEvent != NULL )
        delete trigEvent;
    
}



int PTGREY::checkLastOp()
{
    printf("Info %d (%s)\n", this->lResult.IsOK(), lResult.GetDescription().GetAscii() ); 
    if ( !this->lResult.IsOK() ) 
      printLastError("(%s)\n", lResult.GetDescription().GetAscii() ); 
    return this->lResult.IsOK();
}



int PTGREY::printAllParameters()
{
	PvGenParameterArray *aArray = lDevice->GetParameters();
	uint32_t lParameterArrayCount = aArray->GetCount();
	printf( "\nArray has %d parameters\n", lParameterArrayCount );

	// Traverse through Array and print out parameters available
	for( uint32_t x = 0; x < lParameterArrayCount; x++ )
	{
		// Get a parameter
		PvGenParameter *lGenParameter = aArray->Get( x );

        // Don't show invisible parameters - display everything up to Guru
        if ( !lGenParameter->IsVisible( PvGenVisibilityGuru ) )
        {
            continue;
        }

		// Get and print parameter's name
		PvString lGenParameterName, lCategory;
		lGenParameter->GetCategory( lCategory );
		lGenParameter->GetName( lGenParameterName );
		printf( "%s:%s, ", lCategory.GetAscii(), lGenParameterName.GetAscii() );

        // Parameter available?
        if ( !lGenParameter->IsAvailable() )
        {
            printf( "{Not Available}\n" );
			continue;
        }

		// Parameter readable?
		if ( !lGenParameter->IsReadable() )
		{
            printf( "{Not readable}\n" );
			continue;
		}
		
		// Get the parameter type
		PvGenType lType;
		lGenParameter->GetType( lType );
		switch ( lType )
		{
			// If the parameter is of type PvGenTypeInteger
			case PvGenTypeInteger:
				{
					int64_t lValue;				
					static_cast<PvGenInteger *>( lGenParameter )->GetValue( lValue );
					printf( "Integer: %d", lValue );
				}
				break;
			// If the parameter is of type PvGenTypeEnum
			case PvGenTypeEnum:
				{
					PvString lValue;				
					static_cast<PvGenEnum *>( lGenParameter )->GetValue( lValue );
					printf( "Enum: %s", lValue.GetAscii() );
				}
				break;
			// If the parameter is of type PvGenTypeBoolean
			case PvGenTypeBoolean:
				{
					bool lValue;				
					static_cast<PvGenBoolean *>( lGenParameter )->GetValue( lValue );
					if( lValue ) 
					{
						printf( "Boolean: TRUE" );
					}
					else 
					{
						printf( "Boolean: FALSE" );
					}
				}
				break;
			// If the parameter is of type PvGenTypeString
			case PvGenTypeString:
				{
					PvString lValue;
					static_cast<PvGenString *>( lGenParameter )->GetValue( lValue );
					printf( "String: %s", lValue.GetAscii() );
				}
				break;
			// If the parameter is of type PvGenTypeCommand
			case PvGenTypeCommand:
				printf( "Command" );
				break;
			// If the parameter is of type PvGenTypeFloat
			case PvGenTypeFloat:
				{
					double lValue;				
					static_cast<PvGenFloat *>( lGenParameter )->GetValue( lValue );
					printf( "Float: %f", lValue );
				}
				break;
		}
		printf("\n");
	}

	return SUCCESS;
}


int PTGREY::readInternalTemperature()
{
  double t;	
  PvGenParameterArray *lDeviceParams = lDevice->GetParameters(); 	
  PvGenFloat *linternalTemp = dynamic_cast<PvGenFloat *>( lDeviceParams->Get( "DeviceTemperature" ) );
  this->lResult = linternalTemp->GetValue( t );
  if ( !this->lResult.IsOK() ) 
     {printLastError("Error reading Internal Temperature!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 

  this->internalTemperature=t;   //t must be double but is a float
  printf("%s: Camera Temperature is now %3.2f°C\n", (this->ipAddress).GetAscii(), this->internalTemperature); 

  return SUCCESS;
}


int PTGREY::setExposure(float exposure)  //[us] - Min: 15 Max: 3e+07   attenzione double-float 
{
  PvString lValue;
  PvGenParameterArray *lDeviceParams = lDevice->GetParameters();
  PvGenEnum *lExposureAuto = dynamic_cast<PvGenEnum *>( lDeviceParams->Get( "ExposureAuto" ) );
  if(exposure<15.0 or exposure>30000000.0)
  {
    printf("ERROR: exposure should be in range 15.0 - 3e+07 [us]...\n");
    return ERROR;
  }
  this->lResult = lExposureAuto->GetValue( lValue );
  if(strcmp(lValue.GetAscii(), "Off")==0)
  {
    PvGenParameterArray *lDeviceParams = lDevice->GetParameters();
    PvGenFloat *lExposure = dynamic_cast<PvGenFloat *>( lDeviceParams->Get( "ExposureTime" ) );
    this->lResult = lExposure->SetValue( exposure );
    if ( !this->lResult.IsOK() ) 
     {printLastError("Error setting exposure!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}   
  }
  else
  {
    printf("WARNING: to set the exposure value the ExposureAuto must be Off\n");
  }
  this->exposure=exposure;
  printf("%s: Exposure set to %.1fus\n", (this->ipAddress).GetAscii(), exposure); 
  return SUCCESS;
}


int PTGREY::setExposureAuto(char *exposureAuto)
{
  if(strcmp(exposureAuto, "Off")!=0 & strcmp(exposureAuto, "Once")!=0 & strcmp(exposureAuto, "Continuous")!=0)
  {
    printf("ERROR: setExposureAuto must be Off, Once or Continuous\n");
    return ERROR;
  }

  PvGenParameterArray *lDeviceParams = lDevice->GetParameters(); 	
  this->lResult = lDeviceParams->SetEnumValue( "ExposureAuto", exposureAuto );
  if ( !this->lResult.IsOK() ) 
     {printLastError("Error setting exposureAuto!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 

  return SUCCESS;
}

int PTGREY::setGammaEnable(char *gammaEnable)
{
/*
  if(strcmp(gammaEnable, "On")==0)
  {
    pCamera->GammaEnable.SetValue(true);
  }
  if(strcmp(gammaEnable, "Off")==0)
  {
    pCamera->GammaEnable.SetValue(false);
  }
*/   
  printf("%s: setGammaEnable BYPASSED 4 TEST\n", (this->ipAddress).GetAscii()); 

  return SUCCESS;
}

int PTGREY::setGain(float gain)  //Off impostabile solo in gainAuto=Off
{
  PvString lValue;
  PvGenParameterArray *lDeviceParams = lDevice->GetParameters();
  PvGenEnum *lGainAuto = dynamic_cast<PvGenEnum *>( lDeviceParams->Get( "GainAuto" ) );
  if(gain<0.0 or gain>47.9943)
  {
    printf("ERROR: gain should be in range 0.0 - 47.9943 [dB]...\n");
    return ERROR;
  }
  this->lResult = lGainAuto->GetValue( lValue );
  if(strcmp(lValue.GetAscii(), "Off")==0)
  {
    PvGenParameterArray *lDeviceParams = lDevice->GetParameters();
    PvGenFloat *lGain = dynamic_cast<PvGenFloat *>( lDeviceParams->Get( "Gain" ) );
    this->lResult = lGain->SetValue( gain );
    if ( !this->lResult.IsOK() ) 
     {printLastError("Error setting gain!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}   
  }
  else
  {
    printf("WARNING: to set the gain value the gainAuto must be Off\n");
  }
  this->gain=gain;
  printf("%s: Gain set to %.2fdB\n", (this->ipAddress).GetAscii(), gain); 
  return SUCCESS;
}


int PTGREY::setGainAuto(char *gainAuto)  //Off(0), Once(1), Continuous(2)
{
  if(strcmp(gainAuto, "Off")!=0 & strcmp(gainAuto, "Once")!=0 & strcmp(gainAuto, "Continuous")!=0)
  {
    printf("ERROR: setGainAuto must be Off, Once or Continuous\n");
    return ERROR;
  }

  PvGenParameterArray *lDeviceParams = lDevice->GetParameters(); 	
  this->lResult = lDeviceParams->SetEnumValue( "GainAuto", gainAuto );
  if ( !this->lResult.IsOK() ) 
     {printLastError("Error setting gainAuto!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 

  return SUCCESS;
}



int PTGREY::setFrameRate(double &frameRate)
{
  PvGenParameterArray *lDeviceParams = lDevice->GetParameters();
  PvGenBoolean *sFrameRateEn = dynamic_cast<PvGenBoolean *>( lDeviceParams->Get("AcquisitionFrameRateEnable"));
  if(frameRate<1.0 or frameRate>39.2221)
  {
     double resFrameRate; 

     this->lResult = sFrameRateEn->SetValue( 0 );
     if ( !this->lResult.IsOK() ) {printLastError("Error setting AcquisitionFrameRateEnable in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 
     usleep(3000); 


     PvGenFloat *sResFrameRate = dynamic_cast<PvGenFloat *>( lDeviceParams->Get("AcquisitionResultingFrameRate"));
     this->lResult = sResFrameRate->GetValue( resFrameRate );

     if ( !this->lResult.IsOK() ) {printLastError("Error setting AcquisitionResultingFrameRate in getFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 
    
     printf("%s: Resulting Framerate  %f fps\n", (this->ipAddress).GetAscii(), resFrameRate); 

     this->frameRate = resFrameRate;

     frameRate = resFrameRate;

    //printf("ERROR: frameRate should be in range 1.0 - 39.2221 [Hz]...\n");
    return SUCCESS;
  }
  this->lResult = sFrameRateEn->SetValue( 1 );
  if ( !this->lResult.IsOK() ) {printLastError("Error setting AcquisitionFrameRateEnable in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 
  usleep(3000); 
  
  PvGenFloat *sFrameRate = dynamic_cast<PvGenFloat *>( lDeviceParams->Get("AcquisitionFrameRate"));
  this->lResult = sFrameRate->SetValue( (float)frameRate );
  if ( !this->lResult.IsOK() ) {printLastError("Error setting AcquisitionFrameRate in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 
  usleep(3000);

  this->frameRate = frameRate;
  printf("%s: Framerate set to %.2ffps\n", (this->ipAddress).GetAscii(), frameRate); 

  return SUCCESS;
}

/*
int PTGREY::getReadoutArea(int *x, int *y, int *width, int *height)
{
    PvGenParameterArray *lDeviceParams = lDevice->GetParameters();
	PvGenInteger *lWidth = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "Width" ) );
	PvGenInteger *lHeight = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "Height" ) );
	PvGenInteger *lOffsetX = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "OffsetX" ) );
	PvGenInteger *lOffsetY = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "OffsetY" ) );

	int64_t ox = 0;
	int64_t oy = 0;
	int64_t w = 0;
	int64_t h = 0;


	this->lResult = lWidth->GetValue( w );
	if ( !this->lResult.IsOK() ) {printLastError("Error getting Width in getReadoutArea\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(3000);
	this->lResult = lHeight->GetValue( h );
	if ( !this->lResult.IsOK() ) {printLastError("Error getting Height in getReadoutArea\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(3000);
	this->lResult = lOffsetX->GetValue( ox );
	if ( !this->lResult.IsOK() ) {printLastError("Error getting OffsetX in getReadoutArea\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(3000);
	this->lResult = lOffsetY->GetValue( oy );
	if ( !this->lResult.IsOK() ) {printLastError("Error getting OffsetY in getReadoutArea\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}

	this->x = *x=(int)ox;
	this->y = *y=(int)oy;  
	this->width = *width=(int)w;
        this->height = *height=(int)h;
//	this->height = *height=(int)h-3;	//remove 3 lines of metadata  


    return SUCCESS;
}
*/


int PTGREY::setReadoutArea(int x, int y, int width, int height)
{
   PvGenParameterArray *lDeviceParams = lDevice->GetParameters();
   PvGenInteger *lWidth = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "Width" ) );
   PvGenInteger *lHeight = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "Height" ) );
   PvGenInteger *lOffsetX = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "OffsetX" ) );
   PvGenInteger *lOffsetY = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "OffsetY" ) );

   this->lResult = lWidth->SetValue(width);
   if ( !this->lResult.IsOK() ) {printLastError("Error setting Width in setReadoutArea\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
   usleep(3000);
// this->lResult = lHeight->SetValue(h+3);  //add 3 lines of metadata
   this->lResult = lHeight->SetValue(height);  //add 3 lines of metadata
   if ( !this->lResult.IsOK() ) {printLastError("Error setting Height in setReadoutArea\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
   usleep(3000);
   this->lResult = lOffsetX->SetValue(x);
   if ( !this->lResult.IsOK() ) {printLastError("Error setting OffsetX in setReadoutArea\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
   usleep(3000);
   this->lResult = lOffsetY->SetValue(y);
   if ( !this->lResult.IsOK() ) {printLastError("Error setting OffsetY in setReadoutArea\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}

   this->x = x;
   this->y = y;  
   this->width = width;
   this->height = height;  

   return SUCCESS;
}


int PTGREY::setPixelFormat(char *pixelFormat)    //Enum: Mono8 - Mono16 (others not used: Mono10Packed, Mono12Packed)
{
  if(strcmp(pixelFormat, "Mono8")!=0 & strcmp(pixelFormat, "Mono16")!=0)
  {
    printf("ERROR: PixelFormat must be Mono8 or Mono16\n");
    return ERROR;
  }

  PvGenParameterArray *lDeviceParams = lDevice->GetParameters(); 	
  this->lResult = lDeviceParams->SetEnumValue( "PixelFormat", pixelFormat );
  if ( !this->lResult.IsOK() ) 
     {printLastError("Error setting PixelFormat!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 

  if(strcmp(pixelFormat, "Mono8")==0)
  {
     this->pixelFormat = CSU_PIX_FMT_GRAY8;
     this->Bpp = 1;
  }
  if(strcmp(pixelFormat, "Mono16")==0)
  {
     this->pixelFormat = CSU_PIX_FMT_GRAY16;
     this->Bpp = 2;
  }

  printf("%s: PixelFormat set to %s\n", (this->ipAddress).GetAscii(), pixelFormat); 
  return SUCCESS;
}


int PTGREY::startAcquisition(int *width, int *height, int *payloadSize)
{
    PvGenParameterArray *lDeviceParams = lDevice->GetParameters();
    PvGenInteger *lTLLocked = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "TLParamsLocked" ) );
    PvGenInteger *lWidth = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "Width" ) ); 
    PvGenInteger *lHeight = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "Height" ) );
    PvGenInteger *lPayloadSize = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "PayloadSize" ) );
    PvGenCommand *lResetTimestamp = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "GevTimestampControlReset" ) );
    PvGenCommand *lStart = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "AcquisitionStart" ) );
    
    // Get stream parameters/stats
//17-10-2016: segmentation fault with SDK4. to check!
/* 
    PvGenParameterArray *lStreamParams = lStream->GetParameters();   
    PvGenInteger *lCount = dynamic_cast<PvGenInteger *>( lStreamParams->Get( "ImagesCount" ) );
    PvGenFloat *lFrameRate = dynamic_cast<PvGenFloat *>( lStreamParams->Get( "AcquisitionRateAverage" ) );
    PvGenFloat *lBandwidth = dynamic_cast<PvGenFloat *>( lStreamParams->Get( "BandwidthAverage" ) );
*/


	PvGenInteger *lTickFreq = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "GevTimestampTickFrequency" ) );
	this->lResult = lTickFreq->GetValue( tickFreq );
	if ( !this->lResult.IsOK() ) {printLastError("Error getting GevTimestampTickFrequency in startAcquisition!!!\n(%s)\n", lResult.GetDescription().GetAscii() );}
	//printf("---> GevTimestampTickFrequency = %d\n", tickFreq);

	int64_t w = 0;
	int64_t h = 0;
	int64_t lSize = 0;

	this->lResult = lWidth->GetValue( w );
	if ( !this->lResult.IsOK() ) {printLastError("Error getting frame width in startAcquisition!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
	this->lResult = lHeight->GetValue( h );
	if ( !this->lResult.IsOK() ) {printLastError("Error getting frame height in startAcquisition!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
	this->lResult = lPayloadSize->GetValue( lSize ); 
	if ( !this->lResult.IsOK() ) {printLastError("Error getting payload size in startAcquisition!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 

	this->width = *width = (int)w;
//	this->height = *height= (int)h-3;  				//first 3 rows are metadata
        this->height = *height= (int)h;

	*payloadSize=lSize;  //payload = width*height*2 + metadata
        printf("w:%d h:%d p:%d\n", this->width, this->height, (int)lSize);
/*
 new 09 mar 2016 for SDK4	 
 // If this is a GigE Vision device, configure GigE Vision specific streaming parameters
*/
    PvDeviceGEV *lDeviceGEV = static_cast<PvDeviceGEV *>( this->lDevice );
    //17-10-2016 lStreamGEV moved down
/*
end new
*/

    // Negotiate streaming packet size
    //this->lResult = lDevice->NegotiatePacketSize();  //SDK 3
	this->lResult = lDeviceGEV->NegotiatePacketSize();  //SDK 4

	if ( !this->lResult.IsOK() ) 
        {  
           printLastError("Error negotiating packet size in start acquisition!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); 
           return ERROR;
        } 

    PvGenInteger *lPacketSize = dynamic_cast<PvGenInteger *>( lDevice->GetParameters()->Get( "GevSCPSPacketSize" ) );

    int64_t lPacketSizeValue = 0;
    lResult = lPacketSize->GetValue( lPacketSizeValue );
    if ( !lResult.IsOK() )
    {
		printLastError("FATAL ERROR: Unable to read packet size!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); 
		return ERROR; 
        //fprintf( stderr, "FATAL ERROR: Unable to read packet size\n" );
    }

    printf("--------> lPacketSizeValue %d\n", lPacketSizeValue);
 
  //  PvString aID;
  //  this->lResult = lDevice->GetUniqueID(aID);	
  
    // Open stream
    //this->lResult = this->lStream->Open( this->ipAddress);    //SDK 3
    this->lStream = NULL;  

    //this->lStream = PvStream::CreateAndOpen(this->ipAddress, &this->lResult); //20160309 SDK4	 
    this->lStream = PvStream::CreateAndOpen(this->ipAddress, &this->lResult); //20160309 SDK4
    
    //if ( ( this->lStream == NULL ) || !this->lResult.IsOK() )
    if ( this->lStream == NULL )    
    { 
        printLastError("Error opening stream in start acquisition!!!\n(%s)\n", lResult.GetDescription().GetAscii() );  
        PvStream::Free( this->lStream );
	return ERROR;
    }


    // Use min of BUFFER_COUNT and how many buffers can be queued in PvStream
    uint32_t lBufferCount = ( lStream->GetQueuedBufferMaximum() < BUFFER_COUNT ) ? 
        lStream->GetQueuedBufferMaximum() : 
        BUFFER_COUNT;

 //new 17-10-2016 for SDK4	 
 // If this is a GigE Vision device, configure GigE Vision specific streaming parameters
    PvStreamGEV *lStreamGEV = static_cast<PvStreamGEV *>( this->lStream );

     if( this->lStream->IsOpen() ) 
     {
        //printf("lStream OPEN Local Port %d \n",  lStream->GetLocalPort() );   //SDK 3
	printf("lStream OPEN Local Port %u \n", lStreamGEV->GetLocalPort() );  //SDK 4 not working.... segmentation fault to check!
     }
     else 
     {
        printf("lStream NOT OPEN\n");
     }
     if(lDevice->IsConnected())
	{printf( "PvDevice connect \n");}
     else
	{printf( "PvDevice not connect \n");}

	printf("----------> Buffer count %d lSize %d \n",  lBufferCount, lSize);

    // Create, alloc buffers
    this->lBuffers = new PvBuffer[ lBufferCount ];
    for ( uint32_t i = 0; i < lBufferCount; i++ )
    {
        lResult = lBuffers[ i ].Alloc( static_cast<uint32_t>( lSize ) );
		if ( !lResult.IsOK() )
			printf("Error memory buffer allocation %d !!!\n(%s)\n", (i+1) , lResult.GetDescription().GetAscii() );
    }

     if( lStream->IsOpen() ) printf("lStream OPEN\n");
     else printf("lStream NOT OPEN\n");

   if(lDevice->IsConnected())
		printf( "PvDevice connect \n");
	else
		printf( "PvDevice not connect \n");

    // Have to set the Device IP destination to the Stream
    //this->lResult = lDevice->SetStreamDestination( lStream->GetLocalIPAddress(), lStream->GetLocalPort() );  //SDK 3
     this->lResult = lDeviceGEV->SetStreamDestination( lStreamGEV->GetLocalIPAddress(), lStreamGEV->GetLocalPort() ); //SDK 4  - 9 mar 2016
	if ( !this->lResult.IsOK() ) {printLastError("Error setting stream dest. in start acquisition!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 

     if( lStream->IsOpen() ) printf("lStream OPEN\n");
     else printf("lStream NOT OPEN\n");

   if(lDevice->IsConnected())
		printf( "PvDevice connect \n");
	else
		printf( "PvDevice not connect \n");


    // Queue all buffers in the stream
    for ( uint32_t i = 0; i < lBufferCount; i++ )
    {
printf("------------------------ Queuing Buffer -----------------------------------\n");
        lResult = lStream->QueueBuffer( &lBuffers[i] );
printf("------------------------ Queued Buffer -----------------------------------\n");
		if ( !lResult.IsPending() )
			printf("Error buffer enqueue %d !!!\n(%s, %s)\n", (i+1) , lResult.GetDescription().GetAscii(), lResult.GetCodeString().GetAscii() );
    }

     if( lStream->IsOpen() ) printf("lStream OPEN\n");
     else printf("lStream NOT OPEN\n");

   if(lDevice->IsConnected())
		printf( "PvDevice connect \n");
	else
		printf( "PvDevice not connect \n");

    // TLParamsLocked is optional but when present, it MUST be set to 1
    // before sending the AcquisitionStart command
    if ( lTLLocked != NULL )
    {
usleep(3000);
        this->lResult = lTLLocked->SetValue( 1 );

		if ( !this->lResult.IsOK() ) {printLastError("Error setting TLParamsLocked to 1!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 
    }
	//reset timestamp counter
printf( "2019-09-17: CODE COMMENTED FOR POINT GRAY TESTS \n");
//2019-09-17 fede comment out for test with point gray visible camera
//    this->lResult = lResetTimestamp->Execute();
//	if ( !this->lResult.IsOK() ) {printLastError("Error resetting timestamp counter!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 
    // Tell the device to start sending images
    this->lResult = lStart->Execute();
	if ( !this->lResult.IsOK() ) {printLastError("Error sending StartAcquisition command to device!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 	

//debug
	triggered = 0;
/*	
	printf( "Get Local IP Address %s \n",  lStream->GetLocalIPAddress().GetAscii() );
	printf( "QueuedBuffer %d \n",  lStream->GetQueuedBufferCount() );
	printf( "QueuedBuffer Maximum %d \n",  lStream->GetQueuedBufferMaximum() );


    if(lDevice->IsConnected())
		printf( "PvDevice connect \n");
	else
		printf( "PvDevice not connect \n");


    if(lStream->IsOpen())
		printf( "PvStream open \n");
	else
		printf( "PvStream not open \n");
*/
	return SUCCESS;	
}
  


int PTGREY::stopAcquisition()
{
    PvGenParameterArray *lDeviceParams = lDevice->GetParameters();
    PvGenInteger *lTLLocked = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "TLParamsLocked" ) );
    PvGenCommand *lStop     = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "AcquisitionStop" ) );

    printf("PTGREY : device stop acquisition");

    // Tell the device to stop sending images
    this->lResult = lStop->Execute();
    for(int i = 0; i < 3 && !this->lResult.IsOK(); i++)
    {
        usleep(30000);
        this->lResult = lStop->Execute();
	if ( !this->lResult.IsOK() ) {printf( RED "Try %d : Error sending AcquisitionStop command to the device!!!\n(%s)\n" RESET , i, lResult.GetDescription().GetAscii() ); } 
    }
    if ( !this->lResult.IsOK() ) {printLastError("Error sending AcquisitionStop command to the device!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 

    // If present reset TLParamsLocked to 0. Must be done AFTER the 
    // streaming has been stopped
    if ( lTLLocked != NULL )
    {
        this->lResult = lTLLocked->SetValue( 0 );
		if ( !this->lResult.IsOK() ) {printLastError("Error resetting TLParamsLocked to 0!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 
    }

    //printf( "Aborting buffers still in stream\n" );
    lStream->AbortQueuedBuffers();
    while ( lStream->GetQueuedBufferCount() > 0 )
    {
        PvBuffer *lBuffer = NULL;
        PvResult lOperationResult;
        lStream->RetrieveBuffer( &lBuffer, &lOperationResult );
        //printf( "  Post-abort retrieved buffer: %s\n", lOperationResult.GetCodeString().GetAscii() );
    }

    //printf( "Releasing buffers\n" );
    delete []lBuffers;

    // Now close the stream. Also optionnal but nice to have
    printf( "Closing stream\n" );
    lStream->Close();


    return SUCCESS;
}


int PTGREY::getFrame(int *status, void *frame, void *metaData)
{
//status=1 complete # status=2 incomplete # status=3 timeout # status=4 triggered frame + complete
 //   char lDoodle[] = "|\\-|-/";
 //   static int lDoodleIndex = 0;
    int64_t lImageCountVal = 0;
    double lFrameRateVal = 0.0;
    double lBandwidthVal = 0.0;

    PvBuffer *lBuffer = NULL;
    PvResult lOperationResult;

    // Get stream parameters/stats
    PvGenParameterArray *lStreamParams = lStream->GetParameters();
    PvGenInteger   *lCount = dynamic_cast<PvGenInteger *>( lStreamParams->Get( "ImagesCount" ) );
    PvGenFloat *lFrameRate = dynamic_cast<PvGenFloat *>( lStreamParams->Get( "AcquisitionRateAverage" ) );
    PvGenFloat *lBandwidth = dynamic_cast<PvGenFloat *>( lStreamParams->Get( "BandwidthAverage" ) );

    *status=2; //frame incomplete by default
 //   ++lDoodleIndex %= 6;

    // Retrieve next buffer	
    //this->lResult = lStream->RetrieveBuffer( &lBuffer, &lOperationResult, 1000 );
    int timeOut = int (1./this->frameRate * 1000.) + 100;
    this->lResult = lStream->RetrieveBuffer( &lBuffer, &lOperationResult, timeOut ); //Set timeOut to the frame period plus 100ms
                                                                                     


    //this->lResult = lStream->RetrieveBuffer( &lBuffer, &lOperationResult);
    if ( this->lResult.IsOK() )
    {
      	if(lOperationResult.IsOK())
       	{
			//lCount->GetValue( lImageCountVal );  //comment out 17-10-2016 with SDK4 segmentation fault
			lFrameRate->GetValue( lFrameRateVal );
			lBandwidth->GetValue( lBandwidthVal );

			// If the buffer contains an image, display width and height
			uint32_t lWidth = 0, lHeight = 0;
			if ( lBuffer->GetPayloadType() == PvPayloadTypeImage )
			{
				// Get image specific buffer interface
				PvImage *lImage = lBuffer->GetImage();
				// Read width, height
				lWidth  = lImage->GetWidth();
				lHeight = lImage->GetHeight();
				currTime = lBuffer->GetTimestamp(); 

				currIdx = lBuffer->GetBlockID();
				//if( triggered && currIdx != lastIdx+1)
				//if( triggered)
				//	printf("deltaTime ms = %f idx %d tick %d\n", ( ((currTime - lastTime) *.1) / tickFreq) * 1000  , currIdx, tickFreq );
				lastTime = currTime;
				lastIdx = currIdx;
/*
				printf( "%c Timestamp: %016llX BlockID: %04X W: %i H: %i %.01f FPS %.01f Mb/s\r",
					lDoodle[ lDoodleIndex ],
					lBuffer->GetTimestamp(),
					lBuffer->GetBlockID(),
					lWidth,
					lHeight,
					lFrameRateVal,
					lBandwidthVal / 1000000.0 );
*/
				unsigned int width = lWidth;
 				unsigned int height = lHeight;

		//		PvUInt8* dataPtr = lImage->GetDataPointer();  	//NO!!!!		
		//		PvUInt8* dataPtr = lBuffer->GetDataPointer();	//OK for SDK3
				uint8_t* dataPtr = lBuffer->GetDataPointer();	//8 mar 2016 for SDK4

			        //last 3 rows of the frame are metadata
				//memcpy( frame , (unsigned char *)dataPtr, width*height );       //20191107
                                memcpy( frame , (unsigned char *)dataPtr, width*height*this->Bpp ); //20191107
		//		memcpy( metaData , (unsigned char *)dataPtr+(width*2*(height-3)), width*2*3 );	
          	

                                //printf("metadata size: %d\n", sizeof(BASLERMETADATA));
                                PTGREYMETADATA pgMeta;
                                pgMeta.gain=this->gain;
                                pgMeta.exposure=this->exposure;
                                pgMeta.internalTemperature=this->internalTemperature;
                                pgMeta.timestamp=currTime;
                                memcpy( metaData , (unsigned char *)&pgMeta, sizeof(PTGREYMETADATA));
			
				*status=1; //complete
			}			
        }
        else  //new 28 agosto 2012
    	{   
	  	*status=2; //incomplete		   
      		//printf( "%c Incomplete\r", lDoodle[ lDoodleIndex ] );
		printf("%s %d\n", lOperationResult.GetCodeString().GetAscii(), lBuffer->GetPayloadType() );
		incompleteFrame++;
    	}
		// We have an image - do some processing (...) and VERY IMPORTANT,
		// re-queue the buffer in the stream object
		lStream->QueueBuffer( lBuffer );
		return SUCCESS;
    }
    else// Timeout
    {   
      printf("%s (%s) \n", lResult.GetCodeString().GetAscii(), lResult.GetDescription().GetAscii()  );
      printLastError("Error reading frame!!!\n(%s)\n", lResult.GetDescription().GetAscii() );
      *status=3; //timeout		   
//    printf( "%c Timeout\r", lDoodle[ lDoodleIndex ] );
      return ERROR;
    }

}



int PTGREY::setStreamingMode( int streamingEnabled,  bool autoAdjustLimit, const char *streamingServer, int streamingPort, unsigned int lowLim, unsigned int highLim, int adjRoiX, int adjRoiY, int adjRoiW, int adjRoiH, const char *deviceName)
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
   else
   {
     printf("Streaming DISABLED\n");
   }
   return SUCCESS;
} 


int PTGREY::setAcquisitionMode( int storeEnabled , int acqSkipFrameNumber)
{

   this->storeEnabled = storeEnabled;
   this->acqSkipFrameNumber = acqSkipFrameNumber;
   return SUCCESS;
}

int PTGREY::setTriggerMode( int triggerMode, double burstDuration, int numTrigger )
{
	this->triggerMode = triggerMode;
	this->burstDuration = burstDuration;
	this->numTrigger = numTrigger;

	return SUCCESS;
}

int PTGREY::softwareTrigger()
{
	this->startStoreTrg = 1; 
	return SUCCESS;
}

int PTGREY::setTreeInfo( void *treePtr, int framesNid, int timebaseNid, int framesMetadNid, int frame0TimeNid)
{
	this->treePtr = treePtr ;
	this->framesNid = framesNid;
	this->timebaseNid = timebaseNid;
	this->framesMetadNid = framesMetadNid;
        this->frame0TimeNid = frame0TimeNid;

	return SUCCESS;
}


void PTGREY::printLastError(const char *format, const char *msg)
{
	error[0] = 0;
	if(msg)
		sprintf(error, format, msg );
	else
		sprintf(error, format);
}


void PTGREY::getLastError(char *msg)
{
	sprintf(msg, RED "%s" RESET, (error[0]==0) ? "" : error);
}

int PTGREY::stopFramesAcquisition()
{
	int count = 0;

	if (acqFlag == 0)
		return SUCCESS;

	acqFlag = 0;
	while( !acqStopped && count < 20 )
	{       
		count++;
		usleep(50000);     //20190829 FM: 5000->50000
	}

	if(count == 20)
	{
		printLastError("Cannot stop acquisition loop", 0);
		return ERROR;
	}
		

	return SUCCESS;
}

int PTGREY::startFramesAcquisition()
{
	int frameTriggerCounter;
	int frameCounter;
	int frameStatus;

	int NtriggerCount = 0;
	int burstNframe;
	int rstatus;
	int tcpStreamHandle = -1;

	int metaSize;
	int savedFrameNumber;
	float frameTime = 0.0;
        float timeOffset = 0.0; //20200901
	void *saveList;
	void *streamingList;

//	short *frameBuffer;  //20191107
//	short *metaData;

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

    
        //if ( triggerMode != 1 ) //20200901: in internal mode use the timebaseNid as T0 offset (ex. T_START_SPIDER)
        {                         //20210325: In external trigger, triggered on event must be set timeOffest
         TreeNode *tStartOffset;
         try{
             tStartOffset = new TreeNode(timebaseNid, (Tree *)treePtr);
             Data *nodeData = tStartOffset->getData();
             timeOffset = (float)nodeData->getFloatArray()[0];
         }catch(MdsException *exc)
          {
            printf("Error getting timebaseNid (offset time set to 0.0s)\n");
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

        metaSize = sizeof(PTGREYMETADATA);
	metaData = (unsigned char *)calloc(1, metaSize);

        camStartSave(&saveList); //  # Initialize save frame Linked list reference

   	camStartStreaming(&streamingList); //  # Initialize streaming frame Linked list reference

	burstNframe = (int) ( burstDuration * frameRate + 1. + 0.5);//CT 2021 03 20

	acqFlag = 1;
	frameTriggerCounter = 0;
	frameCounter = 0;
        incompleteFrame = 0;
	savedFrameNumber = 0;

	startStoreTrg = 0;  //manage the mdsplus saving process. SAVE always start with a SW or HW trigger. (0=no-save; 1=save)

	acqStopped = 0;//CT 2021 02 04

        while ( acqFlag )
	{
        getFrame( &frameStatus, frameBuffer, metaData);   //get the frame
    
        if(storeEnabled)
        {
          if ( triggerMode == 1 )        // External trigger source
	  {

           	if ( (frameStatus == 4 || eventTrigger == 1  ) && (startStoreTrg == 0) )       //start data storing @ 1st trigger seen (trigger is on image header!)
                                                                                               //CT In External Trigger Mode acquisition is also triggered  on MDSevent 
		{
            	  startStoreTrg = 1;

                  if( eventTrigger == 1 )//For debug in Event trigger mode is disable time base and use local time frame time stamp
                  {
                      timebaseNid = -1;
                      if(NtriggerCount == 0 ) timeStamp0 = 0;
                  }

                  eventTrigger  = 0;                                                           //CT Reset MDSplus trigger event flag 
            	  printf("TRIGGERED: %d %d\n", frameStatus, eventTrigger);	
		}

           	if (frameTriggerCounter == burstNframe) 
		{
		  triggered = 0;
		  startStoreTrg   = 0;   //disable storing                  
		  NtriggerCount++; 
    
                  printf("ACQUIRED ALL FRAMES %d FOR TRIGGER : %d\n", frameTriggerCounter-1,  NtriggerCount );	
                  frameTriggerCounter = 0;

	          if ( NtriggerCount == numTrigger ) //stop store when all trigger will be received
		  { 
	            printf("ACQUIRED ALL FRAME BURST: %d\n", numTrigger );
                    storeEnabled=0;	
	            //break;             
		  }
		 }//if (frameTriggerCounter == burstNframe) 


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
                        printf("frameTime stamp 0: %f", timeStamp0); 
                    }
                    else
                    {   
                        frameTime = (float)((timeStamp-timeStamp0)/1000.0); //interval from first frame [s]
                     // printf("frameTime: %f", frameTime);      
                    }
                }//if startStoreTrg == 1 

/*************************/

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
            	   printf("Stop Internal trigger acquisition %f %f %f\n", frameTime, burstDuration, frameRate);
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
	
	  // CT la routine camSaveFrame utilizza il frame index in acquisizione. L'indice viene
    	  // utilizzato per individuare nell'array della base temporale il tempo associato al frame.

	  // Con Trigger interno viene utilizzato frameTime come tempo relativo allo 0; timebaseNid deve essere -1
          if (timebaseNid == -1 )
              printf("DEBUG TO REMOVE 20200904 - FRAME TIME: (%s) %f\n", this->ipAddress.GetAscii(), frameTime+timeOffset);
          else
	      printf("(%s) SAVE Num Tr : %2d Frame Tr. Cnt : %5d timebase Idx : %5d\n", this->ipAddress.GetAscii(), NtriggerCount,  frameTriggerCounter,  frameTimeBaseIdx);
          camSaveFrame((void *)frameBuffer, width, height, frameTime+timeOffset, 8*this->Bpp, (void *)treePtr, framesNid, timebaseNid, frameTimeBaseIdx, (void *)metaData, metaSize, framesMetadNid, saveList); 
	  savedFrameNumber++;
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

        frameCounter++;   //never resetted, used for frame timestamp     
        if ( startStoreTrg == 1 ) //CT incremento l'indice dei frame salvato solo se l'acquisizione e' stata triggerata 
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
	printf(RED " Cannot stop camera acquisition \n" RESET );

   
    free(frameBuffer);
    free(frame8bit);
    free(metaData);

	printf("Acquisition Statistics : \tTotal frames read %d, \n\t\t\t\tTotal frames stored %d (expected %d), \n\t\t\t\tNumber of trigger %d (expected %d), \n\t\t\t\tIncomplete frame %d\n", frameCounter, savedFrameNumber, numTrigger * (1+(int)( burstDuration * (frameRate-acqSkipFrameNumber))), NtriggerCount + startStoreTrg, numTrigger, incompleteFrame );

	acqStopped = 1;

	return rstatus;
}

