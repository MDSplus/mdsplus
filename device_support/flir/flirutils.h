/*
Radiometric Conversion functions for FLIR Camera
*/

#ifdef __cplusplus 
extern "C" 
{ 
#endif 
int flirRadiometricConv(void *frame, int width, int height, void *metaData);
int flirRadiometricConvPar(void *frame, int width, int height, void *metaData, float atmTemp, float emissivity, float objectDistance, float ambTemp, float relHum);
void flirRadiometricConvFrames(void *frame, int width, int height, int numFrames, void *metaData);
void flirRadiometricConvFramesPar(void *frame, int width, int height, int numFrames, void *metaData, float *atmTemp, float *emissivity, float *objectDistance, float *ambTemp, float *relHum);
#ifdef __cplusplus 
} 
#endif 
