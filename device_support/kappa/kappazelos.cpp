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
#include <iostream>
#include <stdlib.h>   //for malloc and free
using namespace std;

#include <SDK4/SDK4AcquireApi.h>
#include <SDK4/SDK4ControlTypes.h>
#include <SDK4/SDK4ZelosApi.h>

#include "kappazelos.h"

#define _CHECK(f) {SDK4_ERROR err=(f);if(SDK4_ERR_SUCCESS!=err){cerr<< #f <<" failed: "<<err<<endl;goto Exit;}}
#define error -1
#define success 0


//debug mode if defined
#define debug



const char* ExposureModeToString(SDK4_ENUM_EXPOSUREMODE eExposureMode)
{
	switch(eExposureMode)
	{
	case ZELOS_ENUM_EXPOSUREMODE_FREERUNNINGPARALLEL:
		return "FreeRunningParallel";
	case ZELOS_ENUM_EXPOSUREMODE_FREERUNNINGSEQUENTIAL:
		return "FreeRunningSequential";
	case ZELOS_ENUM_EXPOSUREMODE_RESETRESTART:
		return "ResetRestart";
	case ZELOS_ENUM_EXPOSUREMODE_FRAMEONDEMAND:
		return "FrameOnDemand";
	case ZELOS_ENUM_EXPOSUREMODE_EXTERNALSYNC:
		return "ExternalSync";
	default:
		return "Unknown";
	}
}


#define swapShort(data) (( (data) >> 8 ) & 0x00ff) | (( (data) << 8 ) & 0xff00)

#define swapInt(data) ( (( (data) >> 24 ) & 0x000000ff) | (( (data) >> 8 ) & 0x0000ff00) | \
		      (( (data) << 8 ) & 0x00ff0000)  | (( (data) << 24) & 0xff000000) )

int swapPixel( short *buf, int32_t size)
{
	int i;
	for(  i = 0; i < size; i++ )
		buf[i] = swapShort(buf[i]);

	return 0;	
}




int kappaOpen(const char *cameraName, void **cameraHandle)
{
	vector<string> devnames;
    int32_t numDevices=0;
	DEV_HANDLE hDev;
	CTRL_HANDLE hCtrl;
	uint32_t size=0;


    _CHECK(SDK4InitLib());
	_CHECK(SDK4GetNumDevices(&numDevices));  //TO NOT DELETE ELSE COME UP ERROR -1006

//	devnames.push_back("KappaIPstatico"); //open camera by its name
	devnames.push_back(cameraName);

#ifdef debug
	cout << "Selected Camera: " << devnames[0].c_str() << endl;
#endif
	_CHECK(SDK4OpenDevice(devnames[0].c_str(),DEVICEACCESS_EXCLUSIVE,&hDev));
	*cameraHandle=hDev;

	_CHECK(SDK4DevGetControl(hDev, &hCtrl));
	//_CHECK(SDK4SetPacketSize (hCtrl, 8192));
 	SDK4GetPacketSize (hCtrl, &size);
	cout << "Packet size : " << size << endl;
   return success;
Exit:
    return error;
}




int kappaClose(void *cameraHandle)
{
    _CHECK(SDK4CloseDevice(cameraHandle));
    _CHECK(SDK4CloseLib());

    return success;
Exit:
    return error;
}



int kappaSetColorCoding(void *cameraHandle, SDK4_ENUM_COLORCODING color_coding)
{
	CTRL_HANDLE hCtrl;
	_CHECK(SDK4DevGetControl(cameraHandle, &hCtrl));
	_CHECK(SDK4SetColorCoding(hCtrl, color_coding));

#ifdef debug
	switch(color_coding)
	{
	case SDK4_ENUM_COLORCODING_Y8:
		cout << "Color Coding set to Y8" << endl;
		break;
	case SDK4_ENUM_COLORCODING_Y12:
		cout << "Color Coding set to Y12" << endl;
		break;
	case SDK4_ENUM_COLORCODING_Y14:
		cout << "Color Coding set to Y14" << endl;
		break;
	default:
		cout << "Color Coding set to UNKNOWN." << endl;
		break;
	}
#endif

    return success;
Exit:
    return error;
}




int kappaSetExposureMode(void *cameraHandle, SDK4_ENUM_EXPOSUREMODE expmode)
{
	CTRL_HANDLE hCtrl;
	_CHECK(SDK4DevGetControl(cameraHandle, &hCtrl));
	_CHECK(SDK4SetExposureMode(hCtrl, expmode));	
#ifdef debug
	cout << "Exposure mode set to: " << ExposureModeToString(expmode) << endl;
#endif
    return success;
Exit:
    return error;
}




int kappaSetExposure(void *cameraHandle, float expTime)
{
	CTRL_HANDLE hCtrl;
	SDK4_KEXPOSURE kExposure;
	kExposure.base = SDK4_ENUM_EXPOSUREBASE_1us;
	kExposure.counter = (int)(expTime/1E-6);
	if(kExposure.counter > 256)
	{
	    kExposure.base = SDK4_ENUM_EXPOSUREBASE_10us;
	    kExposure.counter = (int)(expTime/1E-5);
	}
	if(kExposure.counter > 256)
	{
	    kExposure.base = SDK4_ENUM_EXPOSUREBASE_100us;
	    kExposure.counter = (int)(expTime/1E-4);
	}
	if(kExposure.counter > 256)
	{
	    kExposure.base = SDK4_ENUM_EXPOSUREBASE_1ms;
	    kExposure.counter = (int)(expTime/1E-3);
	}
	if(kExposure.counter > 256)
	{
	    kExposure.base = SDK4_ENUM_EXPOSUREBASE_10ms;
	    kExposure.counter = (int)(expTime/1E-2);
	}
	if(kExposure.counter > 256)
	{
	    kExposure.base = SDK4_ENUM_EXPOSUREBASE_100ms;
	    kExposure.counter = (int)(expTime/1E-1);
	}

	_CHECK(SDK4DevGetControl(cameraHandle, &hCtrl));
	_CHECK(SDK4SetExposure(hCtrl, kExposure));	

#ifdef debug
	double espo;
	espo = 0;
	switch(kExposure.base)
	{
		case 0: espo = 0; break;
		case 1: espo = 0.000001; break;
		case 2: espo = 0.00001; break;
		case 3: espo = 0.0001; break;
		case 4: espo = 0.001; break;
		case 5: espo = 0.01; break;
		case 6: espo = 0.1; break;
		case 7: espo = 1; break;
	}
        espo=espo*kExposure.counter;
	cout << "Exposure set to " << espo << " s" << endl;
#endif
    return success;
Exit:
    return error;
}




int kappaSetAET(void *cameraHandle, SDK4_ENUM_AET aet)
{
	CTRL_HANDLE hCtrl;
	_CHECK(SDK4DevGetControl(cameraHandle, &hCtrl));
	_CHECK(SDK4SetAET(hCtrl, aet));	
#ifdef debug
	switch(aet)
	{
		case 0: cout << "AET OFF" << endl; break;
		case 1: cout << "AET ON" << endl; break;
		case 2: cout << "AET PUSH" << endl; break;
		case 3: cout << "AET ON EXT" << endl; break;
	}	
#endif
    return success;
Exit:
    return error;
}




int kappaSetAutoExposureLevel(void *cameraHandle, uint32_t lev)
{
	CTRL_HANDLE hCtrl;
	_CHECK(SDK4DevGetControl(cameraHandle, &hCtrl));
	if(lev>=0 && lev<=255)
        {
	 _CHECK(SDK4SetAutoExposureLevel(hCtrl, lev));	
#ifdef debug
	cout << "Auto Exposure level set to: " << lev << endl;
#endif	
	}
	else
        {
#ifdef debug
	cout << "Auto Exposure level must be in range 0-255." << endl;
	return error;
#endif	 
	}

    return success;
Exit:
    return error;
}




int kappaSetGain(void *cameraHandle, uint32_t gain)
{
	CTRL_HANDLE hCtrl;
	_CHECK(SDK4DevGetControl(cameraHandle, &hCtrl));
	
	if(gain>=0 && gain<=511)
        {
	 	_CHECK(SDK4SetGain(hCtrl, gain));
#ifdef debug
	cout << "Gain set to: " << gain << endl;
#endif	
	}
	else
        {
#ifdef debug
	cout << "Gain must be in range 0-511." << endl;
	return error;
#endif	 
	}
    return success;
Exit:
    return error;
}




int kappaSetAGC(void *cameraHandle, SDK4_ENUM_AGC agc)
{
	CTRL_HANDLE hCtrl;
	_CHECK(SDK4DevGetControl(cameraHandle, &hCtrl));
	_CHECK(SDK4SetAGC(hCtrl, agc));	
#ifdef debug
	switch(agc)
	{
		case 0: cout << "AGC OFF" << endl; break;
		case 1: cout << "AGC ON" << endl; break;
		case 2: cout << "AGC PUSH" << endl; break;
	}	
#endif
    return success;
Exit:
    return error;
}




int kappaSetSoftTrigger(void *cameraHandle, SDK4_ENUM_SWITCH uOnOff)
{
	CTRL_HANDLE hCtrl;
	_CHECK(SDK4DevGetControl(cameraHandle, &hCtrl));
	_CHECK(SDK4SetSoftTrigger(hCtrl, uOnOff));	
#ifdef debug
	switch(uOnOff)
	{
		case 0: cout << "Soft Trigger Switch OFF" << endl; break;
		case 1: cout << "Soft Trigger Switch ON" << endl; break;
	}	
#endif
    return success;
Exit:
    return error;
}




int kappaSetTriggerTimer(void *cameraHandle, uint32_t time) //time in ms. accuracy +/- 10ms.
{
	CTRL_HANDLE hCtrl;
	_CHECK(SDK4DevGetControl(cameraHandle, &hCtrl));
	_CHECK(SDK4SetTriggerTimer(hCtrl, time));	
#ifdef debug
	cout << "Trigger timer set to " << time << " ms" << endl; 
#endif
    return success;
Exit:
    return error;
}




int kappaSetSlowScan(void *cameraHandle, SDK4_ENUM_SWITCH uOnOff)
{
	CTRL_HANDLE hCtrl;
	_CHECK(SDK4DevGetControl(cameraHandle, &hCtrl));
	_CHECK(SDK4SetSlowScan(hCtrl, uOnOff));	
#ifdef debug
	switch(uOnOff)
	{
		case 0: cout << "Slow Scan OFF" << endl; break;
		case 1: cout << "Slow Scan ON" << endl; break;
	}	
#endif
    return success;
Exit:
    return error;
}




int kappaGetReadoutAreaLimits(void *cameraHandle, SDK4_ENUM_ROADOMAIN select, int *x, int *y, int *width, int *height)
{
	CTRL_HANDLE hCtrl;
	SDK4_KAREA roa;

	_CHECK(SDK4DevGetControl(cameraHandle, &hCtrl));
	_CHECK(SDK4GetReadoutAreaLimits(hCtrl, select, &roa));	

	*x = roa.start.xPos;
	*y = roa.start.yPos;
	*width = roa.end.xPos - *x;
	*height = roa.end.yPos - *y;

#ifdef debug
	switch(select)
	{
		case 0: cout << "Readout Area MIN" << endl; break;
		case 1: cout << "Readout Area MAX" << endl; break;
		case 2: cout << "Readout Area ACTIVE" << endl; break;
	}
	 cout << "Readout Area Limits (start x position): " << roa.start.xPos << endl;
	 cout << "Readout Area Limits (start y position): " << roa.start.yPos << endl;
	 cout << "Readout Area Limits (end x position): " << roa.end.xPos << endl;
	 cout << "Readout Area Limits (end y position): " << roa.end.yPos << endl;		
#endif
    return success;
Exit:
    return error;
}


int kappaSetReadoutArea(void *cameraHandle, int x, int y, int width, int height)
{
	CTRL_HANDLE hCtrl;
	SDK4_KAREA roa;
	roa.start.xPos = x;
	roa.start.yPos = y;
	roa.end.xPos = x+width;
	roa.end.yPos = y+height;

	_CHECK(SDK4DevGetControl(cameraHandle, &hCtrl));
	_CHECK(SDK4SetReadoutArea(hCtrl, roa));	
#ifdef debug
	 cout << "Readout Area set to (start x position): " << roa.start.xPos << endl;
	 cout << "Readout Area set to (start y position): " << roa.start.yPos << endl;
	 cout << "Readout Area set to (end x position): " << roa.end.xPos << endl;
	 cout << "Readout Area set to (end y position): " << roa.end.yPos << endl;	
#endif
    return success;
Exit:
    return error;
}




int kappaSetBinning(void *cameraHandle, int binHor, int binVer)
{
	SDK4_KBINNING bin;
	CTRL_HANDLE hCtrl;
   	bin.horizontal=binHor;
	if(bin.horizontal > 8)
	    bin.horizontal = 8;
	if(bin.horizontal < 1)
	    bin.horizontal = 1;
	
   	bin.vertical=binVer;
	if(bin.vertical > 8)
	    bin.vertical = 8;
	if(bin.vertical < 1)
	    bin.vertical = 1;
	_CHECK(SDK4DevGetControl(cameraHandle, &hCtrl));
	_CHECK(SDK4SetBinning(hCtrl, bin));

#ifdef debug
	cout << "Horizontal binning set to ";
	switch(bin.horizontal)
	{
		case 1: cout << "1x" << endl; break;
		case 2: cout << "2x" << endl; break;
		case 3: cout << "3x" << endl; break;
		case 4: cout << "4x" << endl; break;
		case 5: cout << "5x" << endl; break;
		case 6: cout << "6x" << endl; break;
		case 7: cout << "7x" << endl; break;
		case 8: cout << "8x" << endl; break;
	}
	cout << "Vertical binning set to ";
	switch(bin.vertical)
	{
		case 1: cout << "1x" << endl; break;
		case 2: cout << "2x" << endl; break;
		case 3: cout << "3x" << endl; break;
		case 4: cout << "4x" << endl; break;
		case 5: cout << "5x" << endl; break;
		case 6: cout << "6x" << endl; break;
		case 7: cout << "7x" << endl; break;
		case 8: cout << "8x" << endl; break;
	}
#endif
    return success;
Exit:
    return error;
}




int kappaSetMeasureWindow(void *cameraHandle, int x, int y, int width, int height)
{
	SDK4_ENUM_MEASUREWINDOWSELECT select = 0;
	SDK4_KAREA mwin;
	CTRL_HANDLE hCtrl;
   	mwin.start.xPos=x;
   	mwin.start.yPos=y;
   	mwin.end.xPos=x+width;
   	mwin.end.yPos=y+height;
	_CHECK(SDK4DevGetControl(cameraHandle, &hCtrl));
	_CHECK(SDK4SetMeasureWindow(hCtrl, select, mwin));
#ifdef debug
	switch(select)
	{
		case 0: cout << "Measure Window 1" << endl; break;
		case 1: cout << "Measure Window 2" << endl; break;
	}
	 cout << "Measure Window set to (start x position): " << mwin.start.xPos << endl;
	 cout << "Measure Window set to (start y position): " << mwin.start.yPos << endl;
	 cout << "Measure Window set to (end x position): " << mwin.end.xPos << endl;
	 cout << "Measure Window set to (end y position): " << mwin.end.yPos << endl;	
#endif
    return success;
Exit:
    return error;
}




int kappaGetIp(void *cameraHandle, char *ip)
{
	int32_t size = 24;
	CTRL_HANDLE hCtrl;

        //Camera Setup
	_CHECK(SDK4DevGetControl(cameraHandle, &hCtrl));	
	_CHECK(SDK4GetCurrentIPAddress(hCtrl, ip, &size));
#ifdef debug
	cout << "Camera IP Address: " << ip << endl;
#endif
    return success;
Exit:
    return error;
}




int kappaPrintInfo(void *cameraHandle)
{
	static char sModel[256];
	int32_t sizeModel=sizeof(sModel);
	
	char sClassId[256];
	int32_t sizeClassId=sizeof(sClassId);
	
    uint32_t uPayloadSize=0;
		
	//image params
    int32_t width=0;
    int32_t height=0;
    ENUM_PIXELFORMAT format=0;

	DS_HANDLE hDataStream;

	//Device
	_CHECK(SDK4DevGetClassID(cameraHandle,sClassId,&sizeClassId));
	_CHECK(SDK4DevGetModel(cameraHandle,sModel,&sizeModel));
	_CHECK(SDK4DevGetDataStream(cameraHandle,&hDataStream));
  
	//DataStream
	_CHECK(SDK4DSGetWidth(hDataStream,&width));
	_CHECK(SDK4DSGetHeight(hDataStream,&height));
	_CHECK(SDK4DSGetPixelFormat(hDataStream,&format));
	_CHECK(SDK4DSGetPayloadSize(hDataStream,&uPayloadSize));

	cout << endl;
	cout << "ClassID     = " << sClassId << endl;
	cout << "Model       = " << sModel << endl;
	cout << endl;	
	cout << "Width       = " << dec << width  << endl;
	cout << "Height      = " << dec << height << endl;
	cout << "Format      = " << hex << format << endl ;
	cout << "PayloadSize = " << dec << uPayloadSize << endl<< endl;

    return success;
Exit:
    return error;
}




int kappaStartAcquisition(void *cameraHandle, void **buffer, int32_t *width, int32_t *height, uint32_t *payloadSize)
{
	DS_HANDLE hDataStream;

	_CHECK(SDK4DevGetDataStream(cameraHandle, &hDataStream));
	_CHECK(SDK4DSGetWidth(hDataStream, width));
	_CHECK(SDK4DSGetHeight(hDataStream, height));
	_CHECK(SDK4DSGetPayloadSize(hDataStream, payloadSize));
	
	BUFFER_HANDLE *tmpBuffer;

	tmpBuffer = (BUFFER_HANDLE *)malloc(sizeof(BUFFER_HANDLE) *5);
	*buffer = (void *)tmpBuffer;
	for(uint32_t i=0;i<5;i++)
	{
		_CHECK(SDK4DSAllocAndAnnounceBuffer(hDataStream, *payloadSize,NULL, &tmpBuffer[i]));
		_CHECK(SDK4DSQueueBuffer(hDataStream, tmpBuffer[i]));
	}		
	_CHECK(SDK4DevStartAcquisition(cameraHandle));

    return success;
Exit:
    return error;
}




int kappaStopAcquisition(void *cameraHandle, void *buffer)
{

	_CHECK(SDK4DevStopAcquisition(cameraHandle));

    DS_HANDLE hDataStream;	
	_CHECK(SDK4DevGetDataStream(cameraHandle,&hDataStream));
		
	_CHECK(SDK4DSFlushQueue(hDataStream));
	for(int i=0;i<5;i++)
	{
		_CHECK(SDK4DSRevokeBuffer(hDataStream, ((BUFFER_HANDLE *)buffer)[i],NULL,NULL));
	}
    free(buffer);

    return success;
Exit:
    return error;
}



int kappaGetFrame(void *cameraHandle, int32_t *status, void *frame)
{
	BUFFER_HANDLE hTempBuffer;
	int32_t bComplete;
	void* pBuffer;
	uint32_t size;
    uint32_t uPayloadSize=0;

     DS_HANDLE hDataStream;
	_CHECK(SDK4DevGetDataStream(cameraHandle,&hDataStream));
	_CHECK(SDK4DSGetPayloadSize(hDataStream, &uPayloadSize));

	SDK4_ERROR err;
	err=SDK4DSWaitForBuffer(hDataStream,&hTempBuffer,900);
		switch(err)
		{
		case SDK4_ERR_SUCCESS:
			_CHECK(SDK4BufferGetPtr(hTempBuffer,&pBuffer));
			_CHECK(SDK4BufferGetSize(hTempBuffer,&size));
			_CHECK(SDK4BufferIsComplete(hTempBuffer,&bComplete));
			if(bComplete)
			{
				*status=1; //complete
				memcpy(frame, pBuffer, uPayloadSize);	
			}
			else
			{
				*status=2; //incomplete
				memset(frame, 0, uPayloadSize);
			}

			//Queue Buffer again
			_CHECK(SDK4DSQueueBuffer(hDataStream,hTempBuffer));
			break;
		case SDK4_ERR_TIMEOUT:
			*status=3; //timeout
			memset(frame, 0, uPayloadSize);
			break;
		default:
			*status=0; //never!
			break;
		}

    return success;
Exit:
    return error;
}

