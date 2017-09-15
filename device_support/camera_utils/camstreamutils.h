#ifdef __cplusplus
extern "C" {
#endif


//SUPPORTED PIXEL FORMATS derived from 'ffmpeg/libavutil/pixfmt.h'
//DEFINES MUST BE ALSO IN ffmpeg/libavdevices/tcpframes.c
#define CSU_PIX_FMT_NONE 0
#define CSU_PIX_FMT_GRAY8 1         ///<        Y        ,  8bpp
#define CSU_PIX_FMT_GRAY16 2        ///<        Y        ,  16bpp   (should have big-endian and little-endian formats)
#define CSU_PIX_FMT_BAYER_RGGB8 3   ///< bayer, RGRG..(odd line), GBGB..(even line), 8-bit samples */
#define CSU_PIX_FMT_YUV422_Packed 4 ///< planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)


int camOpenTcpConnection(const char *streamingServer, int StreamingPort, int *kSockHandle, int width, int height, int pixelFormat);  
int camCloseTcpConnection(int *kSockHandle);
int camSendFrameOnTcp(int *kSockHandle, int width, int height, void *frame8bit);
int camFrameTo8bit(void *frame, int width, int height, int pixelFormat, unsigned char *frame8bit, bool adjLimits, unsigned int *lowLim, unsigned int *highLim, unsigned int minLim, unsigned int maxLim);
int camFFMPEGoverlay(const char *filename, const char *textString);

void camStreamingFrame(int tcpStreamHandle, void *frame, int width, int height, int pixelFormat, int irFrameFormat, bool adjLimit, unsigned int *lowLim, unsigned int *highLim, unsigned int minLim, unsigned int maxLim, const char *deviceName, void *streamingListPtr);

static void *handleStreaming(void *listPtr);
void camStartStreaming(void **retList);
void camStopStreaming(void *listPtr);

#ifdef __cplusplus
}
#endif

