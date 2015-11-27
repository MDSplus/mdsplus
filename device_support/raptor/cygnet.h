#ifdef __cplusplus
extern "C" {
#endif
int epixOpen(char *confFile, int *xPixels, int *yPixels);
void epixClose();
void epixStartVideoCapture(int id);
void epixStopVideoCapture(int id);
int epixCaptureFrame(int id, int frameIdx, int bufIdx, int baseTicks, int xPixels, int yPixels, int dataNid, int timeNid, void *treePtr, void *listPtr, int timeoutMs,  int *retFrameIdx, int *retBufIdx, int *retBaseTicks, float *retDuration);
void epixSetConfiguration(int id, float frameRate, char trigMode);
void epixGetConfiguration(int id, int *binning, int *roiXSize, int *roiXOffset, int *roiYSize, int *roiYOffset);
void epixGetTemp(int id, float *pcbTemp, short *cmosTemp);
int doTransaction(int id, char *outBufIn, int outBytes, char *readBuf, int readBytes);
short getPCBTemp(int id);
short getCMOSTemp(int id);
#ifdef __cplusplus
}
#endif
