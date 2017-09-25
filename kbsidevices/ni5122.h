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
#include <visa.h>
int NiScope_init(char *dev, ViSession * ih);
int NiScope_initiateacquisition(ViSession ih);
int NiScope_acquisitionstatus(ViSession ih);
int NiScope_configuretriggerimmediate(ViSession ih);
int NiScope_ConfigureChanCharacteristics(ViSession ih, char *dev, ViReal64 * impe);
int NiScope_ConfigureVertical(ViSession ih, char *dev, ViReal64 * range, ViReal64 * offset,
			      ViInt32 coupl, ViReal64 * pa);
int NiScope_ConfigureHorizontalTiming(ViSession ih, ViReal64 * msr, ViInt32 mup);
int NiScope_ConfigureTriggerEdge(ViSession ih, char *ts, ViReal64 * level, ViInt32 slope,
				 ViInt32 tc, ViReal64 * delay);
int NiScope_Commit(ViSession ih);
int NiScope_SampleRate(ViSession ih, ViReal64 * samplerate);
int NiScope_ActualRecordLength(ViSession ih, ViPInt32 recordlength);
int NiScope_ActualNumWfms(ViSession ih, char *dev, ViPInt32 numwfms);
int NiScope_ActualMeasWfmSize(ViSession ih, ViPInt32 measwaveformsize);
int NiScope_Store(ViSession ih, char *dev, ViInt16 * bindata, ViReal64 * gainch,
		  ViReal64 * offsetch);
int NiScope_close(ViSession ih);
