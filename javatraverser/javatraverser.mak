CLASSPATH = -classpath .;..\java\classes\jScope.jar
JAVAC = "$(JDK_DIR)\bin\javac.exe"
JAR = "$(JDK_DIR)\bin\jar.exe"
JARDIR = ..\java\classes

.SUFFIXES: .class .java
.java.class:
	$(JAVAC) $*.java

SOURCES_DATA=ActionData.java\
ApdData.java\
ArrayData.java\
AtomicData.java\
ByteData.java\
CallData.java\
ComplexData.java\
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
DeviceLabel.java\
DeviceLabelBeanInfo.java\
DeviceLabelCustomizer.java\
DeviceMultiComponent.java\
DeviceOk.java\
DeviceOkBeanInfo.java\
DeviceParameters.java\
DeviceParametersBeanInfo.java\
DeviceReset.java\
DeviceResetBeanInfo.java\
DeviceSetup.java\
DeviceSetupBeanInfo.java\
DeviceTable.java\
DeviceTableBeanInfo.java\
DeviceTableCustomizer.java\
DeviceWave.java\
DeviceWaveBeanInfo.java\
DeviceWaveDisplay.java\
DeviceWaveDisplayBeanInfo.java\
DeviceWaveDisplayCustomizer.java\
DeviceWaveParameters.java\
DeviceWaveParametersBeanInfo.java\
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
OctaArray.java\
ParameterEditor.java\
PythonEditor.java\
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
WindowEditor.java\
Convert.java




GIFS = DeviceApply.gif \
       DeviceChoice.gif \
       DeviceReset.gif \
       compound.gif \
       signal.gif \
       text.gif \
       DeviceButtons.gif \
       DeviceDispatch.gif \
       DeviceSetup.gif \
	   DeviceLabel.gif \
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

all : $(SOURCES_DATA) $(SOURCES_ARRAY) $(SOURCES_TRAV)
	$(JAVAC) $(CLASSPATH) $(SOURCES_DATA)
	$(JAVAC) $(CLASSPATH) $(SOURCES_ARRAY)
	$(JAVAC) $(CLASSPATH) $(SOURCES_TRAV)
	$(JAR) -cmf DeviceBeansManifest.mf ..\java\classes\DeviceBeans.jar *.class *.gif
	$(JAR) -cf ..\java\classes\jTraverser.jar *.class *.gif

