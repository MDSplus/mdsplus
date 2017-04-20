#ifdef __cplusplus
extern "C" {
#endif

int camOpenTcpConnection(int StreamingPort, int *kSockHandle, int width, int height);  
int camOpenTcpConnectionNew(const char *streamingServer, int StreamingPort, int *kSockHandle, int width, int height);  
int camCloseTcpConnection(int *kSockHandle);
int camSendFrameOnTcp(int *kSockHandle, int width, int height, void *frame8bit);
int camFrameTo8bit(unsigned short *frame, int width, int height, unsigned char *frame8bit, bool adjLimits, unsigned int *lowLim, unsigned int *highLim, unsigned int minLim, unsigned int maxLim);
int camFFMPEGoverlay(const char *filename, const char *textString);

void camStreamingFrame(int tcpStreamHandle, void *frame, void *frameMetadata, int width, int height, int pixelSize, int irFrameFormat, bool adjLimit, unsigned int *lowLim, unsigned int *highLim, unsigned int minLim, unsigned int maxLim, const char *deviceName, void *streamingListPtr);

static void *handleStreaming(void *listPtr);
void camStartStreaming(void **retList);
void camStopStreaming(void *listPtr);

#ifdef __cplusplus
}
#endif

