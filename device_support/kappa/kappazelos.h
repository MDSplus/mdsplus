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
#include <SDK4/SDK4ControlTypes.h>
#include <SDK4/SDK4ZelosTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

//camera
int kappaOpen(const char *cameraName, void **cameraHandle);
int kappaClose(void *cameraHandle);

//settings
int kappaSetColorCoding(void *cameraHandle, SDK4_ENUM_COLORCODING color_coding);
int kappaSetExposureMode(void *cameraHandle, SDK4_ENUM_EXPOSUREMODE expmode); 
int kappaSetExposure(void *cameraHandle, float expTime); 
int kappaSetAET(void *cameraHandle, SDK4_ENUM_AET aet); 
int kappaSetAutoExposureLevel(void *cameraHandle, uint32_t lev);
int kappaSetGain(void *cameraHandle, uint32_t gain);
int kappaSetAGC(void *cameraHandle, SDK4_ENUM_AGC agc);
int kappaSetSoftTrigger(void *cameraHandle, SDK4_ENUM_SWITCH uOnOff);
int kappaSetTriggerTimer(void *cameraHandle, uint32_t time);
int kappaSetSlowScan(void *cameraHandle, SDK4_ENUM_SWITCH uOnOff);
int kappaGetReadoutAreaLimits(void *cameraHandle, SDK4_ENUM_ROADOMAIN select, int *x, int *y, int *width, int *height);
int kappaSetReadoutArea(void *cameraHandle, int x, int y, int width, int height);
int kappaSetBinning(void *cameraHandle, int binHor, int binVer);
int kappaSetMeasureWindow(void *cameraHandle, int x, int y, int width, int height);

//info
int kappaGetIp(void *cameraHandle, char *ip);
int kappaPrintInfo(void *cameraHandle);

//acquisition
int kappaStartAcquisition(void *cameraHandle, void **buffer, int32_t *width, int32_t *height, uint32_t *payloadSize);
int kappaStopAcquisition(void *cameraHandle, void *buffer);
int kappaGetFrame(void *cameraHandle, int32_t *status, void *frame);

#ifdef __cplusplus
}
#endif

