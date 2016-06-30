#include <vector>
#include <string.h>
#include <stdio.h>
using namespace std;

#include "FLIRSC65X.h"
#include "fff.h"
#include <libcammdsutils.h>
#include <libcamstreamutils.h>
#include "flirutils.h"

#include <math.h>

#define BUFFER_COUNT 16 

#define ERROR -1
#define SUCCESS 0

//if defined, print to stdout messages.
#define debug


#define MAX_CAM 10
static FLIR_SC65X *camPtr[MAX_CAM] = {0};


#ifdef __cplusplus 
extern "C" 
{ 
#endif 


int flirOpen(const char *ipAddress, int *camHandle)
{
	FLIR_SC65X *cam;
	int cameraHandle;

	cam = new FLIR_SC65X(ipAddress);
	if(!cam->checkLastOp())
	{
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
		*camHandle = -1;
		return ERROR;
	}
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
	if(camPtr[camHandle] == 0 ) 
		return ERROR;
	return camPtr[camHandle]->printAllParameters();
}


int setExposureMode(int camHandle, EXPMODE_ENUM exposureMode)
{
	if(camPtr[camHandle] == 0 ) 
		return ERROR;
	return camPtr[camHandle]->setExposureMode(exposureMode);
}


int setFrameRate(int camHandle, FPS_ENUM frameRate, int *frameToSkip)
{
	if(camPtr[camHandle] == 0 ) 
		return ERROR;
	return camPtr[camHandle]->setFrameRate(frameRate, frameToSkip);
}


int setFrameRateNew(int camHandle, double frameRate)
{
	if(camPtr[camHandle] == 0 ) 
		return ERROR;
	return camPtr[camHandle]->setFrameRate(frameRate);
}

int setIrFormat(int camHandle, IRFMT_ENUM irFormat)
{
	if(camPtr[camHandle] == 0 ) 
		return ERROR;
	return camPtr[camHandle]->setIrFormat(irFormat);
}


int getReadoutArea(int camHandle, int *x, int *y, int *width, int *height)
{
	if(camPtr[camHandle] == 0 ) 
		return ERROR;
	return camPtr[camHandle]->getReadoutArea(x, y, width, height);
}


int setReadoutArea(int camHandle, int x, int y, int width, int height)
{
	if(camPtr[camHandle] == 0 ) 
		return ERROR;
	return camPtr[camHandle]->setReadoutArea(x, y, width, height);
}


int setObjectParameters(int camHandle, double reflectedTemperature, double atmosphericTemperature, 
											double objectDistance, double objectEmissivity, 
											double relativeHumidity, double extOpticsTemperature, 
											double extOpticsTransmission, double estimatedTransmission)
{
	if(camPtr[camHandle] == 0 ) 
		return ERROR;
	return camPtr[camHandle]->setObjectParameters(reflectedTemperature, atmosphericTemperature, objectDistance, objectEmissivity, relativeHumidity, extOpticsTemperature, extOpticsTransmission, estimatedTransmission);
}


int setMeasurementRange(int camHandle, int measRange)
{
	if(camPtr[camHandle] == 0 ) 
		return ERROR;
	return camPtr[camHandle]->setMeasurementRange(measRange);
}


int getFocusAbsPosition(int camHandle, int *focusPos)
{
	if(camPtr[camHandle] == 0 ) 
		return ERROR;
	return camPtr[camHandle]->getFocusAbsPosition(focusPos);
}


int setFocusAbsPosition(int camHandle, int focusPos)
{
	if(camPtr[camHandle] == 0 ) 
		return ERROR;
	return camPtr[camHandle]->setFocusAbsPosition(focusPos);
}


int executeAutoFocus(int camHandle)
{
	if(camPtr[camHandle] == 0 ) 
		return ERROR;
	return camPtr[camHandle]->executeAutoFocus();
}


int setCalibMode(int camHandle, int calMode)
{
	if(camPtr[camHandle] == 0 ) 
		return ERROR;
	return camPtr[camHandle]->setCalibMode(calMode);
}


int executeAutoCalib(int camHandle)
{
	if(camPtr[camHandle] == 0 ) 
		return ERROR;
	return camPtr[camHandle]->executeAutoCalib();
}


int startAcquisition(int camHandle, int *width, int *height, unsigned int *payloadSize)
{
	if(camPtr[camHandle] == 0 ) 
		return ERROR;
	return camPtr[camHandle]->startAcquisition(width, height, payloadSize);
}


int stopAcquisition(int camHandle)
{
	if(camPtr[camHandle] == 0 ) 
		return SUCCESS;
	return camPtr[camHandle]->stopAcquisition();
}


int getFrame(int camHandle, int *status, void *frame, void *metaData)
{
	if(camPtr[camHandle] == 0 ) 
		return ERROR;
	return camPtr[camHandle]->getFrame(status, frame, metaData);
}


int frameConv(int camHandle, unsigned short *frame, int width, int height)
{
	if(camPtr[camHandle] == 0 ) 
		return ERROR;
	return camPtr[camHandle]->frameConv(frame, width, height);
}

int setAcquisitionMode( int camHandle,  int storeEnabled,  int acqSkipFrameNumber)
{
	if(camPtr[camHandle] == 0 ) 
		return ERROR;
	return camPtr[camHandle]->setAcquisitionMode(  storeEnabled, acqSkipFrameNumber);
}

int setTriggerMode( int camHandle, int triggerMode, double burstDuration, int numTrigger )
{
	if(camPtr[camHandle] == 0 ) 
		return ERROR;
	return camPtr[camHandle]->setTriggerMode(  triggerMode,  burstDuration, numTrigger );
}

int setTreeInfo( int camHandle,  void *treePtr, int framesNid, int timebaseNid, int framesMetadNid)
{
	if(camPtr[camHandle] == 0 ) 
		return ERROR;
	return camPtr[camHandle]->setTreeInfo( treePtr,  framesNid,  timebaseNid,  framesMetadNid );
}

int setStreamingMode(int camHandle, IRFMT_ENUM irFormat, int streamingEnabled, int autoAdjustLimit, 
						const char *streamingServer, int streamingPort, int lowLim, int highLim)
{
	if(camPtr[camHandle] == 0 ) 
		return ERROR;
	return camPtr[camHandle]->setStreamingMode( irFormat, streamingEnabled,  autoAdjustLimit, streamingServer, streamingPort, lowLim, highLim);

}
int startFramesAcquisition(int camHandle)
{
	if(camPtr[camHandle] == 0 ) 
		return ERROR;
    return camPtr[camHandle]->startFramesAcquisition();
}

int stopFramesAcquisition(int camHandle)
{
	if(camPtr[camHandle] == 0 ) 
		return SUCCESS;
    return camPtr[camHandle]->stopFramesAcquisition();
}

void  getLastError(int camHandle, char *msg)
{
	if(camPtr[camHandle] == 0 ) 
		return;
    camPtr[camHandle]->getLastError(msg);
}


#ifdef __cplusplus 
} 
#endif 




FLIR_SC65X::FLIR_SC65X(const char *ipAddress)
{
    this->ipAddress = PvString(ipAddress);  
    this->lDevice = new PvDevice();
    this->lResult = this->lDevice->Connect(this->ipAddress, PvAccessExclusive);
}


FLIR_SC65X::FLIR_SC65X()  //new 25/07/2013: let to use the device without the camera
{
    this->lDevice = new PvDevice();
}


FLIR_SC65X::~FLIR_SC65X()
{
    this->lResult = this->lDevice->Disconnect();
	if ( !this->lResult.IsOK() ) 
		printLastError("Error Device disconnection !!!\n(%s)\n", lResult.GetDescription().GetAscii() ); 
	else
		printf("Device Disconnected\n");
    delete(this->lDevice); 
}



int FLIR_SC65X::checkLastOp()
{
	if ( !this->lResult.IsOK() ) 
		printLastError("(%s)\n", lResult.GetDescription().GetAscii() ); 
    return this->lResult.IsOK();
}



int FLIR_SC65X::printAllParameters()
{
    PvGenParameterArray *aArray = lDevice->GetGenParameters();
	PvUInt32 lParameterArrayCount = aArray->GetCount();
	printf( "\nArray has %d parameters\n", lParameterArrayCount );

	// Traverse through Array and print out parameters available
	for( PvUInt32 x = 0; x < lParameterArrayCount; x++ )
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
					PvInt64 lValue;				
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
    PvGenParameterArray *lDeviceParams = lDevice->GetGenParameters();
	PvGenInteger *currPort = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "CurrentPort" ) );
	PvGenEnum *currCfg = dynamic_cast<PvGenEnum *>( lDeviceParams->Get( "CurrentIOConfig" ) );

	this->lResult = currPort->SetValue( 0 );
	if ( !this->lResult.IsOK() ) {printLastError("Error setting current port to 0!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 

	switch(exposureMode)
	{
	  case internal_mode:
usleep(300); 	
		this->lResult = currCfg->SetValue( "GeneralPurpose" );
		if ( !this->lResult.IsOK() ) 
			{printLastError("Error setting exposure mode!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 
		else
			{printf("Sync. INTERNAL configured.\n");} 
		break;
	  case external_mode:
usleep(300); 
		this->lResult = currCfg->SetValue( "MarkImage" );
		if ( !this->lResult.IsOK() ) 
			{printLastError("Error setting exposure mode!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 
		else
			{printf("Sync. EXTERNAL configured.\n");} 
		break;
	}

/*
	PvString prova;
    currCfg->GetValue( prova );
	printf("Port 0 configuration: ");
	if(prova=="MarkImage")
		printf("MarkImage\n");
	if(prova=="GeneralPurpose")
		printf("GeneralPurpose\n");
*/
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
    PvGenParameterArray *lDeviceParams = lDevice->GetGenParameters();
	PvGenInteger *lWidth = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "Width" ) );
	PvGenInteger *lHeight = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "Height" ) );
	PvGenEnum *sWindowing = dynamic_cast<PvGenEnum *>( lDeviceParams->Get( "IRWindowing" ) );
	PvGenEnum *sFrameRate = dynamic_cast<PvGenEnum *>( lDeviceParams->Get( "IRFrameRate" ) );


	switch(fps)
	{
		case fps_200: 
usleep(300);	
			this->lResult = lWidth->SetValue( 640 );
			 if ( !this->lResult.IsOK() ) {printLastError("Error setting Width in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 
usleep(300);
			this->lResult = lHeight->SetValue( 123 );
			 if ( !this->lResult.IsOK() ) {printLastError("Error setting Height in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 

			this->lResult = sFrameRate->SetValue(0);
		 	 if ( !this->lResult.IsOK() ) {printLastError("Error setting FrameRate in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(300);
			this->lResult = sWindowing->SetValue("Quarter");//"Quarter"
		 	 if ( !this->lResult.IsOK() ) {printLastError("Error setting Windowing in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}

#ifdef debug
			printf("Frame rate set to 200Hz.\n");
#endif
			*frameToSkip = 8;
			break;

		case fps_100: 
usleep(300);
			this->lResult = lWidth->SetValue( 640 );
			 if ( !this->lResult.IsOK() ) {printLastError("Error setting Width in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 
usleep(300);
			this->lResult = lHeight->SetValue( 243 );
			 if ( !this->lResult.IsOK() ) {printLastError("Error setting Height in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}

			this->lResult = sFrameRate->SetValue(0);
		 	 if ( !this->lResult.IsOK() ) {printLastError("Error setting FrameRate in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(300);
			this->lResult = sWindowing->SetValue("Half"); //"Half"
		 	 if ( !this->lResult.IsOK() ) {printLastError("Error setting Windowing in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}

#ifdef debug
			printf("Frame rate set to 100Hz.\n");
#endif
			*frameToSkip = 4;
			break;

		case fps_50:
			this->lResult = sFrameRate->SetValue(0);
		 	 if ( !this->lResult.IsOK() ) {printLastError("Error setting FrameRate in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(300);
			this->lResult = sWindowing->SetValue("Off"); //"Off"
		 	 if ( !this->lResult.IsOK() ) {printLastError("Error setting Windowing in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(300);
			this->lResult = lWidth->SetValue( 640 );
			 if ( !this->lResult.IsOK() ) {printLastError("Error setting Width in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 
usleep(300);
			this->lResult = lHeight->SetValue( 483 );
			 if ( !this->lResult.IsOK() ) {printLastError("Error setting Height in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
#ifdef debug
			printf("Frame rate set to 50Hz.\n");
#endif
			*frameToSkip = 2;
			break;

		case fps_25:
			this->lResult = sFrameRate->SetValue(1);
		 	 if ( !this->lResult.IsOK() ) {printLastError("Error setting FrameRate in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(300);
			this->lResult = sWindowing->SetValue("Off"); //"Off"
		 	 if ( !this->lResult.IsOK() ) {printLastError("Error setting Windowing in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(300);
			this->lResult = lWidth->SetValue( 640 );
			 if ( !this->lResult.IsOK() ) {printLastError("Error setting Width in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 
usleep(300);
			this->lResult = lHeight->SetValue( 483 );
			 if ( !this->lResult.IsOK() ) {printLastError("Error setting Height in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
#ifdef debug
			printf("Frame rate set to 25Hz.\n");
#endif
			*frameToSkip = 1;
			break;

		case fps_12: 
			this->lResult = sFrameRate->SetValue(2);
		 	 if ( !this->lResult.IsOK() ) {printLastError("Error setting FrameRate in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(300);
			this->lResult = lWidth->SetValue( 640 );
			 if ( !this->lResult.IsOK() ) {printLastError("Error setting Width in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 
usleep(300);
			this->lResult = lHeight->SetValue( 483 );
			 if ( !this->lResult.IsOK() ) {printLastError("Error setting Height in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(300);
			this->lResult = sWindowing->SetValue("Off"); //"Off"
		 	 if ( !this->lResult.IsOK() ) {printLastError("Error setting Windowing in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
#ifdef debug
			printf("Frame rate set to 12.5Hz.\n");
#endif
			*frameToSkip = 1;
			break;

		case fps_6:
			this->lResult = sFrameRate->SetValue(3);
		 	 if ( !this->lResult.IsOK() ) {printLastError("Error setting FrameRate in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(300);
			this->lResult = lWidth->SetValue( 640 );
			 if ( !this->lResult.IsOK() ) {printLastError("Error setting Width in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 
usleep(300);
			this->lResult = lHeight->SetValue( 483 );
			 if ( !this->lResult.IsOK() ) {printLastError("Error setting Height in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(300);
			this->lResult = sWindowing->SetValue("Off"); //"Off"
		 	 if ( !this->lResult.IsOK() ) {printLastError("Error setting Windowing in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
#ifdef debug
			printf("Frame rate set to 6.25Hz.\n");
#endif
			*frameToSkip = 1;
			break;

		case fps_3:
			this->lResult = sFrameRate->SetValue(4);
		 	 if ( !this->lResult.IsOK() ) {printLastError("Error setting FrameRate in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(300);
			this->lResult = lWidth->SetValue( 640 );
			 if ( !this->lResult.IsOK() ) {printLastError("Error setting Width in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 
usleep(300);
			this->lResult = lHeight->SetValue( 483 );
			 if ( !this->lResult.IsOK() ) {printLastError("Error setting Height in setFrameRate!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(300);
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
    PvGenParameterArray *lDeviceParams = lDevice->GetGenParameters();
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
    PvGenParameterArray *lDeviceParams = lDevice->GetGenParameters();
	PvGenInteger *lWidth = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "Width" ) );
	PvGenInteger *lHeight = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "Height" ) );
	PvGenInteger *lOffsetX = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "OffsetX" ) );
	PvGenInteger *lOffsetY = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "OffsetY" ) );

	PvInt64 ox = 0;
	PvInt64 oy = 0;
	PvInt64 w = 0;
	PvInt64 h = 0;


	this->lResult = lWidth->GetValue( w );
	if ( !this->lResult.IsOK() ) {printLastError("Error getting Width in getReadoutArea\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(300);
	this->lResult = lHeight->GetValue( h );
	if ( !this->lResult.IsOK() ) {printLastError("Error getting Height in getReadoutArea\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(300);
	this->lResult = lOffsetX->GetValue( ox );
	if ( !this->lResult.IsOK() ) {printLastError("Error getting OffsetX in getReadoutArea\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(300);
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
    PvGenParameterArray *lDeviceParams = lDevice->GetGenParameters();
	PvGenInteger *lWidth = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "Width" ) );
	PvGenInteger *lHeight = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "Height" ) );
	PvGenInteger *lOffsetX = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "OffsetX" ) );
	PvGenInteger *lOffsetY = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "OffsetY" ) );

	PvInt64 w = width;
	PvInt64 h = height;
	PvInt64 ox = x;
	PvInt64 oy = y;


	this->lResult = lWidth->SetValue(w);
	if ( !this->lResult.IsOK() ) {printLastError("Error setting Width in setReadoutArea\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(300);
	this->lResult = lHeight->SetValue(h+3);  //add 3 lines of metadata
	if ( !this->lResult.IsOK() ) {printLastError("Error setting Height in setReadoutArea\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(300);
	this->lResult = lOffsetX->SetValue(ox);
	if ( !this->lResult.IsOK() ) {printLastError("Error setting OffsetX in setReadoutArea\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
usleep(300);
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
    PvGenParameterArray *lDeviceParams = lDevice->GetGenParameters();
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

usleep(300);
	this->lResult = AtmTemp->SetValue(atmosphericTemperature + 273.15);
	if ( !this->lResult.IsOK() ) {printLastError("Error setting atmosphericTemperature!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}

usleep(300);
	this->lResult = ObjDist->SetValue(objectDistance);
	if ( !this->lResult.IsOK() ) {printLastError("Error setting objectDistance!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}

usleep(300);
	this->lResult = ObjEmis->SetValue(objectEmissivity);
	if ( !this->lResult.IsOK() ) {printLastError("Error setting objectEmissivity!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}

usleep(300);
	this->lResult = RelHumi->SetValue(relativeHumidity);
	if ( !this->lResult.IsOK() ) {printLastError("Error setting relativeHumidity!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}

usleep(300);
	this->lResult = EopTemp->SetValue(extOpticsTemperature + 273.15);
	if ( !this->lResult.IsOK() ) {printLastError("Error setting extOpticsTemperature!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}

usleep(300);
	this->lResult = EopTran->SetValue(extOpticsTransmission);
	if ( !this->lResult.IsOK() ) {printLastError("Error setting extOpticsTransmission!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}

usleep(300);
	this->lResult = EstTran->SetValue(estimatedTransmission);
	if ( !this->lResult.IsOK() ) {printLastError("Error setting estimatedTransmission!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}

    return SUCCESS;
}



int FLIR_SC65X::setMeasurementRange(int measRange)
{
	if(measRange<0 or measRange>2)
	{
	  printLastError("Error: measRange in setMeasurementRange must be 0,1,2!!!\n", 0);
	  return ERROR;
	}

    PvGenParameterArray *lDeviceParams = lDevice->GetGenParameters();
	PvGenInteger *currCase = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "CurrentCase" ) );
	PvGenInteger *queryCase = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "QueryCase" ) );
	PvGenFloat *lowLim = dynamic_cast<PvGenFloat *>( lDeviceParams->Get( "QueryCaseLowLimit" ) );
	PvGenFloat *highLim = dynamic_cast<PvGenFloat *>( lDeviceParams->Get( "QueryCaseHighLimit" ) );

	PvGenBoolean *queryCaseEnabled = dynamic_cast<PvGenBoolean *>( lDeviceParams->Get( "QueryCaseEnabled" ) );

	PvInt64 val = measRange;

    this->lResult = queryCase->SetValue(val);
	 if ( !this->lResult.IsOK() ) {printLastError("Error: case not defined in setMeasurementRange!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 

usleep(300);
     this->lResult = currCase->SetValue(val);
	 if ( !this->lResult.IsOK() ) {printLastError("Error in setMeasurementRange!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 

	double limitLow, limitHigh = 0;
usleep(300);

    this->lResult = lowLim->GetValue(limitLow);
	if ( !this->lResult.IsOK() ) {printLastError("Error in low limit !!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 

usleep(300);

    this->lResult = highLim->GetValue(limitHigh);
	if ( !this->lResult.IsOK() ) {printLastError("Error in high limit !!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 


#ifdef debug
	printf("Measurement range set to %.0f...%.0f\n",(limitLow-273.15) ,(limitHigh-273.15) );
#endif

    return SUCCESS;
}



int FLIR_SC65X::getFocusAbsPosition(int *focusPos)
{
    PvGenParameterArray *lDeviceParams = lDevice->GetGenParameters();
	PvGenInteger *lfocusPos = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "FocusPos" ) );

	PvInt64 val = 0;

	this->lResult = lfocusPos->GetValue( val );
	if ( !this->lResult.IsOK() ) {printLastError("Error getting Focus Absolute Position\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}

	*focusPos=(int)val;

    return SUCCESS;
}



int FLIR_SC65X::setFocusAbsPosition(int focusPos)
{
    PvGenParameterArray *lDeviceParams = lDevice->GetGenParameters();
	PvGenInteger *lfocusPos = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "FocusPos" ) );

	PvInt64 val = focusPos;

	this->lResult = lfocusPos->SetValue(val);
	if ( !this->lResult.IsOK() ) {printLastError("Error setting Focus Absolute Position\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}

    return SUCCESS;
}



int FLIR_SC65X::executeAutoFocus()
{
    PvGenParameterArray *lDeviceParams = lDevice->GetGenParameters();
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
    PvGenParameterArray *lDeviceParams = lDevice->GetGenParameters();
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
    PvGenParameterArray *lDeviceParams = lDevice->GetGenParameters();
    PvGenCommand *autoCalib = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "NUCAction" ) );
  
    this->lResult = autoCalib->Execute();
	 if ( !this->lResult.IsOK() ) {printLastError("Error executing calibration!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 	

#ifdef debug
	printf("AutoCalibration Executed.\n");
#endif

    return SUCCESS;
}



int FLIR_SC65X::startAcquisition(int *width, int *height, unsigned int *payloadSize)
{
    PvGenParameterArray *lDeviceParams = lDevice->GetGenParameters();
    PvGenInteger *lTLLocked = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "TLParamsLocked" ) );
	PvGenInteger *lWidth = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "Width" ) );
	PvGenInteger *lHeight = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "Height" ) );
    PvGenInteger *lPayloadSize = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "PayloadSize" ) );
	PvGenCommand *lResetTimestamp = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "GevTimestampControlReset" ) );
    PvGenCommand *lStart = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "AcquisitionStart" ) );

	PvInt64 w = 0;
	PvInt64 h = 0;
    PvInt64 lSize = 0;

printf("-- Start Streaming \n");

	this->lResult = lWidth->GetValue( w );
	if ( !this->lResult.IsOK() ) {printLastError("Error getting frame width in startAcquisition!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
	this->lResult = lHeight->GetValue( h );
	if ( !this->lResult.IsOK() ) {printLastError("Error getting frame height in startAcquisition!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;}
	this->lResult = lPayloadSize->GetValue( lSize ); 
	if ( !this->lResult.IsOK() ) {printLastError("Error getting payload size in startAcquisition!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 

	this->width = *width = (int)w;
	this->height = *height= (int)h-3;  				//first 3 rows are metadata

	*payloadSize=(unsigned int)lSize;  //payload = width*height*2 + metadata

    // Negotiate streaming packet size
    this->lResult = lDevice->NegotiatePacketSize();
	if ( !this->lResult.IsOK() ) {printLastError("Error negotiating packet size in start acquisition!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 

    // Open stream
    this->lResult = this->lStream.Open( this->ipAddress );
	if ( !this->lResult.IsOK() ) {printLastError("Error opening stream in start acquisition!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 

    // Use min of BUFFER_COUNT and how many buffers can be queued in PvStream
    PvUInt32 lBufferCount = ( lStream.GetQueuedBufferMaximum() < BUFFER_COUNT ) ? 
        lStream.GetQueuedBufferMaximum() : 
        BUFFER_COUNT;

    // Create, alloc buffers
    this->lBuffers = new PvBuffer[ lBufferCount ];

    for ( PvUInt32 i = 0; i < lBufferCount; i++ )
    {
        lBuffers[ i ].Alloc( static_cast<PvUInt32>( lSize ) );
    }

    // Have to set the Device IP destination to the Stream
    this->lResult = lDevice->SetStreamDestination( lStream.GetLocalIPAddress(), lStream.GetLocalPort() );
	if ( !this->lResult.IsOK() ) {printLastError("Error setting stream dest. in start acquisition!!!\n(%s)\n", lResult.GetDescription().GetAscii() ); return ERROR;} 

    // Queue all buffers in the stream
    for ( PvUInt32 i = 0; i < lBufferCount; i++ )
    {
        lStream.QueueBuffer( lBuffers + i );
    }

    // TLParamsLocked is optional but when present, it MUST be set to 1
    // before sending the AcquisitionStart command
    if ( lTLLocked != NULL )
    {
usleep(300);
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
	
	return SUCCESS;	
}
  


int FLIR_SC65X::stopAcquisition()
{
    PvGenParameterArray *lDeviceParams = lDevice->GetGenParameters();
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
    lStream.AbortQueuedBuffers();
    while ( lStream.GetQueuedBufferCount() > 0 )
    {
        PvBuffer *lBuffer = NULL;
        PvResult lOperationResult;
        lStream.RetrieveBuffer( &lBuffer, &lOperationResult );
        //printf( "  Post-abort retrieved buffer: %s\n", lOperationResult.GetCodeString().GetAscii() );
    }

    //printf( "Releasing buffers\n" );
    delete []lBuffers;

    // Now close the stream. Also optionnal but nice to have
    printf( "Closing stream\n" );
    lStream.Close();


    return SUCCESS;
}

int FLIR_SC65X::getFrame(int *status, void *frame, void *metaData)
{
//status=1 complete # status=2 incomplete # status=3 timeout # status=4 triggered frame + complete
    char lDoodle[] = "|\\-|-/";
    static int lDoodleIndex = 0;
    PvInt64 lImageCountVal = 0;
    double lFrameRateVal = 0.0;
    double lBandwidthVal = 0.0;

    PvBuffer *lBuffer = NULL;
    PvResult lOperationResult;


    // Get stream parameters/stats
    PvGenParameterArray *lStreamParams = lStream.GetParameters();
    PvGenInteger   *lCount = dynamic_cast<PvGenInteger *>( lStreamParams->Get( "ImagesCount" ) );
    PvGenFloat *lFrameRate = dynamic_cast<PvGenFloat *>( lStreamParams->Get( "AcquisitionRateAverage" ) );
    PvGenFloat *lBandwidth = dynamic_cast<PvGenFloat *>( lStreamParams->Get( "BandwidthAverage" ) );

	*status=2; //frame incomplete by default
    ++lDoodleIndex %= 6;

    // Retrieve next buffer	
    this->lResult = lStream.RetrieveBuffer( &lBuffer, &lOperationResult, 1000 );

    if ( this->lResult.IsOK() )
    {
      	if(lOperationResult.IsOK())
       	{
			lCount->GetValue( lImageCountVal );
			lFrameRate->GetValue( lFrameRateVal );
			lBandwidth->GetValue( lBandwidthVal );

			// If the buffer contains an image, display width and height
			PvUInt32 lWidth = 0, lHeight = 0;
			if ( lBuffer->GetPayloadType() == PvPayloadTypeImage )
			{
				// Get image specific buffer interface
				PvImage *lImage = lBuffer->GetImage();
				// Read width, height
				lWidth  = lImage->GetWidth();
				lHeight = lImage->GetHeight();
				currTime = lBuffer->GetTimestamp(); 

/*
				if( triggered )
					printf("deltaTime = %d %d frequency %f\n", (currTime - lastTime), lBuffer->GetBlockID(), (currTime - lastTime) * frameRate );
				lastTime = currTime;
*/
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

		//		PvUInt8* dataPtr = lImage->GetDataPointer();  
				PvUInt8* dataPtr = lBuffer->GetDataPointer();

			//last 3 rows of the frame are metadata
				memcpy( frame , (unsigned char *)dataPtr, width*2*(height-3) );
				memcpy( metaData , (unsigned char *)dataPtr+(width*2*(height-3)), width*2*3 );		
			
				*status=1; //complete


				FPGA_HEADER* pFPGA;
				pFPGA = (FPGA_HEADER*)metaData;	


				//if(pFPGA->dp1_trig_state)  //NOT WORK!!!!!!!!!!
				if(pFPGA->dp1_trig_type & FPGA_TRIG_TYPE_MARK)  //correct way :-)
				{
				    printf("\nTrigger dp1_trig_state %d \n", pFPGA->dp1_trig_state);
				    printf("\nTrigger dp1_trig_type  %d \n", pFPGA->dp1_trig_type);
					printf("external trigger!\n");
					*status=4; //complete + triggered!
					triggered = 1;
				}

			}			
        }
        else  //new 28 agosto 2012
    	{   
	  		*status=2; //incomplete		   
      		printf( "%c Incomplete\r", lDoodle[ lDoodleIndex ] );
    	}
		// We have an image - do some processing (...) and VERY IMPORTANT,
		// re-queue the buffer in the stream object
		lStream.QueueBuffer( lBuffer );
		return SUCCESS;
    }
    else// Timeout
    {   
	  *status=3; //timeout		   
      printf( "%c Timeout\r", lDoodle[ lDoodleIndex ] );
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

int FLIR_SC65X::setStreamingMode( IRFMT_ENUM irFormat, int streamingEnabled,  int autoAdjustLimit, 
									const char *streamingServer, int streamingPort, int lowLim, int highLim)
{
   this->streamingEnabled = streamingEnabled;
	

   if( streamingEnabled )
   {
		memcpy( this->streamingServer, streamingServer, strlen(streamingServer) );
   		this->streamingPort = streamingPort;
		this->autoAdjustLimit = autoAdjustLimit;

		switch(irFormat)
		{
			case radiometric:
				this->lowLim = lowLim;
				this->highLim = highLim;
          		minLim= 0;            
          		maxLim= 32767; 
				break;
			case linear100mK:
				this->lowLim = lowLim * 10;
				this->highLim = highLim * 10;
          		minLim= 0;            
          		maxLim= 62000-27315;  //346.85°C
				break;
			case linear10mK:
				this->lowLim = lowLim * 100;
				this->highLim = highLim * 100;
          		minLim= 0;            
          		maxLim= 62000-27315; //3468.5°C
				break;
		}
   }
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

int FLIR_SC65X::setTreeInfo( void *treePtr, int framesNid, int timebaseNid, int framesMetadNid)
{
	this->treePtr = treePtr ;
	this->framesNid = framesNid;
	this->timebaseNid = timebaseNid;
	this->framesMetadNid = framesMetadNid;

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
		usleep(500);
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
	int startStoreTrg = 0;
	int NtriggerCount = 0;
	int burstNframe;
	int rstatus;
	int tcpStreamHandle = -1;
	int metaSize;

	float frameTime;

	void *saveList;
	short *frameBuffer;
	short *metaData;
	unsigned char *frame8bit;


	frameBuffer = (short *) calloc(1, width * height * sizeof(short));
	frame8bit = (unsigned char *) calloc(1, width * height * sizeof(char));

	metaSize = width * 3 * sizeof(short);
	metaData = (short *)calloc(1, metaSize);

    startSave(&saveList); //  # Initialize Linked list referevce

	burstNframe = (int) ( burstDuration * frameRate + 1 );

	acqFlag = 1;
	frameTriggerCounter = 0;
	frameCounter = 0;

	startStoreTrg = (triggerMode == 0);
    while ( acqFlag )
	{
        getFrame( &frameStatus, frameBuffer, metaData);   //get the frame
        if ( irFrameFormat != radiometric )         
        	frameConv((unsigned short *)frameBuffer, width, height);  //convert kelvin in Celsius


        if ( triggerMode == 1 )        // External trigger source
		{

           	if ( (frameStatus == 4) && (startStoreTrg == 0) )       //start data storing @ 1st trigger seen (trigger is on image header!)
			{
            	startStoreTrg = 1;
            	printf("TRIGGERED:\n");	
			}

           	if (frameTriggerCounter == burstNframe) 
		   	{
				//debug
				triggered = 0;
    
                printf("ACQUIRED ALL TRIGGER FRAMES: %d\n", burstNframe );	
                startStoreTrg   = 0;                          //disable storing
                frameTriggerCounter = 0;
                if ( autoCalibration )    //execute calibration action @ every burst of frames (only if NO auto calibration)
                    executeAutoCalib();

	           	if ( NtriggerCount == numTrigger ) { //stop store when all trigger will be received
	                printf("ACQUIRED ALL FRAME BURST: %d\n", numTrigger );	
	                break;             
				}

                NtriggerCount++; 
			}

        } else { //internal trigger source
        	//Single acquisition, one trigger, will be perfomed in intrnal trigger mode           
        	//frameTime = frameCounter * 1./frameRate;
        	if ( frameTriggerCounter == burstNframe )
			{
            	printf("Stop Internal trigger acquisition %f %f %f\n", frameTime, burstDuration, frameRate);
				break;
			}
		}


		if( (frameStatus != 3 ) && ( storeEnabled == 1 && startStoreTrg == 1 ) 
								&& ( acqSkipFrameNumber <= 0 || (frameTriggerCounter % (acqSkipFrameNumber + 1) ) == 0 ) )
		{

			int frameTimeBaseIdx;
			frameTimeBaseIdx = NtriggerCount * burstNframe + frameTriggerCounter;
			printf("SAVE Frame : %d timebase Idx : %d\n", frameTriggerCounter,  frameTimeBaseIdx);
	//
	// CT la routine camSaveFrame utilizza il frame index in acquisizione. L'indice viene
    // utilizzato per individuare nell'array della base temporale il tempo associato al frame.
	// Anche con trigger interno viene inizializzata la time base e il frame time non e' quindi necessario

            camSaveFrame((void *)frameBuffer, width, height, 0 /*frameTime*/, 14, (void *)treePtr, framesNid, timebaseNid, frameTimeBaseIdx, (void *)metaData, metaSize, framesMetadNid, saveList); 

			//ATTENZIONE VERIFICARE LO STATO

		} 


        if( streamingEnabled )
		{
        	if( tcpStreamHandle == -1) 
			{
            	rstatus = camOpenTcpConnectionNew(streamingServer, streamingPort, &tcpStreamHandle, width, height);
            	if( rstatus !=-1 )
            		printf( "\nConnected to FFMPEG on localhost: %d",streamingPort);
			}


			if ( streamingSkipFrameNumber - 1 <= 0 || (frameCounter % ( streamingSkipFrameNumber - 1)) == 0 )
			{
printf("Streaming data \n");
            	if ( irFrameFormat == radiometric )
                	flirRadiometricConv(frameBuffer, width, height, metaData) ;
    
            	camFrameTo8bit((unsigned short *) frameBuffer, width, height, frame8bit, autoAdjustLimit, 
																				&lowLim, &highLim, minLim, maxLim);
            	camSendFrameOnTcp(&tcpStreamHandle, width, height, frame8bit);
			}             
		}

        frameCounter++;   //never resetted, used for frame timestamp     
        if ( startStoreTrg == 1 ) //CT incremento l'indice dei frame salvato solo se l'acquisizione e' stata triggerata 
            frameTriggerCounter++;     

    }//endwhile

    stopSave(saveList); // Stop asynhronous store stream

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

	acqStopped = 1;

	return rstatus;

}

