#ifdef __cplusplus
extern "C" {
#endif

extern int flirRadiometricConv(void *frame, int width, int height, void *metaData);

int camOpenTcpConnection(int StreamingPort, int *kSockHandle, int width, int height);  
int camOpenTcpConnectionNew(const char *streamingServer, int StreamingPort, int *kSockHandle, int width, int height);  
int camCloseTcpConnection(int *kSockHandle);
int camSendFrameOnTcp(int *kSockHandle, int width, int height, void *frame8bit);
int camFrameTo8bit(unsigned short *frame, int width, int height, unsigned char *frame8bit, bool adjLimits, unsigned int *lowLim, unsigned int *highLim, unsigned int minLim, unsigned int maxLim);

void camStreamingFrame(int tcpStreamHandle, void *frame, void *frameMetadata, int width, int height, int pixelSize, int irFrameFormat, bool adjLimit, unsigned int minLim, unsigned int maxLim, void *streamingListPtr);

void *handleStreaming(void *listPtr);
void startStreaming(void **retList);
void stopStreaming(void *listPtr);

#ifdef __cplusplus
}
#endif

