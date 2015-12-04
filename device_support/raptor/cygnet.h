#ifdef __cplusplus
extern "C" {
#endif
int epixOpen(char *confFile, int *xPixels, int *yPixels);
void epixClose();
void epixStartVideoCapture(int id);
void epixStopVideoCapture(int id);
int epixCaptureFrame(int iID, int iDataNid, double dTriggerTime, int iTimeOutMs, void *pTree, void *pList, int *piBufIdx, int *piFrameIdx, int *piBaseTicks, double *pdCurrTime);
void epixSetConfiguration(int id, float frameRate, char trigMode);
void epixGetConfiguration(int id, int *binning, int *roiXSize, int *roiXOffset, int *roiYSize, int *roiYOffset);
void epixGetTemp(int id, float *pcbTemp, short *cmosTemp);
short epixGetPCBTemp(int id);
short epixGetCMOSTemp(int id);
#ifdef __cplusplus
}
#endif
