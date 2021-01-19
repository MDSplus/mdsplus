#include <visa.h>
int NiScope_init(char *dev, ViSession *ih);
int NiScope_initiateacquisition(ViSession ih);
int NiScope_acquisitionstatus(ViSession ih);
int NiScope_configuretriggerimmediate(ViSession ih);
int NiScope_ConfigureChanCharacteristics(ViSession ih, char *dev,
                                         ViReal64 *impe);
int NiScope_ConfigureVertical(ViSession ih, char *dev, ViReal64 *range,
                              ViReal64 *offset, ViInt32 coupl, ViReal64 *pa);
int NiScope_ConfigureHorizontalTiming(ViSession ih, ViReal64 *msr, ViInt32 mup);
int NiScope_ConfigureTriggerEdge(ViSession ih, char *ts, ViReal64 *level,
                                 ViInt32 slope, ViInt32 tc, ViReal64 *delay);
int NiScope_Commit(ViSession ih);
int NiScope_SampleRate(ViSession ih, ViReal64 *samplerate);
int NiScope_ActualRecordLength(ViSession ih, ViPInt32 recordlength);
int NiScope_ActualNumWfms(ViSession ih, char *dev, ViPInt32 numwfms);
int NiScope_ActualMeasWfmSize(ViSession ih, ViPInt32 measwaveformsize);
int NiScope_Store(ViSession ih, char *dev, ViInt16 *bindata, ViReal64 *gainch,
                  ViReal64 *offsetch);
int NiScope_close(ViSession ih);
