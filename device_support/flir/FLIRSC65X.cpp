#include <vector>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <mdsobjects.h>
using namespace MDSplus;
using namespace std;

#include "FLIRSC65X.h"
#include "fff.h"
#include <cammdsutils.h>
#include <camstreamutils.h>
#include "flirutils.h"

#include <math.h>
#include <sys/time.h>

#define BUFFER_COUNT 16 

#define ERROR -1
#define SUCCESS 0

//if defined, print to stdout messages.
#define debug


#define MAX_CAM 10
static FLIR_SC65X *camPtr[MAX_CAM] = {0};
static char errorOnOpen[512];


#ifdef __cplusplus 
extern "C" 
{ 
#endif 


int flirOpen(const char *ipAddress, int *camHandle)
{
	FLIR_SC65X *cam;
	int cameraHandle;

	errorOnOpen[0] = 0;

	cam = new FLIR_SC65X(ipAddress);
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


int flirIsConnected( int camHandle )
{
	if( camHandle < 0 || camHandle >= MAX_CAM || camPtr[camHandle] == 0 ) 
	{
		sprintf(errorOnOpen, "Camera does not connect");
		return ERROR;
	}
	return SUCCESS;
}

int flirClose(int camHandle)
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
	if( flirIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->printAllParameters();
	return ERROR;
}


int setExposureMode(int camHandle, EXPMODE_ENUM exposureMode)
{
	if( flirIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->setExposureMode(exposureMode);
	return ERROR;
}


int setFrameRate(int camHandle, FPS_ENUM frameRate, int *frameToSkip)
{
	if( flirIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->setFrameRate(frameRate, frameToSkip);
	return ERROR;
}


int setFrameRateNew(int camHandle, double frameRate)
{
	if( flirIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->setFrameRate(frameRate);
	return ERROR;
}


int setIrFormat(int camHandle, IRFMT_ENUM irFormat)
{
	if( flirIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->setIrFormat(irFormat);
	return ERROR;
}


int getReadoutArea(int camHandle, int *x, int *y, int *width, int *height)
{
	if( flirIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->getReadoutArea(x, y, width, height);
	return ERROR;
}


int setReadoutArea(int camHandle, int x, int y, int width, int height)
{
	if( flirIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->setReadoutArea(x, y, width, height);
	return ERROR;
}


int setObjectParameters(int camHandle, double reflectedTemperature, double atmosphericTemperature, 
											double objectDistance, double objectEmissivity, 
											double relativeHumidity, double extOpticsTemperature, 
											double extOpticsTransmission, double estimatedTransmission)
{
	if( flirIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->setObjectParameters(reflectedTemperature, atmosphericTemperature, objectDistance, objectEmissivity, relativeHumidity, extOpticsTemperature, extOpticsTransmission, estimatedTransmission);
	return ERROR;
}


int setMeasurementRange(int camHandle, int measRange)
{
	if( flirIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->setMeasurementRange(measRange);
	return ERROR;
}


int getFocusAbsPosition(int camHandle, int *focusPos)
{
	if( flirIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->getFocusAbsPosition(focusPos);
	return ERROR;
}


int setFocusAbsPosition(int camHandle, int focusPos)
{
	if( flirIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->setFocusAbsPosition(focusPos);
	return ERROR;
}


int executeAutoFocus(int camHandle)
{
	if( flirIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->executeAutoFocus();
	return ERROR;
}


int setCalibMode(int camHandle, int calMode)
{
	if( flirIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->setCalibMode(calMode);
	return ERROR;
}


int executeAutoCalib(int camHandle)
{
	if( flirIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->executeAutoCalib();
	return ERROR;
}


int startAcquisition(int camHandle, int *width, int *height, int *payloadSize)
{
	if( flirIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->startAcquisition(width, height, payloadSize);
	return ERROR;
}


int stopAcquisition(int camHandle)
{
	if( flirIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->stopAcquisition();
	return ERROR;
}


int getFrame(int camHandle, int *status, void *frame, void *metaData)
{
	if( flirIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->getFrame(status, frame, metaData);
	return ERROR;
}


int frameConv(int camHandle, unsigned short *frame, int width, int height)
{
	if( flirIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->frameConv(frame, width, height);
	return ERROR;
}

int setAcquisitionMode( int camHandle,  int storeEnabled,  int acqSkipFrameNumber)
{
	if( flirIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->setAcquisitionMode(  storeEnabled, acqSkipFrameNumber);
	return ERROR;
}

int setTriggerMode( int camHandle, int triggerMode, double burstDuration, int numTrigger )
{
	if( flirIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->setTriggerMode(  triggerMode,  burstDuration, numTrigger );
	return ERROR;
}

int softwareTrigger(int camHandle)
{
	if( flirIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->softwareTrigger();
	return ERROR;
}

int setTreeInfo( int camHandle,  void *treePtr, int framesNid, int timebaseNid, int framesMetadNid, int frame0TimeNid)
{
	if( flirIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->setTreeInfo( treePtr,  framesNid,  timebaseNid,  framesMetadNid, frame0TimeNid );
	return ERROR;
}

int setStreamingMode(int camHandle, IRFMT_ENUM irFormat, int streamingEnabled, bool autoAdjustLimit, 
						const char *streamingServer, int streamingPort, int lowLim, int highLim, int adjRoiX, int adjRoiY, int adjRoiW, int adjRoiH, const char *deviceName)
{
	if( flirIsConnected( camHandle ) == SUCCESS )
		return camPtr[camHandle]->setStreamingMode( irFormat, streamingEnabled,  autoAdjustLimit, streamingServer, streamingPort, lowLim, highLim, adjRoiX, adjRoiY, adjRoiW, adjRoiH, deviceName);
	return ERROR;

}
int startFramesAcquisition(int camHandle)
{
	if( flirIsConnected( camHandle ) == SUCCESS )
	    return camPtr[camHandle]->startFramesAcquisition();
	return ERROR;
}

int stopFramesAcquisition(int camHandle)
{
	if( flirIsConnected( camHandle ) == SUCCESS )
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




FLIR_SC65X::FLIR_SC65X(const char *ipAddress)
{
    this->ipAddress = PvString(ipAddress); 
/* SDK 3 
    this->lDevice = new PvDevice();
    this->lResult = this->lDevice->Connect(this->ipAddress, PvAccessExclusive);	
*/
    this->lDevice = PvDevice::CreateAndConnect(this->ipAddress, &this->lResult); //20160309 SDK4	 
    if ( !this->lResult.IsOK() ) 
    {
        printf("--------------------------------- Error Device connection: (%s)\n", this->lResult.GetCodeString().GetAscii() ); 
        PvDevice::Free(this->lDevice);
    }
    else
    {    		
	printf("---------------------------- OK Device connection: (%s)\n", this->lResult.GetDescription().GetAscii() ); 
    }
}


FLIR_SC65X::FLIR_SC65X()  //new 25/07/2013: let to use the device without the camera
{
    //this->lDevice = new PvDevice();     //SDK 3
    //SDK 4  09/03/2016  NON sembra possibile creare un PVDEVICE se non ho la telecamera
}


FLIR_SC65X::~FLIR_SC65X()
{
    this->lResult = this->lDevice->Disconnect();
    if ( !this->lResult.IsOK() ) 
	printLastError("Error Device disconnection !!!\n(%s)\n", lResult.GetDescription().GetAscii() ); 
    else
	printf("FLIR_SC65X destructor class executed\n");
    //delete(this->lDevice); 	//SDK 3
    PvDevice::Free(this->lDevice);  //SDK 4  09/03/2016
}



int FLIR_SC65X::checkLastOp()
{
	printf("Info %d (%s)\n", this->lResult.IsOK(), lResult.GetDescription().GetAscii() ); 
	if ( !this->lResult.IsOK() ) 
		printLastError("(%s)\n", lResult.GetDescription().GetAscii() ); 
    return this->lResult.IsOK();
}



int FLIR_SC65X::printAllParameters()
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



int FLIR_SC65X::setExposureMode(EXPMODE_ENUM exposureMode)
{
    PvGenParameterArray *lDeviceParams = lDevice->GetParameters();
	PvGenInteger *currPort = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "CurrentPort" ) );
	PvGenEnum *currCfg = dynamic_cast<PvGenEnum *>( lDeviceParams->Get( "CurrentIOConfig" ) );

	this->lResult = currPort->SetValue( 0 );
	if ( !this->lResult.IsOK() ) {printLastError("Error setting current port to 0!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 

	switch(exposureMode)
	{
	  case internal_mode:
usleep(3000); 	
		this->lResult = currCfg->SetValue( "GeneralPurpose" );
		if ( !this->lResult.IsOK() ) 
			{printLastError("Error setting exposure mode!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 
		else
			{printf("Sync. INTERNAL configured.\n");} 
		break;
	  case external_mode:
usleep(3000); 
		this->lResult = currCfg->SetValue( "MarkImage" );
		if ( !this->lResult.IsOK() ) 
			{printLastError("Error setting exposure mode!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 
		else
			{printf("Sync. EXTERNAL configured.\n");} 
		break;
	}

	return SUCCESS;
}

int FLIR_SC65X::setFrameRate(double frameRate)
{
	FPS_ENUM fps;
            
    if ( frameRate == 200.)
       fps = fps_200;
    else if ( fabs( frameRate - 100. ) < 1.e-4 ) 
       fps = fps_100;
    else if ( fabs( frameRate - 50. ) < 1.e-4 ) 
       fps = fps_50;
    else if ( fabs( frameRate - 25. ) < 1.e-4 ) 
       fps = fps_25;
    else if ( fabs( frameRate - 12.5) < 1.e-4 ) 
       fps = fps_12;
    else if ( fabs( frameRate - 6.25) < 1.e-4 ) 
       fps = fps_6;
    else if ( fabs( frameRate - 3.12) < 1.e-4 ) 
       fps = fps_3;
       
	this->frameRate = frameRate;
    
    return ( setFrameRate(fps, &streamingSkipFrameNumber) );

}

int FLIR_SC65X::setFrameRate(FPS_ENUM fps, int *frameToSkip)
{
    PvGenParameterArray *lDeviceParams = lDevice->GetParameters();
	PvGenInteger *lWidth = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "Width" ) );
	PvGenInteger *lHeight = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "Height" ) );
	PvGenEnum *sWindowing = dynamic_cast<PvGenEnum *>( lDeviceParams->Get( "IRWindowing" ) );
	PvGenEnum *sFrameRate = dynamic_cast<PvGenEnum *>( lDeviceParams->Get( "IRFrameRate" ) );


	switch(fps)
	{
		case fps_200: 
usleep(3000);	
			this->lResult = lWidth->SetValue( 640 );
			 if ( !this->lResult.IsOK() ) {printLastError("Error setting Width in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 
usleep(3000);
			this->lResult = lHeight->SetValue( 123 );
			 if ( !this->lResult.IsOK() ) {printLastError("Error setting Height in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 
usleep(3000);
			this->lResult = sFrameRate->SetValue(0);
		 	 if ( !this->lResult.IsOK() ) {printLastError("Error setting FrameRate in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(3000);
			this->lResult = sWindowing->SetValue("Quarter");//"Quarter"
		 	 if ( !this->lResult.IsOK() ) {printLastError("Error setting Windowing in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}

#ifdef debug
			printf("Frame rate set to 200Hz.\n");
#endif
			*frameToSkip = 8;
			break;

		case fps_100: 
usleep(3000);
			this->lResult = lWidth->SetValue( 640 );
			 if ( !this->lResult.IsOK() ) {printLastError("Error setting Width in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 
usleep(3000);
			this->lResult = lHeight->SetValue( 243 );
			 if ( !this->lResult.IsOK() ) {printLastError("Error setting Height in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(3000);
			this->lResult = sFrameRate->SetValue(0);
		 	 if ( !this->lResult.IsOK() ) {printLastError("Error setting FrameRate in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(3000);
			this->lResult = sWindowing->SetValue("Half"); //"Half"
		 	 if ( !this->lResult.IsOK() ) {printLastError("Error setting Windowing in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}

#ifdef debug
			printf("Frame rate set to 100Hz.\n");
#endif
			*frameToSkip = 4;
			break;

		case fps_50:
usleep(3000);        
			this->lResult = sFrameRate->SetValue(0);
		 	 if ( !this->lResult.IsOK() ) {printLastError("Error setting FrameRate in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(3000);
			this->lResult = sWindowing->SetValue("Off"); //"Off"
		 	 if ( !this->lResult.IsOK() ) {printLastError("Error setting Windowing in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(3000);
			this->lResult = lWidth->SetValue( 640 );
			 if ( !this->lResult.IsOK() ) {printLastError("Error setting Width in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 
usleep(3000);
			this->lResult = lHeight->SetValue( 483 );
			 if ( !this->lResult.IsOK() ) {printLastError("Error setting Height in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
#ifdef debug
			printf("Frame rate set to 50Hz.\n");
#endif
			*frameToSkip = 2;
			break;

		case fps_25:
usleep(3000);        
			this->lResult = sFrameRate->SetValue(1);
		 	 if ( !this->lResult.IsOK() ) {printLastError("Error setting FrameRate in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(3000);
			this->lResult = sWindowing->SetValue("Off"); //"Off"
		 	 if ( !this->lResult.IsOK() ) {printLastError("Error setting Windowing in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(3000);
			this->lResult = lWidth->SetValue( 640 );
			 if ( !this->lResult.IsOK() ) {printLastError("Error setting Width in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 
usleep(3000);
			this->lResult = lHeight->SetValue( 483 );
			 if ( !this->lResult.IsOK() ) {printLastError("Error setting Height in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
#ifdef debug
			printf("Frame rate set to 25Hz.\n");
#endif
			*frameToSkip = 1;
			break;

		case fps_12:
usleep(3000);         
			this->lResult = sFrameRate->SetValue(2);
		 	 if ( !this->lResult.IsOK() ) {printLastError("Error setting FrameRate in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(3000);
			this->lResult = lWidth->SetValue( 640 );
			 if ( !this->lResult.IsOK() ) {printLastError("Error setting Width in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 
usleep(3000);
			this->lResult = lHeight->SetValue( 483 );
			 if ( !this->lResult.IsOK() ) {printLastError("Error setting Height in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(3000);
			this->lResult = sWindowing->SetValue("Off"); //"Off"
		 	 if ( !this->lResult.IsOK() ) {printLastError("Error setting Windowing in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
#ifdef debug
			printf("Frame rate set to 12.5Hz.\n");
#endif
			*frameToSkip = 1;
			break;

		case fps_6:
usleep(3000);        
			this->lResult = sFrameRate->SetValue(3);
		 	 if ( !this->lResult.IsOK() ) {printLastError("Error setting FrameRate in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(3000);
			this->lResult = lWidth->SetValue( 640 );
			 if ( !this->lResult.IsOK() ) {printLastError("Error setting Width in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 
usleep(3000);
			this->lResult = lHeight->SetValue( 483 );
			 if ( !this->lResult.IsOK() ) {printLastError("Error setting Height in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(3000);
			this->lResult = sWindowing->SetValue("Off"); //"Off"
		 	 if ( !this->lResult.IsOK() ) {printLastError("Error setting Windowing in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
#ifdef debug
			printf("Frame rate set to 6.25Hz.\n");
#endif
			*frameToSkip = 1;
			break;

		case fps_3:
usleep(3000);        
			this->lResult = sFrameRate->SetValue(4);
		 	 if ( !this->lResult.IsOK() ) {printLastError("Error setting FrameRate in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(3000);
			this->lResult = lWidth->SetValue( 640 );
			 if ( !this->lResult.IsOK() ) {printLastError("Error setting Width in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 
usleep(3000);
			this->lResult = lHeight->SetValue( 483 );
			 if ( !this->lResult.IsOK() ) {printLastError("Error setting Height in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(3000);
			this->lResult = sWindowing->SetValue("Off"); //"Off"
		 	 if ( !this->lResult.IsOK() ) {printLastError("Error setting Windowing in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
#ifdef debug
			printf("Frame rate set to 3.12Hz.\n");
#endif
			*frameToSkip = 1;
			break;

		default:
			printLastError("Invalid frame rate value!!!\n", 0); 
			return ERROR;
	}

usleep(3500000);
	
    return SUCCESS;
}



int FLIR_SC65X::setIrFormat(IRFMT_ENUM irFormat)
{
    PvGenParameterArray *lDeviceParams = lDevice->GetParameters();
	PvGenEnum *sFormat = dynamic_cast<PvGenEnum *>( lDeviceParams->Get( "IRFormat" ) );

	switch(irFormat)
	{
		case radiometric:
			this->lResult = sFormat->SetValue("Radiometric");
			if ( !this->lResult.IsOK() ) 
				{printLastError("Error setting Radiometric in setIrFormat!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 
			else
				{printf("IR FORMAT set to radiometric.\n");} 
			break;
		case linear100mK:
			this->lResult = sFormat->SetValue("TemperatureLinear100mK"); 
			if ( !this->lResult.IsOK() ) 
				{printLastError("Error setting TemperatureLinear100mK!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 
			else
				{printf("IR FORMAT set to TemperatureLinear100mK.\n");} 
			break;
		case linear10mK:
			this->lResult = sFormat->SetValue("TemperatureLinear10mK"); 
			if ( !this->lResult.IsOK() ) 
				{printLastError("Error setting TemperatureLinear10mK!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 
			else
				{printf("IR FORMAT set to TemperatureLinear10mK.\n");} 
			break;
	}

	this->irFrameFormat = irFormat;


	return SUCCESS;
}



int FLIR_SC65X::getReadoutArea(int *x, int *y, int *width, int *height)
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
	this->height = *height=(int)h-3;	//remove 3 lines of metadata  


    return SUCCESS;
}



int FLIR_SC65X::setReadoutArea(int x, int y, int width, int height)
{
    PvGenParameterArray *lDeviceParams = lDevice->GetParameters();
	PvGenInteger *lWidth = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "Width" ) );
	PvGenInteger *lHeight = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "Height" ) );
	PvGenInteger *lOffsetX = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "OffsetX" ) );
	PvGenInteger *lOffsetY = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "OffsetY" ) );

	int64_t w = width;
	int64_t h = height;
	int64_t ox = x;
	int64_t oy = y;


	this->lResult = lWidth->SetValue(w);
	if ( !this->lResult.IsOK() ) {printLastError("Error setting Width in setReadoutArea\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(3000);
	this->lResult = lHeight->SetValue(h+3);  //add 3 lines of metadata
	if ( !this->lResult.IsOK() ) {printLastError("Error setting Height in setReadoutArea\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(3000);
	this->lResult = lOffsetX->SetValue(ox);
	if ( !this->lResult.IsOK() ) {printLastError("Error setting OffsetX in setReadoutArea\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(3000);
	this->lResult = lOffsetY->SetValue(oy);
	if ( !this->lResult.IsOK() ) {printLastError("Error setting OffsetY in setReadoutArea\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}

	this->x = x;
	this->y = y;  
	this->width = width;
	this->height = height;  


    return SUCCESS;
}



int FLIR_SC65X::setObjectParameters(double reflectedTemperature, double atmosphericTemperature, double objectDistance, double objectEmissivity, double relativeHumidity, double extOpticsTemperature, double extOpticsTransmission, double estimatedTransmission)	
{
    PvGenParameterArray *lDeviceParams = lDevice->GetParameters();
	PvGenFloat *RefTemp = dynamic_cast<PvGenFloat *>( lDeviceParams->Get( "ReflectedTemperature" ) );
	PvGenFloat *AtmTemp = dynamic_cast<PvGenFloat *>( lDeviceParams->Get( "AtmosphericTemperature" ) );
	PvGenFloat *ObjDist = dynamic_cast<PvGenFloat *>( lDeviceParams->Get( "ObjectDistance" ) );
	PvGenFloat *ObjEmis = dynamic_cast<PvGenFloat *>( lDeviceParams->Get( "ObjectEmissivity" ) );
	PvGenFloat *RelHumi = dynamic_cast<PvGenFloat *>( lDeviceParams->Get( "RelativeHumidity" ) );
	PvGenFloat *EopTemp = dynamic_cast<PvGenFloat *>( lDeviceParams->Get( "ExtOpticsTemperature" ) );
	PvGenFloat *EopTran = dynamic_cast<PvGenFloat *>( lDeviceParams->Get( "ExtOpticsTransmission" ) );
	PvGenFloat *EstTran = dynamic_cast<PvGenFloat *>( lDeviceParams->Get( "EstimatedTransmission" ) );



	this->lResult = RefTemp->SetValue(reflectedTemperature + 273.15);
	if ( !this->lResult.IsOK() ) {printLastError("Error setting reflectedTemperature!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}

usleep(3000);
	this->lResult = AtmTemp->SetValue(atmosphericTemperature + 273.15);
	if ( !this->lResult.IsOK() ) {printLastError("Error setting atmosphericTemperature!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}

usleep(3000);
	this->lResult = ObjDist->SetValue(objectDistance);
	if ( !this->lResult.IsOK() ) {printLastError("Error setting objectDistance!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}

usleep(3000);
	this->lResult = ObjEmis->SetValue(objectEmissivity);
	if ( !this->lResult.IsOK() ) {printLastError("Error setting objectEmissivity!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}

usleep(3000);
	this->lResult = RelHumi->SetValue(relativeHumidity);
	if ( !this->lResult.IsOK() ) {printLastError("Error setting relativeHumidity!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}

usleep(3000);
	this->lResult = EopTemp->SetValue(extOpticsTemperature + 273.15);
	if ( !this->lResult.IsOK() ) {printLastError("Error setting extOpticsTemperature!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}

usleep(3000);
	this->lResult = EopTran->SetValue(extOpticsTransmission);
	if ( !this->lResult.IsOK() ) {printLastError("Error setting extOpticsTransmission!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}

usleep(3000);
	this->lResult = EstTran->SetValue(estimatedTransmission);
	if ( !this->lResult.IsOK() ) {printLastError("Error setting estimatedTransmission!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}

    return SUCCESS;
}



int FLIR_SC65X::setMeasurementRange(int measRange)
{

         // with new FLIR CAMERAS range is 0-5 instead of 0-2
	if(measRange<0 or measRange>5)
	{
	  printLastError("Error: measRange in setMeasurementRange must be in range 0-5!!!\n", 0);
	  return ERROR;
	}

        printf("Try setting MeasurementRange to %d. REMEMBER to select the correct focal lenght!");

    PvGenParameterArray *lDeviceParams = lDevice->GetParameters();
	PvGenInteger *currCase = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "CurrentCase" ) );
	PvGenInteger *queryCase = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "QueryCase" ) );
	PvGenFloat *lowLim = dynamic_cast<PvGenFloat *>( lDeviceParams->Get( "QueryCaseLowLimit" ) );
	PvGenFloat *highLim = dynamic_cast<PvGenFloat *>( lDeviceParams->Get( "QueryCaseHighLimit" ) );

	PvGenBoolean *queryCaseEnabled = dynamic_cast<PvGenBoolean *>( lDeviceParams->Get( "QueryCaseEnabled" ) );

	int64_t val = measRange;

    this->lResult = queryCase->SetValue(val);
	 if ( !this->lResult.IsOK() ) {printLastError("Error: case not defined in setMeasurementRange!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 

usleep(3000);
     this->lResult = currCase->SetValue(val);
	 if ( !this->lResult.IsOK() ) {printLastError("Error in setMeasurementRange!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 

	double limitLow, limitHigh = 0;
usleep(3000);

    this->lResult = lowLim->GetValue(limitLow);
	if ( !this->lResult.IsOK() ) {printLastError("Error in low limit !!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 

usleep(3000);

    this->lResult = highLim->GetValue(limitHigh);
	if ( !this->lResult.IsOK() ) {printLastError("Error in high limit !!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 


#ifdef debug
	printf("Measurement range set to %.0f...%.0f\n",(limitLow-273.15) ,(limitHigh-273.15) );
#endif

    return SUCCESS;
}



int FLIR_SC65X::getFocusAbsPosition(int *focusPos)
{
    PvGenParameterArray *lDeviceParams = lDevice->GetParameters();
    PvGenInteger *lfocusPos = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "FocusPos" ) );

    int64_t val = 0;

    this->lResult = lfocusPos->GetValue( val );
    if ( !this->lResult.IsOK() ) {printLastError("Error getting Focus Absolute Position\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}

    printf("getFocusAbsPosition val: %d\n", val);

    *focusPos=val;
    
    return SUCCESS;
}



int FLIR_SC65X::setFocusAbsPosition(int focusPos)
{
   PvGenParameterArray *lDeviceParams = lDevice->GetParameters();
   PvGenInteger *lfocusPos = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "FocusPos" ) );
   PvGenInteger *lfocusSpeed = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "FocusSpeed" ) );
   PvGenInteger *lfocusStep = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "FocusStep" ) );
   PvGenCommand *lfocusIncrement = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "FocusIncrement" ) );
   PvGenCommand *lfocusDecrement = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "FocusDecrement" ) );

   int64_t val = 0;

    this->lResult = lfocusPos->GetValue( val );
    if ( !this->lResult.IsOK() ) {printLastError("Error getting Focus Absolute Position\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}

    this->lResult = lfocusSpeed->SetValue(1); //maybe not necessary
    if ( !this->lResult.IsOK() ) {printLastError("Error setting focus speed\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}

    int currFocPos = (int)val;
    int newFocPos = focusPos;

    this->lResult = lfocusPos->SetValue( newFocPos );  //set is not fine as read!!! maybe motor is not a step by step one.
    if ( !this->lResult.IsOK() ) {printLastError("Error setting Focus Absolute Position\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}

    return SUCCESS;
}



int FLIR_SC65X::executeAutoFocus()
{
    PvGenParameterArray *lDeviceParams = lDevice->GetParameters();
    PvGenCommand *autoFocus = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "AutoFocus" ) );
  
    this->lResult = autoFocus->Execute();
	 if ( !this->lResult.IsOK() ) {printLastError("Error executing auto focus!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 	

#ifdef debug
	printf("AutoFocus Executed.\n");
#endif

    return SUCCESS;
}



int FLIR_SC65X::setCalibMode(int calMode) //0=off     1=automatic
{
    PvGenParameterArray *lDeviceParams = lDevice->GetParameters();
	PvGenEnum *calibMode = dynamic_cast<PvGenEnum *>( lDeviceParams->Get( "NUCMode" ) );

    this->lResult = calibMode->SetValue( calMode );
	if ( !this->lResult.IsOK() ) 
	  {printLastError("Error setting calibration mode!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 

	PvString lValue;
	PvGenParameter *lGenParameter = lDeviceParams->Get( "NUCMode" );			
	static_cast<PvGenEnum *>( lGenParameter )->GetValue( lValue );


	this->autoCalibration = calMode;

	printf( "Calibration Mode set to: %s\n", lValue.GetAscii() );

    return SUCCESS;
}



int FLIR_SC65X::executeAutoCalib()
{
    PvGenParameterArray *lDeviceParams = lDevice->GetParameters();
    PvGenCommand *autoCalib = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "NUCAction" ) );
  
    this->lResult = autoCalib->Execute();
	 if ( !this->lResult.IsOK() ) {printLastError("Error executing calibration!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 	

#ifdef debug
	printf("AutoCalibration Executed.\n");
#endif

    return SUCCESS;
}



int FLIR_SC65X::startAcquisition(int *width, int *height, int *payloadSize)
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
	this->height = *height= (int)h-3;  				//first 3 rows are metadata

	*payloadSize=lSize;  //payload = width*height*2 + metadata


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
  
    // Open stream
    //this->lResult = this->lStream->Open( this->ipAddress);    //SDK 3
    this->lStream = PvStream::CreateAndOpen(this->ipAddress, &this->lResult); //20160309 SDK4	 
    if ( ( this->lStream == NULL ) || !this->lResult.IsOK() )
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
    this->lResult = lResetTimestamp->Execute();
	if ( !this->lResult.IsOK() ) {printLastError("Error resetting timestamp counter!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 

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
  


int FLIR_SC65X::stopAcquisition()
{
    PvGenParameterArray *lDeviceParams = lDevice->GetParameters();
    PvGenInteger *lTLLocked = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "TLParamsLocked" ) );
    PvGenCommand *lStop = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "AcquisitionStop" ) );

    // Tell the device to stop sending images
    this->lResult = lStop->Execute();
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


int FLIR_SC65X::getFrame(int *status, void *frame, void *metaData)
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
    this->lResult = lStream->RetrieveBuffer( &lBuffer, &lOperationResult, 1000 );

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
				memcpy( frame , (unsigned char *)dataPtr, width*2*(height-3) );
				memcpy( metaData , (unsigned char *)dataPtr+(width*2*(height-3)), width*2*3 );		
			
				*status=1; //complete

				FPGA_HEADER* pFPGA;
				pFPGA = (FPGA_HEADER*)metaData;	

				//if(pFPGA->dp1_trig_state)  //NOT WORK!!!!!!!!!!
				if(pFPGA->dp1_trig_type & FPGA_TRIG_TYPE_MARK)  //correct way :-)
				{
				    //printf("\nTrigger dp1_trig_state %d \n", pFPGA->dp1_trig_state);
				    //printf("\nTrigger dp1_trig_type  %d \n", pFPGA->dp1_trig_type);
					printf("External trigger!\n");
					*status=4; //complete + triggered!
					triggered = 1;
				}

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




//Convert frame pixels from Kelvin to Celsius
int FLIR_SC65X::frameConv(unsigned short *frame, int width, int height)
{
    short sub=0;
    switch(irFrameFormat)
    {   
	  case linear100mK:     sub=2731;  //1 = 100mK = 100mC -> precisione decimo di grado
                    break;
	  case linear10mK:      sub=27315; //1 = 10mK = 10mC -> precisione centesimo di grado
                    break;                       
    }

    for(int i=0; i<width*height; i++) 
    {
       frame[i] = frame[i] - sub;
    }
	
    return SUCCESS;

}

int FLIR_SC65X::setStreamingMode( IRFMT_ENUM irFormat, int streamingEnabled,  bool autoAdjustLimit, const char *streamingServer, int streamingPort, unsigned int lowLim, unsigned int highLim, int adjRoiX, int adjRoiY, int adjRoiW, int adjRoiH, const char *deviceName)
{
   this->streamingEnabled = streamingEnabled;
	
   if( streamingEnabled )
   {
		memcpy( this->streamingServer, streamingServer, strlen(streamingServer)+1 );
		memcpy( this->deviceName, deviceName, strlen(deviceName)+1 );
   		this->streamingPort = streamingPort;
		this->autoAdjustLimit = autoAdjustLimit;

	//for FLIR 655
  	//unsigned int minLim = 2000; // 200 K or -73 deg Celsius
  	//unsigned int maxLim = 62000; // 6200 K or 5927 deg Celsius

		switch(irFormat)
		{
			case radiometric:
				this->lowLim = lowLim * 10;   //20170918: streaming is already converted in temperature. Radiometric frames are never send.
				this->highLim = highLim * 10;
          		        minLim= 0;            
          		        maxLim= 62000-27315; //3468.5°C 
				break;
			case linear100mK:
				this->lowLim = lowLim * 10;
				this->highLim = highLim * 10;
          		        minLim= 0;            
          		        maxLim= 62000-27315;  //3468.5°C 
				break;
			case linear10mK:
				this->lowLim = lowLim * 100;
				this->highLim = highLim * 100;
          		        minLim= 0;            
          		        maxLim= 62000-27315; //346.85°C
				break;
		}

	this->adjRoiX = adjRoiX;
	this->adjRoiY = adjRoiY;
	this->adjRoiW = adjRoiW;
	this->adjRoiH = adjRoiH;

   }
   return SUCCESS;
} 


int FLIR_SC65X::setAcquisitionMode( int storeEnabled , int acqSkipFrameNumber)
{

   this->storeEnabled = storeEnabled;
   this->acqSkipFrameNumber = acqSkipFrameNumber;
   return SUCCESS;
}

int FLIR_SC65X::setTriggerMode( int triggerMode, double burstDuration, int numTrigger )
{
	this->triggerMode = triggerMode;
	this->burstDuration = burstDuration;
	this->numTrigger = numTrigger;

	return setExposureMode((EXPMODE_ENUM) triggerMode);
}

int FLIR_SC65X::softwareTrigger()
{
	this->startStoreTrg = 1; 
	return SUCCESS;
}

int FLIR_SC65X::setTreeInfo( void *treePtr, int framesNid, int timebaseNid, int framesMetadNid, int frame0TimeNid)
{
	this->treePtr = treePtr ;
	this->framesNid = framesNid;
	this->timebaseNid = timebaseNid;
	this->framesMetadNid = framesMetadNid;
        this->frame0TimeNid = frame0TimeNid;

	return SUCCESS;
}


void FLIR_SC65X::printLastError(const char *format, const char *msg)
{
	error[0] = 0;
	if(msg)
		sprintf(error, format, msg);
	else
		sprintf(error, format);
}


void FLIR_SC65X::getLastError(char *msg)
{
	sprintf(msg, "%s", (error[0]==0) ? "" : error);
}

int FLIR_SC65X::stopFramesAcquisition()
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
		printLastError("Cannot stop acquisition loop", 0);
		return ERROR;
	}
		

	return SUCCESS;
}

int FLIR_SC65X::startFramesAcquisition()
{
	int frameTriggerCounter;
	int frameCounter;
	int frameStatus;
//	int startStoreTrg = 0;  //moved outside to let the device call the softwareTrigger() function
	int NtriggerCount = 0;
	int burstNframe;
	int rstatus;
	int tcpStreamHandle = -1;
	int metaSize;
	int enqueueFrameNumber;

	float frameTime = 0.0;

	void *saveList;
	void *streamingList;

	short *frameBuffer;
	short *metaData;
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
   
	frameBuffer = (short *) calloc(1, width * height * sizeof(short));
	frame8bit = (unsigned char *) calloc(1, width * height * sizeof(char));

	metaSize = width * 3 * sizeof(short);
	metaData = (short *)calloc(1, metaSize);

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

                  if ( autoCalibration )    //execute calibration action @ every burst of frames (only if NO auto calibration)
                  {  
			executeAutoCalib();
		  }

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

        if ( irFrameFormat != radiometric )         
             frameConv((unsigned short *)frameBuffer, width, height);  //convert kelvin in Celsius
           
	//frameStatus -> status=1 complete # status=2 incomplete # status=3 timeout # status=4 triggered frame + complete
	if( (frameStatus != 3 ) && ( storeEnabled == 1 && startStoreTrg == 1 ) && ( acqSkipFrameNumber <= 0 || (frameTriggerCounter % (acqSkipFrameNumber + 1) ) == 0 ) )
	{
	  int frameTimeBaseIdx;
	  frameTimeBaseIdx = NtriggerCount * burstNframe + frameTriggerCounter;
	  //printf("SAVE Frame : %d timebase Idx : %d\n", frameTriggerCounter,  frameTimeBaseIdx);
	
	  // CT la routine camSaveFrame utilizza il frame index in acquisizione. L'indice viene
    	  // utilizzato per individuare nell'array della base temporale il tempo associato al frame.

	  // Con Trigger interno viene utilizzato frameTime come tempo relativo allo 0; timebaseNid deve essere -1

          camSaveFrame((void *)frameBuffer, width, height, frameTime, 14, (void *)treePtr, framesNid, timebaseNid, frameTimeBaseIdx, (void *)metaData, metaSize, framesMetadNid, saveList); 
	  enqueueFrameNumber++;
	} 

        if( streamingEnabled )
	{
           if( tcpStreamHandle == -1) 
	   {
            	rstatus = camOpenTcpConnection(streamingServer, streamingPort, &tcpStreamHandle, width, height, CSU_PIX_FMT_GRAY16);
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
                if( irFrameFormat == radiometric ) 
                {
                  flirRadiometricConv(frameBuffer, width, height, metaData);   //radiometric conversion in Celsius using metadata
                }
	        camStreamingFrame( tcpStreamHandle, frameBuffer, width, height, CSU_PIX_FMT_GRAY16, irFrameFormat, autoAdjustLimit, &lowLim, &highLim, minLim, maxLim, adjRoiX, adjRoiY, adjRoiW, adjRoiH, this->deviceName, streamingList);
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
	printf("Cannot stop camera acquisition\n");

    if ( !autoCalibration )
        setCalibMode(1);  //re-enable auto calibration

    
    free(frameBuffer);
    free(frame8bit);
    free(metaData);

	printf("Acquisition Statistics : \tTotal frames read %d, \n\t\t\t\tTotal frames stored %d (expected %d), \n\t\t\t\tNumber of trigger %d (expected %d), \n\t\t\t\tIncomplete frame %d\n", frameCounter, enqueueFrameNumber, 1 + numTrigger * ((int)( burstDuration * (frameRate-acqSkipFrameNumber))), NtriggerCount, numTrigger, incompleteFrame );

	acqStopped = 1;

	return rstatus;
}

