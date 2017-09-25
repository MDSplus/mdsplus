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
#pragma once

extern int CamBytcnt(unsigned short *iosb);
extern int CamError(int *xexp, int *qexp, unsigned short *iosb_in);
extern int CamX(unsigned short *iosb_in);
extern int CamQ(unsigned short *iosb_in);
extern int CamGetStat(unsigned short *iosb_in);
extern int CamXandQ(unsigned short *iosb_in);
extern int CamFQrepw(char *name, int a, int f, int count, void *data, int mem, unsigned short *iosb);
extern int CamFQstopw(char *name, int a, int f, int count, void *data, int mem, unsigned short *iosb);
extern int CamFStopw(char *name, int a, int f, int count, void *data, int mem, unsigned short *iosb);
extern int CamQrepw(char *name, int a, int f, int count, void *data, int mem, unsigned short *iosb);
extern int CamQscanw(char *name, int a, int f, int count, void *data, int mem, unsigned short *iosb);
extern int CamQstopw(char *name, int a, int f, int count, void *data, int mem, unsigned short *iosb);
extern int CamStopw(char *name, int a, int f, int count, void *data, int mem, unsigned short *iosb);
extern int CamPiow(char *name, int a, int f, void *data, int mem, unsigned short *iosb);
extern int CamPioQrepw(char *name, int a, int f, void *data, int mem, unsigned short *iosb);
extern int CamVerbose(int mode);
