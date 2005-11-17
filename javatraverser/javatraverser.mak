.SUFFIXES: .class .java
.java.class:
	%JDK_DIR%\bin\javac $*.java

SOURCES_DATA=ActionData.java\
ApdData.java\
ArrayData.java\
AtomicData.java\
ByteData.java\
CallData.java\
CompoundData.java\
ConditionData.java\
ConglomData.java\
Data.java\
DependencyData.java\
DimensionData.java\
DispatchData.java\
DoubleData.java\
FloatData.java\
FunctionData.java\
IdentData.java\
IntData.java\
MethodData.java\
NidData.java\
OctaData.java\
ParameterData.java\
PathData.java\
ProcedureData.java\
ProgramData.java\
QuadData.java\
RangeData.java\
RoutineData.java\
ShortData.java\
SignalData.java\
SlopeData.java\
StringData.java\
WindowData.java\
WithErrorData.java\
WithUnitsData.java



SOURCES_ARRAY=ByteArray.java\
DoubleArray.java\
FloatArray.java\
IntArray.java\
OctaArray.java\
QuadArray.java\
ShortArray.java\
StringArray.java

SOURCES_TRAV=ActionEditor.java\
ArgEditor.java\
AxisEditor.java\
ByteArray.java\
CompileTree.java\
Database.java\
DatabaseException.java\
DataChangeEvent.java\
DataChangeListener.java\
DataEditor.java\
DataId.java\
DataListener.java\
DecompileTree.java\
DeviceApply.java\
DeviceApplyBeanInfo.java\
DeviceButtons.java\
DeviceButtonsBeanInfo.java\
DeviceButtonsCustomizer.java\
DeviceCancel.java\
DeviceCancelBeanInfo.java\
DeviceChannel.java\
DeviceChannelBeanInfo.java\
DeviceChannelCustomizer.java\
DeviceChoice.java\
DeviceChoiceBeanInfo.java\
DeviceChoiceCustomizer.java\
DeviceComponent.java\
DeviceControl.java\
DeviceCustomizer.java\
DeviceDispatch.java\
DeviceDispatchBeanInfo.java\
DeviceDispatchField.java\
DeviceField.java\
DeviceFieldBeanInfo.java\
DeviceFieldCustomizer.java\
DeviceOk.java\
DeviceOkBeanInfo.java\
DeviceReset.java\
DeviceResetBeanInfo.java\
DeviceSetup.java\
DeviceSetupBeanInfo.java\
DeviceTable.java\
DeviceTableBeanInfo.java\
DeviceTableCustomizer.java\
DeviceWave.java\
DeviceWaveBeanInfo.java\
DeviceWaveCustomizer.java\
DeviceWaveDisplay.java\
DeviceWaveDisplayBeanInfo.java\
DeviceWaveDisplayCustomizer.java\
DispatchEditor.java\
DisplayData.java\
DisplayNci.java\
DisplayTags.java\
DoubleArray.java\
Editor.java\
EventData.java\
ExprEditor.java\
FloatArray.java\
FloatArrayEditor.java\
FrameRepository.java\
IllegalDataException.java\
IntArray.java\
IntArrayEditor.java\
jTraverser.java\
LabeledExprEditor.java\
LoadPulse.java\
MethodEditor.java\
ModifyData.java\
Node.java\
NodeBeanInfo.java\
NodeDataPropertyEditor.java\
NodeDisplayData.java\
NodeDisplayNci.java\
NodeDisplayTags.java\
NodeEditor.java\
NodeId.java\
NodeInfo.java\
NodeInfoPropertyEditor.java\
NodeModifyData.java\
NodePropertyEditor.java\
OctaArray.java\
ParameterEditor.java\
ProcedureEditor.java\
ProgramEditor.java\
QuadArray.java\
RangeEditor.java\
RemoteTree.java\
RoutineEditor.java\
ShortArray.java\
SyntaxException.java\
TaskEditor.java\
Tree.java\
TreeDialog.java\
TreeNode.java\
TreeServer.java\
UnsupportedDataException.java\
WindowEditor.java


DEVICES = ACQD240Setup.java\
ACQD241Setup.java\
ACQD265Setup.java\
ACQD270Setup.java\
ADC265Setup.java\
Aurora14Setup.java\
BOLOSetup.java\
CADHSetup.java\
CAMERASetup.java\
CHVPSSetup.java\
CHVPS_SETSetup.java\
DEQUSetupSetup.java\
DeviceSetup.java\
DFLUSetupSetup.java\
DIO2EncoderSetup.java\
DIO2Setup.java\
E1463Setup.java\
EDAMSetupSetup.java\
EM_EQU_TESTSetup.java\
EM_FLU_TESTSetup.java\
EM_GAINSetup.java\
FLU_GAINSetup.java\
FR10Setup.java\
IPC901Setup.java\
J221Setup.java\
K3115Setup.java\
L5613Setup.java\
L6810Setup.java\
LASER_NDSetup.java\
LASER_RUSetup.java\
MCUSetup.java\
MHD_BR_TESTSetup.java\
MPBDecoderSetup.java\
MPBEncoderSetup.java\
MPBRecorderSetup.java\
NI6071ESetup.java\
OAM01Setup.java\
PELLETSetup.java\
PXD114Setup.java\
RFXABUnitsSetup.java\
RFXANSALDOSetup.java\
RFXAxiControlSetup.java\
RFXChopperSetupSetup.java\
RFXClockSetup.java\
RFXControlSetup.java\
RFXDClockSetup.java\
RFXFeedforwardSetupSetup.java\
RFXGClockSetup.java\
RFXInverterSetupSetup.java\
RFXMOPSetup.java\
RFXParametersSetup.java\
RFXPConfigSetup.java\
RFXPCSetupSetup.java\
RFXPMSetupSetup.java\
RFXPoloidalSetup.java\
RFXPRConfigSetup.java\
RFXPulseSetup.java\
RFXPVSetupSetup.java\
RFXTFSetupSetup.java\
RFXTimesSetup.java\
RFXTimingSetup.java\
RFXToroidalSetup.java\
RFXVIConfigSetup.java\
RFXVISetupSetup.java\
ST133Setup.java\
SXCSetup.java\
T2ControlSetup.java\
TOMOSetupSetup.java\
TR10Setup.java\
TR32Setup.java\
TR6_3Setup.java\
TRCFSetup.java\
TRCHSetup.java\
VMEWavesSetup.java\
WE7000Setup.java\
WE7116Setup.java\
WE7275Setup.java\
WEGroupSetup.java\
XRAYSetup.java 





GIFS = DeviceApply.gif \
       DeviceChoice.gif \
       DeviceReset.gif \
       compound.gif \
       signal.gif \
       text.gif \
       DeviceButtons.gif \
       DeviceDispatch.gif \
       DeviceSetup.gif \
       device.gif \
       structure.gif \
       window.gif \
       DeviceCancel.gif \
       DeviceField.gif \
       action.gif \
       dispatch.gif \
       subtree.gif \
       DeviceChannel.gif \
       DeviceOk.gif \
       axis.gif \
       numeric.gif \
       task.gif

all : $(SOURCES_DATA) $(SOURCES_ARRAY) $(SOURCES_TRAV) $(DEVICES)
	%JDK_DIR%\bin\javac -classpath .;..\java\classes\jScope.jar $(SOURCES_DATA)
	%JDK_DIR%\bin\javac -classpath .;..\java\classes\jScope.jar $(SOURCES_TRAV)
	%JDK_DIR%\bin\jar -cmf DeviceBeansManifest.mf ..\java\classes\DeviceBeans.jar *.class *.gif
	%JDK_DIR%\bin\javac -classpath .;..\java\classes\jScope.jar $(DEVICES)
	%JDK_DIR%\bin\jar -cf ..\java\classes\jTraverser.jar *.class *.gif

