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

int camOpenTree(char *treeName, int shot, void **treePtr);
int camCloseTree(void *treePtr);
//int camSaveFrame(void *frame, int width, int height, float frameTime, int pixelSize, void *treePtr, int dataNid, int timebaseNid, int frameIdx, void *frameMetadata, int metaSize, int metaNid);
void camSaveFrame(void *frame, int width, int height, float frameTime, int pixelSize, void *treePtr, int dataNid, int timebaseNid, int frameIdx, void *frameMetadata, int metaSize, int metaNid, void *saveListPtr);
void camSaveFrameDirect(void *frame, int width, int height, float frameTime, int pixelSize, void *treePtr, int dataNid, int timebaseNid, 
        int frameIdx, void *saveListPtr);

static void *handleSave(void *listPtr);
void camStartSaveDeferred(void **retList);
void camStartSave(void **retList);
void camStopSave(void *listPtr);

#ifdef __cplusplus
}
#endif

