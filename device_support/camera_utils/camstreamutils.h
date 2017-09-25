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

extern int flirRadiometricConv(void *frame, int width, int height, void *metaData);

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

