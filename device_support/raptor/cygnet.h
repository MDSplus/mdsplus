#ifdef __cplusplus
extern "C"
{
#endif
  int epixOpen(char *pcConfFile);
  void epixClose();
  void epixStartVideoCapture(int iID);
  void epixStopVideoCapture(int iID);
  int epixCaptureFrame(int iID, int iFramesNid, double dTriggerTime,
                       int iTimeoutMs, void *pTree, void *pList, int *piBufIdx,
                       int *piFrameIdx, int *piBaseTicks, double *pdCurrTime);
  void epixSetConfiguration(int iID, double dFrameRate, char cTrigMode);
  void epixGetConfiguration(int iID, char *pcBinning, short *psRoiXSize,
                            short *psRoiXOffset, short *psRoiYSize,
                            short *psRoiYOffset);
  void epixGetTemp(int iID, float *pfPcbTemp, short *psCmosTemp);
  short epixGetPCBTemp(int iID);
  short epixGetCMOSTemp(int iID);
#ifdef __cplusplus
}
#endif
