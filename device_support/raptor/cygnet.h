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
#ifdef __cplusplus
extern "C" {
#endif
int   epixOpen(char *pcConfFile);
void  epixClose();
void  epixStartVideoCapture(int iID);
void  epixStopVideoCapture(int iID);
int   epixCaptureFrame(int iID, int iFramesNid, double dTriggerTime, int iTimeoutMs, void *pTree, void *pList, int *piBufIdx, int *piFrameIdx, int *piBaseTicks, double *pdCurrTime);
void  epixSetConfiguration(int iID, double dFrameRate, char cTrigMode);
void  epixGetConfiguration(int iID, char *pcBinning, short *psRoiXSize, short *psRoiXOffset, short *psRoiYSize, short *psRoiYOffset);
void  epixGetTemp(int iID, float *pfPcbTemp, short *psCmosTemp);
short epixGetPCBTemp(int iID);
short epixGetCMOSTemp(int iID);
#ifdef __cplusplus
}
#endif
