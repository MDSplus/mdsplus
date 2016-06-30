#include <vector>
#include <string.h>
#include <stdio.h>
using namespace std;

#include "FLIRSC65X.h"
#include "fff.h"

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
		return ERROR;

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
	return camPtr[camHandle]->printAllParameters();
}


int setExposureMode(int camHandle, EXPMODE_ENUM exposureMode)
{
	return camPtr[camHandle]->setExposureMode(exposureMode);
}


int setFrameRate(int camHandle, FPS_ENUM fps, int *frameToSkip)
{
	return camPtr[camHandle]->setFrameRate(fps, frameToSkip);
}


int setIrFormat(int camHandle, IRFMT_ENUM irFormat)
{
	return camPtr[camHandle]->setIrFormat(irFormat);
}


int getReadoutArea(int camHandle, int *x, int *y, int *width, int *height)
{
	return camPtr[camHandle]->getReadoutArea(x, y, width, height);
}


int setReadoutArea(int camHandle, int x, int y, int width, int height)
{
	return camPtr[camHandle]->setReadoutArea(x, y, width, height);
}


int setObjectParameters(int camHandle, double reflectedTemperature, double atmosphericTemperature, 
											double objectDistance, double objectEmissivity, 
											double relativeHumidity, double extOpticsTemperature, 
											double extOpticsTransmission, double estimatedTransmission)
{
	return camPtr[camHandle]->setObjectParameters(reflectedTemperature, atmosphericTemperature, objectDistance, objectEmissivity, relativeHumidity, extOpticsTemperature, extOpticsTransmission, estimatedTransmission);
}


int setMeasurementRange(int camHandle, int measRange)
{
	return camPtr[camHandle]->setMeasurementRange(measRange);
}


int executeAutoFocus(int camHandle)
{
	return camPtr[camHandle]->executeAutoFocus();
}


int setCalibMode(int camHandle, int calMode)
{
	return camPtr[camHandle]->setCalibMode(calMode);
}


int executeAutoCalib(int camHandle)
{
	return camPtr[camHandle]->executeAutoCalib();
}


int startAcquisition(int camHandle, int *width, int *height, unsigned int *payloadSize)
{
	return camPtr[camHandle]->startAcquisition(width, height, payloadSize);
}


int stopAcquisition(int camHandle)
{
	return camPtr[camHandle]->stopAcquisition();
}


int getFrame(int camHandle, int *status, void *frame, void *metaData)
{
	return camPtr[camHandle]->getFrame(status, frame, metaData);
}


int frameConv(int camHandle, unsigned short *frame, int width, int height, int milliKelvinScale)
{
	return camPtr[camHandle]->frameConv(frame, width, height, milliKelvinScale);
}

#ifdef __cplusplus 
} 
#endif 




FLIR_SC65X::FLIR_SC65X(const char *ipAddress)
{
    this->ipAddress=PvString(ipAddress);  
    this->lDevice = new PvDevice();
	this->lResult = this->lDevice->Connect(this->ipAddress, PvAccessExclusive);
}



FLIR_SC65X::~FLIR_SC65X()
{
	this->lResult = this->lDevice->Disconnect();
    delete(this->lDevice); 
}



int FLIR_SC65X::checkLastOp()
{
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
	if ( !this->lResult.IsOK() ) {printf("Error setting current port to 0!!!\n"); return ERROR;} 

	switch(exposureMode)
	{
	  case internal_mode: 	
		this->lResult = currCfg->SetValue( "GeneralPurpose" );
		if ( !this->lResult.IsOK() ) 
			{printf("Error setting exposure mode!!!\n"); return ERROR;} 
		else
			{printf("Sync. INTERNAL configured.\n");} 
		break;
	  case external_mode: 
		this->lResult = currCfg->SetValue( "MarkImage" );
		if ( !this->lResult.IsOK() ) 
			{printf("Error setting exposure mode!!!\n"); return ERROR;} 
		else
			{printf("Sync. EXTERNAL configured.\n");} 
		break;
	}

	PvString prova;
    currCfg->GetValue( prova );
	printf("Port 0 configuration: ");
	if(prova=="MarkImage")
		printf("MarkImage\n");
	if(prova=="GeneralPurpose")
		printf("GeneralPurpose\n");

	return SUCCESS;
}



int FLIR_SC65X::setFrameRate(FPS_ENUM fps, int *frameToSkip)
{
    PvGenParameterArray *lDeviceParams = lDevice->GetGenParameters();
	PvGenInteger *lWidth = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "Width" ) );
	PvGenInteger *lHeight = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "Height" ) );
	PvGenEnum *sWindowing = dynamic_cast<PvGenEnum *>( lDeviceParams->Get( "IRWindowing" ) );
	PvGenEnum *sFrameRate = dynamic_cast<PvGenEnum *>( lDeviceParams->Get( "IRFrameRate" ) );

//commentato perchè dava errori l'impostazione dell'offset... forse non è gestito bene!
//	PvGenInteger *lOffsetX = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "OffsetX" ) );
//	PvGenInteger *lOffsetY = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "OffsetY" ) );
//	this->lResult = lOffsetX->SetValue( 0 );
//	 if ( !this->lResult.IsOK() ) {printf("Error setting OffsetX in setFrameRate!!!\n"); return ERROR;} 
//	this->lResult = lOffsetY->SetValue( 0 );
//	 if ( !this->lResult.IsOK() ) {printf("Error setting OffsetY in setFrameRate!!!\n"); return ERROR;} 

	switch(fps)
	{
		case fps_200: 
			this->lResult = sFrameRate->SetValue(0);
		 	 if ( !this->lResult.IsOK() ) {printf("Error setting FrameRate in setFrameRate!!!\n"); return ERROR;}

			this->lResult = sWindowing->SetValue("Quarter");//"Quarter"
		 	 if ( !this->lResult.IsOK() ) {printf("Error setting Windowing in setFrameRate!!!\n"); return ERROR;}
	
			this->lResult = lWidth->SetValue( 640 );
			 if ( !this->lResult.IsOK() ) {printf("Error setting Width in setFrameRate!!!\n"); return ERROR;} 

			this->lResult = lHeight->SetValue( 123 );
			 if ( !this->lResult.IsOK() ) {printf("Error setting Height in setFrameRate!!!\n"); return ERROR;} 
#ifdef debug
			printf("Frame rate set to 200Hz.\n");
#endif
			*frameToSkip = 8;
			break;

		case fps_100: 
			this->lResult = sFrameRate->SetValue(0);
		 	 if ( !this->lResult.IsOK() ) {printf("Error setting FrameRate in setFrameRate!!!\n"); return ERROR;}

			this->lResult = sWindowing->SetValue("Half"); //"Half"
		 	 if ( !this->lResult.IsOK() ) {printf("Error setting Windowing in setFrameRate!!!\n"); return ERROR;}

			this->lResult = lWidth->SetValue( 640 );
			 if ( !this->lResult.IsOK() ) {printf("Error setting Width in setFrameRate!!!\n"); return ERROR;} 

			this->lResult = lHeight->SetValue( 243 );
			 if ( !this->lResult.IsOK() ) {printf("Error setting Height in setFrameRate!!!\n"); return ERROR;}
#ifdef debug
			printf("Frame rate set to 100Hz.\n");
#endif
			*frameToSkip = 4;
			break;

		case fps_50:
			this->lResult = sFrameRate->SetValue(0);
		 	 if ( !this->lResult.IsOK() ) {printf("Error setting FrameRate in setFrameRate!!!\n"); return ERROR;}

			this->lResult = sWindowing->SetValue("Off"); //"Off"
		 	 if ( !this->lResult.IsOK() ) {printf("Error setting Windowing in setFrameRate!!!\n"); return ERROR;}

			this->lResult = lWidth->SetValue( 640 );
			 if ( !this->lResult.IsOK() ) {printf("Error setting Width in setFrameRate!!!\n"); return ERROR;} 

			this->lResult = lHeight->SetValue( 483 );
			 if ( !this->lResult.IsOK() ) {printf("Error setting Height in setFrameRate!!!\n"); return ERROR;}
#ifdef debug
			printf("Frame rate set to 50Hz.\n");
#endif
			*frameToSkip = 2;
			break;

		case fps_25:
			this->lResult = sFrameRate->SetValue(1);
		 	 if ( !this->lResult.IsOK() ) {printf("Error setting FrameRate in setFrameRate!!!\n"); return ERROR;}

			this->lResult = sWindowing->SetValue("Off"); //"Off"
		 	 if ( !this->lResult.IsOK() ) {printf("Error setting Windowing in setFrameRate!!!\n"); return ERROR;}

			this->lResult = lWidth->SetValue( 640 );
			 if ( !this->lResult.IsOK() ) {printf("Error setting Width in setFrameRate!!!\n"); return ERROR;} 

			this->lResult = lHeight->SetValue( 483 );
			 if ( !this->lResult.IsOK() ) {printf("Error setting Height in setFrameRate!!!\n"); return ERROR;}
#ifdef debug
			printf("Frame rate set to 25Hz.\n");
#endif
			*frameToSkip = 1;
			break;

		case fps_12: 
			this->lResult = sFrameRate->SetValue(2);
		 	 if ( !this->lResult.IsOK() ) {printf("Error setting FrameRate in setFrameRate!!!\n"); return ERROR;}

			this->lResult = lWidth->SetValue( 640 );
			 if ( !this->lResult.IsOK() ) {printf("Error setting Width in setFrameRate!!!\n"); return ERROR;} 

			this->lResult = lHeight->SetValue( 483 );
			 if ( !this->lResult.IsOK() ) {printf("Error setting Height in setFrameRate!!!\n"); return ERROR;}

			this->lResult = sWindowing->SetValue("Off"); //"Off"
		 	 if ( !this->lResult.IsOK() ) {printf("Error setting Windowing in setFrameRate!!!\n"); return ERROR;}
#ifdef debug
			printf("Frame rate set to 12.5Hz.\n");
#endif
			*frameToSkip = 1;
			break;

		case fps_6:
			this->lResult = sFrameRate->SetValue(3);
		 	 if ( !this->lResult.IsOK() ) {printf("Error setting FrameRate in setFrameRate!!!\n"); return ERROR;}

			this->lResult = lWidth->SetValue( 640 );
			 if ( !this->lResult.IsOK() ) {printf("Error setting Width in setFrameRate!!!\n"); return ERROR;} 

			this->lResult = lHeight->SetValue( 483 );
			 if ( !this->lResult.IsOK() ) {printf("Error setting Height in setFrameRate!!!\n"); return ERROR;}

			this->lResult = sWindowing->SetValue("Off"); //"Off"
		 	 if ( !this->lResult.IsOK() ) {printf("Error setting Windowing in setFrameRate!!!\n"); return ERROR;}
#ifdef debug
			printf("Frame rate set to 6.25Hz.\n");
#endif
			*frameToSkip = 1;
			break;

		case fps_3:
			this->lResult = sFrameRate->SetValue(4);
		 	 if ( !this->lResult.IsOK() ) {printf("Error setting FrameRate in setFrameRate!!!\n"); return ERROR;}

			this->lResult = lWidth->SetValue( 640 );
			 if ( !this->lResult.IsOK() ) {printf("Error setting Width in setFrameRate!!!\n"); return ERROR;} 

			this->lResult = lHeight->SetValue( 483 );
			 if ( !this->lResult.IsOK() ) {printf("Error setting Height in setFrameRate!!!\n"); return ERROR;}

			this->lResult = sWindowing->SetValue("Off"); //"Off"
		 	 if ( !this->lResult.IsOK() ) {printf("Error setting Windowing in setFrameRate!!!\n"); return ERROR;}
#ifdef debug
			printf("Frame rate set to 3.12Hz.\n");
#endif
			*frameToSkip = 1;
			break;
	}

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
				{printf("Error setting Radiometric in setIrFormat!!!\n"); return ERROR;} 
			else
				{printf("IR FORMAT set to radiometric.\n");} 
			break;
		case linear100mK:
			this->lResult = sFormat->SetValue("TemperatureLinear100mK"); 
			if ( !this->lResult.IsOK() ) 
				{printf("Error setting TemperatureLinear100mK!!!\n"); return ERROR;} 
			else
				{printf("IR FORMAT set to TemperatureLinear100mK.\n");} 
			break;
		case linear10mK:
			this->lResult = sFormat->SetValue("TemperatureLinear10mK"); 
			if ( !this->lResult.IsOK() ) 
				{printf("Error setting TemperatureLinear10mK!!!\n"); return ERROR;} 
			else
				{printf("IR FORMAT set to TemperatureLinear10mK.\n");} 
			break;
	}

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
	if ( !this->lResult.IsOK() ) {printf("Error getting Width in getReadoutArea\n"); return ERROR;}

	this->lResult = lHeight->GetValue( h );
	if ( !this->lResult.IsOK() ) {printf("Error getting Height in getReadoutArea\n"); return ERROR;}

	this->lResult = lOffsetX->GetValue( ox );
	if ( !this->lResult.IsOK() ) {printf("Error getting OffsetX in getReadoutArea\n"); return ERROR;}

	this->lResult = lOffsetY->GetValue( oy );
	if ( !this->lResult.IsOK() ) {printf("Error getting OffsetY in getReadoutArea\n"); return ERROR;}

	*x=(int)ox;
	*y=(int)oy;  
	*width=(int)w;
	*height=(int)h-3;	//remove 3 lines of metadata  

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
	if ( !this->lResult.IsOK() ) {printf("Error setting Width in setReadoutArea\n"); return ERROR;}

	this->lResult = lHeight->SetValue(h+3);  //add 3 lines of metadata
	if ( !this->lResult.IsOK() ) {printf("Error setting Height in setReadoutArea\n"); return ERROR;}

	this->lResult = lOffsetX->SetValue(ox);
	if ( !this->lResult.IsOK() ) {printf("Error setting OffsetX in setReadoutArea\n"); return ERROR;}

	this->lResult = lOffsetY->SetValue(oy);
	if ( !this->lResult.IsOK() ) {printf("Error setting OffsetY in setReadoutArea\n"); return ERROR;}

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
	if ( !this->lResult.IsOK() ) {printf("Error setting reflectedTemperature!!!\n"); return ERROR;}

	this->lResult = AtmTemp->SetValue(atmosphericTemperature + 273.15);
	if ( !this->lResult.IsOK() ) {printf("Error setting atmosphericTemperature!!!\n"); return ERROR;}

	this->lResult = ObjDist->SetValue(objectDistance);
	if ( !this->lResult.IsOK() ) {printf("Error setting objectDistance!!!\n"); return ERROR;}

	this->lResult = ObjEmis->SetValue(objectEmissivity);
	if ( !this->lResult.IsOK() ) {printf("Error setting objectEmissivity!!!\n"); return ERROR;}

	this->lResult = RelHumi->SetValue(relativeHumidity);
	if ( !this->lResult.IsOK() ) {printf("Error setting relativeHumidity!!!\n"); return ERROR;}

	this->lResult = EopTemp->SetValue(extOpticsTemperature + 273.15);
	if ( !this->lResult.IsOK() ) {printf("Error setting extOpticsTemperature!!!\n"); return ERROR;}

	this->lResult = EopTran->SetValue(extOpticsTransmission);
	if ( !this->lResult.IsOK() ) {printf("Error setting extOpticsTransmission!!!\n"); return ERROR;}

	this->lResult = EstTran->SetValue(estimatedTransmission);
	if ( !this->lResult.IsOK() ) {printf("Error setting estimatedTransmission!!!\n"); return ERROR;}

    return SUCCESS;
}



int FLIR_SC65X::setMeasurementRange(int measRange)
{
	if(measRange<0 or measRange>2)
	{
	  printf("Error: measRange in setMeasurementRange must be 0,1,2!!!\n");
	  return ERROR;
	}

    PvGenParameterArray *lDeviceParams = lDevice->GetGenParameters();
	PvGenInteger *currCase = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "CurrentCase" ) );
	PvGenInteger *queryCase = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "QueryCase" ) );
	PvGenFloat *lowLim = dynamic_cast<PvGenFloat *>( lDeviceParams->Get( "QueryCaseLowLimit" ) );
	PvGenFloat *highLim = dynamic_cast<PvGenFloat *>( lDeviceParams->Get( "QueryCaseHighLimit" ) );

	PvInt64 val = measRange;

    this->lResult = queryCase->SetValue(val);
	 if ( !this->lResult.IsOK() ) {printf("Error: case not defined in setMeasurementRange!!!\n"); return ERROR;} 

    this->lResult = currCase->SetValue(val);
	 if ( !this->lResult.IsOK() ) {printf("Error in setMeasurementRange!!!\n"); return ERROR;} 

	double limitLow, limitHigh = 0;

    this->lResult = lowLim->GetValue(limitLow);
    this->lResult = highLim->GetValue(limitHigh);

#ifdef debug
	printf("Measurement range set to %.0f...%.0f\n",(limitLow-273.15) ,(limitHigh-273.15) );
#endif

    return SUCCESS;
}



int FLIR_SC65X::executeAutoFocus()
{
    PvGenParameterArray *lDeviceParams = lDevice->GetGenParameters();
    PvGenCommand *autoFocus = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "AutoFocus" ) );
  
    this->lResult = autoFocus->Execute();
	 if ( !this->lResult.IsOK() ) {printf("Error executing auto focus!!!\n"); return ERROR;} 	

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
	  {printf("Error setting calibration mode!!!\n"); return ERROR;} 

	PvString lValue;
	PvGenParameter *lGenParameter = lDeviceParams->Get( "NUCMode" );			
	static_cast<PvGenEnum *>( lGenParameter )->GetValue( lValue );
	printf( "Calibration Mode set to: %s\n", lValue.GetAscii() );

    return SUCCESS;
}



int FLIR_SC65X::executeAutoCalib()
{
    PvGenParameterArray *lDeviceParams = lDevice->GetGenParameters();
    PvGenCommand *autoCalib = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "NUCAction" ) );
  
    this->lResult = autoCalib->Execute();
	 if ( !this->lResult.IsOK() ) {printf("Error executing calibration!!!\n"); return ERROR;} 	

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

	this->lResult = lWidth->GetValue( w );
	if ( !this->lResult.IsOK() ) {printf("Error getting frame width in startAcquisition!!!\n"); return ERROR;}
	this->lResult = lHeight->GetValue( h );
	if ( !this->lResult.IsOK() ) {printf("Error getting frame height in startAcquisition!!!\n"); return ERROR;}
	this->lResult = lPayloadSize->GetValue( lSize ); 
	if ( !this->lResult.IsOK() ) {printf("Error getting payload size in startAcquisition!!!\n"); return ERROR;} 

	*width=(int)w;
	*height=(int)h-3;  					//first 3 rows are metadata
	*payloadSize=(unsigned int)lSize;  //payload = width*height*2 + metadata

    // Negotiate streaming packet size
    this->lResult = lDevice->NegotiatePacketSize();
	if ( !this->lResult.IsOK() ) {printf("Error negotiating packet size in start acquisition!!!\n"); return ERROR;} 

    // Open stream
    this->lResult = this->lStream.Open( this->ipAddress );
	if ( !this->lResult.IsOK() ) {printf("Error opening stream in start acquisition!!!\n"); return ERROR;} 

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
	if ( !this->lResult.IsOK() ) {printf("Error setting stream dest. in start acquisition!!!\n"); return ERROR;} 

    // Queue all buffers in the stream
    for ( PvUInt32 i = 0; i < lBufferCount; i++ )
    {
        lStream.QueueBuffer( lBuffers + i );
    }

    // TLParamsLocked is optional but when present, it MUST be set to 1
    // before sending the AcquisitionStart command
    if ( lTLLocked != NULL )
    {
        this->lResult = lTLLocked->SetValue( 1 );
		if ( !this->lResult.IsOK() ) {printf("Error setting TLParamsLocked to 1!!!\n"); return ERROR;} 
    }
  
	//reset timestamp counter
    this->lResult = lResetTimestamp->Execute();
	if ( !this->lResult.IsOK() ) {printf("Error resetting timestamp counter!!!\n"); return ERROR;} 

    // Tell the device to start sending images
    this->lResult = lStart->Execute();
	if ( !this->lResult.IsOK() ) {printf("Error sending StartAcquisition command to device!!!\n"); return ERROR;} 	

	return SUCCESS;	
}
  


int FLIR_SC65X::stopAcquisition()
{
    PvGenParameterArray *lDeviceParams = lDevice->GetGenParameters();
    PvGenInteger *lTLLocked = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "TLParamsLocked" ) );
    PvGenCommand *lStop = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "AcquisitionStop" ) );

    // Tell the device to stop sending images
    this->lResult = lStop->Execute();
	if ( !this->lResult.IsOK() ) {printf("Error sending AcquisitionStop command to the device!!!\n"); return ERROR;} 

    // If present reset TLParamsLocked to 0. Must be done AFTER the 
    // streaming has been stopped
    if ( lTLLocked != NULL )
    {
        this->lResult = lTLLocked->SetValue( 0 );
		if ( !this->lResult.IsOK() ) {printf("Error resetting TLParamsLocked to 0!!!\n"); return ERROR;} 
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
    int lDoodleIndex = 0;
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

		//	PvUInt8* dataPtr = lImage->GetDataPointer();  
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
					//printf("external trigger!\n");
					*status=4; //complete + triggered!	
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
int FLIR_SC65X::frameConv(unsigned short *frame, int width, int height, int milliKelvinScale)
{
    short sub=0;
    switch(milliKelvinScale)
    {   
	  case 100:     sub=2731;  //1 = 100mK = 100mC -> precisione decimo di grado
                    break;
	  case 10:      sub=27315; //1 = 10mK = 10mC -> precisione centesimo di grado
                    break;                       
    }
	for(int i=0; i<width*height; i++) 
	{
       frame[i] = frame[i] - sub;
	}
}




