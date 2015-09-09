@ECHO OFF
ECHO preparing
if defined JDK_DIR GOTO:start
rem This script located the current version of
rem "Java Development Kit" and sets the
rem %JDK_PATH% environment variable
setlocal ENABLEEXTENSIONS
set KEY=HKEY_LOCAL_MACHINE\SOFTWARE\JavaSoft\Java Development Kit
FOR /F "usebackq tokens=2,* skip=2" %%L IN (`reg query "%KEY%" /v CurrentVersion`) DO SET JDKVER=%%M
FOR /F "usebackq tokens=2,* skip=2" %%L IN (`reg query "%KEY%\%JDKVER%" /v JavaHome`) DO SET JDK_DIR="%%M"
SET JDK_DIR=%JDK_DIR:"=%
IF EXIST "%JDK_DIR%" GOTO:start
ECHO JDK not found. Please set %%JDK_DIR%% to the root path of your jdk.
SET /A ERROR=1
GOTO:end

:start
SET SOURCES_DATA=ActionData.java ^
ApdData.java ^
ArrayData.java ^
AtomicData.java ^
ByteData.java ^
CallData.java ^
ComplexData.java ^
CompoundData.java ^
ConditionData.java ^
ConglomData.java ^
Data.java ^
DependencyData.java ^
DimensionData.java ^
DispatchData.java ^
DoubleData.java ^
FloatData.java ^
FunctionData.java ^
IdentData.java ^
IntData.java ^
MethodData.java ^
NidData.java ^
OctaData.java ^
ParameterData.java ^
PathData.java ^
ProcedureData.java ^
ProgramData.java ^
QuadData.java ^
RangeData.java ^
RoutineData.java ^
ShortData.java ^
SignalData.java ^
SlopeData.java ^
StringData.java ^
WindowData.java ^
WithErrorData.java ^
WithUnitsData.java

SET SOURCES_ARRAY=ByteArray.java ^
DoubleArray.java ^
FloatArray.java ^
IntArray.java ^
OctaArray.java ^
QuadArray.java ^
ShortArray.java ^
StringArray.java

SET SOURCES_TRAV=ActionEditor.java ^
ArgEditor.java ^
AxisEditor.java ^
ByteArray.java ^
CompileTree.java ^
Database.java ^
DatabaseException.java ^
DataChangeEvent.java ^
DataChangeListener.java ^
DataEditor.java ^
DataId.java ^
DataListener.java ^
DecompileTree.java ^
DeviceApply.java ^
DeviceApplyBeanInfo.java ^
DeviceButtons.java ^
DeviceButtonsBeanInfo.java ^
DeviceButtonsCustomizer.java ^
DeviceCancel.java ^
DeviceCancelBeanInfo.java ^
DeviceChannel.java ^
DeviceChannelBeanInfo.java ^
DeviceChannelCustomizer.java ^
DeviceChoice.java ^
DeviceChoiceBeanInfo.java ^
DeviceChoiceCustomizer.java ^
DeviceComponent.java ^
DeviceControl.java ^
DeviceCustomizer.java ^
DeviceDispatch.java ^
DeviceDispatchBeanInfo.java ^
DeviceDispatchField.java ^
DeviceField.java ^
DeviceFieldBeanInfo.java ^
DeviceFieldCustomizer.java ^
DeviceOk.java ^
DeviceLabel.java ^
DeviceLabelBeanInfo.java ^
DeviceLabelCustomizer.java ^
DeviceMultiComponent.java ^
DeviceOk.java ^
DeviceOkBeanInfo.java ^
DeviceParameters.java ^
DeviceParametersBeanInfo.java ^
DeviceReset.java ^
DeviceResetBeanInfo.java ^
DeviceSetup.java ^
DeviceSetupBeanInfo.java ^
DeviceTable.java ^
DeviceTableBeanInfo.java ^
DeviceTableCustomizer.java ^
DeviceWave.java ^
DeviceWaveBeanInfo.java ^
DeviceWaveDisplay.java ^
DeviceWaveDisplayBeanInfo.java ^
DeviceWaveDisplayCustomizer.java ^
DeviceWaveParameters.java ^
DeviceWaveParametersBeanInfo.java ^
DispatchEditor.java ^
DisplayData.java ^
DisplayNci.java ^
DisplayTags.java ^
DoubleArray.java ^
Editor.java ^
EventData.java ^
ExprEditor.java ^
FloatArray.java ^
FloatArrayEditor.java ^
FrameRepository.java ^
IllegalDataException.java ^
IntArray.java ^
IntArrayEditor.java ^
jTraverser.java ^
LabeledExprEditor.java ^
LoadPulse.java ^
MethodEditor.java ^
ModifyData.java ^
Node.java ^
NodeBeanInfo.java ^
NodeDataPropertyEditor.java ^
NodeDisplayData.java ^
NodeDisplayNci.java ^
NodeDisplayTags.java ^
NodeEditor.java ^
NodeId.java ^
NodeInfo.java ^
NodeInfoPropertyEditor.java ^
NodeModifyData.java ^
OctaArray.java ^
ParameterEditor.java ^
PythonEditor.java ^
ProcedureEditor.java ^
ProgramEditor.java ^
QuadArray.java ^
RangeEditor.java ^
RemoteTree.java ^
RoutineEditor.java ^
ShortArray.java ^
SyntaxException.java ^
TaskEditor.java ^
Tree.java ^
TreeDialog.java ^
TreeNode.java ^
TreeServer.java ^
UnsupportedDataException.java ^
WindowEditor.java ^
Convert.java

SET GIFS=DeviceApply.gif ^
DeviceChoice.gif ^
DeviceReset.gif ^
compound.gif ^
signal.gif ^
text.gif ^
DeviceButtons.gif ^
DeviceDispatch.gif ^
DeviceSetup.gif ^
DeviceLabel.gif ^
device.gif ^
structure.gif ^
window.gif ^
DeviceCancel.gif ^
DeviceField.gif ^
action.gif ^
dispatch.gif ^
subtree.gif ^
DeviceChannel.gif ^
DeviceOk.gif ^
axis.gif ^
numeric.gif ^
task.gif

SET CLASSPATH=-classpath ".;%MDSPLUS_DIR%\java\classes\jScope.jar"
SET JAVAC="%JDK_DIR%\bin\javac.exe" ||rem -Xlint -deprecation
SET JCFLAGS= ||rem -Xlint -deprecation
SET JAR="%JDK_DIR%\bin\jar.exe"
SET MANIFEST=%CD%\DeviceBeansManifest.mf
SET JARDIR=..\java\classes
MKDIR %JARDIR% 2>NUL

ECHO compiling *.java to *.class . . .
%JAVAC% %JCFLAGS% -d %JARDIR% %CLASSPATH% %SOURCES_DATA% %SOURCES_ARRAY% %SOURCES_TRAV%
SET /A ERROR=%ERRORLEVEL%
IF %ERROR% NEQ 0 GOTO:cleanup

ECHO gathering data
COPY /Y *.gif %JARDIR% >NUL

ECHO creating jar packages
PUSHD %JARDIR%
%JAR% -cmf %MANIFEST% DeviceBeans.jar *.class *.gif
%JAR% -cf jTraverser.jar *.class *.gif
POPD

:cleanup
ECHO cleaning up
PUSHD %JARDIR%
DEL /Q *.gif 2>NUL
DEL /Q *.class 2>NUL
POPD

:jtraveser
IF %ERROR% NEQ 0 GOTO:end
ECHO start jTraverser?
PAUSE
CLS
java -cp "%JARDIR%\jTraverser.jar" jTraverser

:end
PAUSE
EXIT /B ERROR