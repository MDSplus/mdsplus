.SUFFIXES: .class .java
.java.class:
	%JDK_DIR%\bin\javac.exe $*.java

SOURCES = ActionData.java\
ActionEditor.java\
ApdData.java\
ArgEditor.java\
ArrayData.java\
AtomicData.java\
AxisEditor.java\
ByteArray.java\
ByteData.java\
CallData.java\
CompoundData.java\
ConditionData.java\
ConglomData.java\
Data.java\
DataChangeEvent.java\
DataChangeListener.java\
DataEditor.java\
DataId.java\
DataListener.java\
Database.java\
DatabaseException.java\
DependencyData.java\
DeviceComponent.java\
DeviceSetup.java\
DeviceSetupBeanInfo.java\
DimensionData.java\
DispatchData.java\
DispatchEditor.java\
DisplayData.java\
DisplayNci.java\
DoubleArray.java\
DoubleData.java\
Editor.java\
EventData.java\
ExprEditor.java\
FloatArray.java\
FloatData.java\
FrameRepository.java\
FunctionData.java\
IllegalDataException.java\
IntArray.java\
IntData.java\
LabeledExprEditor.java\
MethodData.java\
MethodEditor.java\
ModifyData.java\
NidData.java\
Node.java\
NodeBeanInfo.java\
NodeDataPropertyEditor.java\
NodeDisplayData.java\
NodeDisplayNci.java\
NodeEditor.java\
NodeId.java\
NodeInfo.java\
NodeInfoPropertyEditor.java\
NodeModifyData.java\
NodePropertyEditor.java\
OctaArray.java\
OctaData.java\
ParameterData.java\
PathData.java\
ProcedureData.java\
ProcedureEditor.java\
ProgramData.java\
ProgramEditor.java\
QuadArray.java\
QuadData.java\
RangeData.java\
RangeEditor.java\
RoutineData.java\
RoutineEditor.java\
ShortArray.java\
ShortData.java\
SignalData.java\
StringData.java\
SyntaxException.java\
TaskEditor.java\
Tree.java\
TreeDialog.java\
TreeNode.java\
UnsupportedDataException.java\
WindowData.java\
WindowEditor.java\
WithErrorData.java\
WithUnitsData.java\
jTraverser.java

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

all : $(SOURCES)
	%JDK_DIR%\bin\javac.exe $(SOURCES)
	%JDK_DIR%\bin\jar.exe -uf ..\java\classes\MDSplus.jar *.class *.gif

#jTraverser.class : *.java
#	%JDK_DIR%\bin\javac.exe jTraverser.java

#TreeNode.class : *.java
#	%JDK_DIR%\bin\javac.exe TreeNode.java