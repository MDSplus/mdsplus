#include <stdio.h>
#include <Mrm/MrmPublic.h>
#include <Xm/Xm.h>
#include <Xmds/XmdsWaveform.h>
void XmdsInitialize()
{  
  xmdsWaveformWidgetClass = (WidgetClass)&xmdsWaveformClassRec;  
  MrmRegisterClass(MrmwcUnknown,"XmdsWaveformWidgetClass","XmdsCreateWaveform",XmdsCreateWaveform,xmdsWaveformWidgetClass);
}
void XmdsResetAllXds(){}
void XmdsXdsAreValid(){}
void XmdsApplyAllXds(){}
