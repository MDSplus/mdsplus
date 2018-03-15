@ECHO OFF
<!-- : --- get date Script ---------------------------
FOR /F "delims=" %%x IN ('cscript //nologo "%~f0?.wsf"') DO %%x
GOTO:rest
-->
<job id="Elevate"><script language="VBScript">
Wscript.Echo("set Year=" & DatePart("yyyy", Date))
Wscript.Echo("set Month=0" & DatePart("m", Date))
Wscript.Echo("set Day=0" & DatePart("d", Date))
</script></job>
:rest

ECHO preparing
if defined JDK_HOME GOTO:start
rem This script located the current version of
rem "Java Development Kit" and sets the
rem %JDK_HOME% environment variable
setlocal ENABLEEXTENSIONS
set KEY=HKEY_LOCAL_MACHINE\SOFTWARE\JavaSoft\Java Development Kit
FOR /F "usebackq tokens=2,* skip=2" %%L IN (`reg query "%KEY%" /v CurrentVersion`) DO SET JDKVER=%%M
FOR /F "usebackq tokens=2,* skip=2" %%L IN (`reg query "%KEY%\%JDKVER%" /v JavaHome`) DO SET JDK_HOME="%%M"
SET JDK_HOME=%JDK_HOME:"=%
IF EXIST "%JDK_HOME%" GOTO:start
ECHO JDK not found. Please set %%JDK_HOME%% to the root path of your jdk.
SET /A ERROR=1
GOTO:end

:start
SET JARDIR=%CD%\..\java\classes
SET SRCDIR=%CD%\java
SET JSCH=%SRCDIR%\..\jsch-0.1.54.jar
SET CLASSPATH=-classpath ".;%JSCH%"
SET JAVAC="%JDK_HOME%\bin\javac.exe" ||rem -Xlint -deprecation
SET JCFLAGS= -O -source 1.6 -target 1.6 -g:none||rem -Xlint -deprecation
SET JAR="%JDK_HOME%\bin\jar.exe"
SET JTMANIFEST=%JARDIR%\JTMANIFEST.mf

SET TRAV_SRC=^
jtraverser\DataChangeEvent.java ^
jtraverser\DataChangeListener.java ^
jtraverser\MdsView.java ^
jtraverser\Node.java ^
jtraverser\TreeView.java ^
jtraverser\TreeManager.java ^
jtraverser\dialogs\ActionList.java ^
jtraverser\dialogs\AddNodeDialog.java ^
jtraverser\dialogs\CheckBoxList.java ^
jtraverser\dialogs\DataDialog.java ^
jtraverser\dialogs\Dialogs.java ^
jtraverser\dialogs\FlagsDialog.java ^
jtraverser\dialogs\GraphPanel.java ^
jtraverser\dialogs\OpenTreeDialog.java ^
jtraverser\dialogs\SubTreeList.java ^
jtraverser\dialogs\TagsDialog.java ^
jtraverser\editor\ActionEditor.java ^
jtraverser\editor\ArgEditor.java ^
jtraverser\editor\ArrayEditor.java ^
jtraverser\editor\Editor.java ^
jtraverser\editor\ExprEditor.java ^
jtraverser\editor\MethodEditor.java ^
jtraverser\editor\ParameterEditor.java ^
jtraverser\editor\ProcedureEditor.java ^
jtraverser\editor\ProgramEditor.java ^
jtraverser\editor\PythonEditor.java ^
jtraverser\editor\RangeEditor.java ^
jtraverser\editor\RoutineEditor.java ^
jtraverser\editor\SegmentEditor.java ^
jtraverser\editor\SignalEditor.java ^
jtraverser\editor\WindowEditor.java ^
jtraverser\editor\usage\ActionEditor.java ^
jtraverser\editor\usage\AnyEditor.java ^
jtraverser\editor\usage\AxisEditor.java ^
jtraverser\editor\usage\DispatchEditor.java ^
jtraverser\editor\usage\NumericEditor.java ^
jtraverser\editor\usage\SignalEditor.java ^
jtraverser\editor\usage\TaskEditor.java ^
jtraverser\editor\usage\TextEditor.java ^
jtraverser\editor\usage\WindowEditor.java ^
jtraverser\jTraverserFacade.java ^
jTraverser.java

SET TOOLS_SRC=^
jtraverser\tools\CompileTree.java ^
jtraverser\tools\DecompileTree.java

SET MDS_SRC=^
mds\Mds.java ^
mds\MdsEvent.java ^
mds\MdsListener.java ^
mds\MdsException.java ^
mds\MdsShr.java ^
mds\Shr.java ^
mds\TCL.java ^
mds\TdiShr.java ^
mds\TreeShr.java ^
mds\UpdateEvent.java ^
mds\UpdateEventListener.java ^
mds\data\CTX.java ^
mds\data\DATA.java ^
mds\data\DTYPE.java ^
mds\data\OPC.java ^
mds\data\TREE.java ^
mds\data\descriptor\ARRAY.java ^
mds\data\descriptor\Descriptor.java ^
mds\data\descriptor\Descriptor_A.java ^
mds\data\descriptor\Descriptor_APD.java ^
mds\data\descriptor\Descriptor_CA.java ^
mds\data\descriptor\Descriptor_D.java ^
mds\data\descriptor\Descriptor_R.java ^
mds\data\descriptor\Descriptor_S.java ^
mds\data\descriptor\Descriptor_XD.java ^
mds\data\descriptor\Descriptor_XS.java ^
mds\data\descriptor_apd\Dictionary.java ^
mds\data\descriptor_apd\List.java ^
mds\data\descriptor_a\COMPLEXArray.java ^
mds\data\descriptor_a\CStringArray.java ^
mds\data\descriptor_a\Complex32Array.java ^
mds\data\descriptor_a\Complex64Array.java ^
mds\data\descriptor_a\EmptyArray.java ^
mds\data\descriptor_a\FLOATArray.java ^
mds\data\descriptor_a\Float32Array.java ^
mds\data\descriptor_a\Float64Array.java ^
mds\data\descriptor_a\Int128Array.java ^
mds\data\descriptor_a\Int16Array.java ^
mds\data\descriptor_a\Int32Array.java ^
mds\data\descriptor_a\Int64Array.java ^
mds\data\descriptor_a\Int8Array.java ^
mds\data\descriptor_a\INTEGER_UNSIGNEDArray.java ^
mds\data\descriptor_a\INTEGERArray.java ^
mds\data\descriptor_a\NUMBERArray.java ^
mds\data\descriptor_a\NidArray.java ^
mds\data\descriptor_a\Uint128Array.java ^
mds\data\descriptor_a\Uint16Array.java ^
mds\data\descriptor_a\Uint32Array.java ^
mds\data\descriptor_a\Uint64Array.java ^
mds\data\descriptor_a\Uint8Array.java ^
mds\data\descriptor_r\Action.java ^
mds\data\descriptor_r\Call.java ^
mds\data\descriptor_r\Condition.java ^
mds\data\descriptor_r\Conglom.java ^
mds\data\descriptor_r\Dependency.java ^
mds\data\descriptor_r\Dim.java ^
mds\data\descriptor_r\Dispatch.java ^
mds\data\descriptor_r\Function.java ^
mds\data\descriptor_r\Method.java ^
mds\data\descriptor_r\Opaque.java ^
mds\data\descriptor_r\PARAMETER.java ^
mds\data\descriptor_r\Param.java ^
mds\data\descriptor_r\Procedure.java ^
mds\data\descriptor_r\Program.java ^
mds\data\descriptor_r\Range.java ^
mds\data\descriptor_r\Routine.java ^
mds\data\descriptor_r\Signal.java ^
mds\data\descriptor_r\Slope.java ^
mds\data\descriptor_r\Window.java ^
mds\data\descriptor_r\With_Error.java ^
mds\data\descriptor_r\With_Units.java ^
mds\data\descriptor_r\function\BINARY.java ^
mds\data\descriptor_r\function\BUILD.java ^
mds\data\descriptor_r\function\CAST.java ^
mds\data\descriptor_r\function\COMPRESSION.java ^
mds\data\descriptor_r\function\CONST.java ^
mds\data\descriptor_r\function\Fun.java ^
mds\data\descriptor_r\function\MODIFIER.java ^
mds\data\descriptor_r\function\UNARY.java ^
mds\data\descriptor_s\COMPLEX.java ^
mds\data\descriptor_s\CString.java ^
mds\data\descriptor_s\Complex32.java ^
mds\data\descriptor_s\Complex64.java ^
mds\data\descriptor_s\Event.java ^
mds\data\descriptor_s\FLOAT.java ^
mds\data\descriptor_s\Float32.java ^
mds\data\descriptor_s\Float64.java ^
mds\data\descriptor_s\Ident.java ^
mds\data\descriptor_s\Int128.java ^
mds\data\descriptor_s\Int16.java ^
mds\data\descriptor_s\Int32.java ^
mds\data\descriptor_s\Int64.java ^
mds\data\descriptor_s\Int8.java ^
mds\data\descriptor_s\INTEGER_UNSIGNED.java ^
mds\data\descriptor_s\INTEGER.java ^
mds\data\descriptor_s\Missing.java ^
mds\data\descriptor_s\NUMBER.java ^
mds\data\descriptor_s\Nid.java ^
mds\data\descriptor_s\NODE.java ^
mds\data\descriptor_s\Path.java ^
mds\data\descriptor_s\Pointer.java ^
mds\data\descriptor_s\Uint128.java ^
mds\data\descriptor_s\Uint16.java ^
mds\data\descriptor_s\Uint32.java ^
mds\data\descriptor_s\Uint64.java ^
mds\data\descriptor_s\Uint8.java ^
mds\mdsip\MdsIp.java ^
mds\mdsip\Message.java ^
mds\mdslib\MdsLib.java

SET TRAV_GIF=^
jtraverser\action.gif ^
jtraverser\any.gif ^
jtraverser\axis.gif ^
jtraverser\compound.gif ^
jtraverser\device.gif ^
jtraverser\dispatch.gif ^
jtraverser\numeric.gif ^
jtraverser\signal.gif ^
jtraverser\structure.gif ^
jtraverser\subtree.gif ^
jtraverser\task.gif ^
jtraverser\text.gif ^
jtraverser\window.gif

MKDIR %JARDIR% 2>NUL
SET TRAV_CLS=%TRAV_SRC:.java=*.class%
SET TOOLS_CLS=%TOOLS_SRC:.java=*.class%
SET MDS_CLS=%MDS_SRC:.java=*.class%

ECHO compiling *.java to *.class . . .
PUSHD %SRCDIR%
%JAVAC% %JCFLAGS% -d %JARDIR% %CLASSPATH% %MDS_SRC% %TRAV_SRC% %TOOLS_SRC% debug\DEBUG.java
SET /A ERROR=%ERRORLEVEL%
POPD
IF %ERROR% NEQ 0 GOTO:cleanup

ECHO gathering data
MKDIR %JARDIR%\jTraverser 2>NUL
COPY /Y %SRCDIR%\jtraverser\*.gif  %JARDIR%\jtraverser>NUL
COPY /Y %CD%\JTMANIFEST.mf %JTMANIFEST%>NUL
ECHO Built-Date: %Year%-%Month:~-2%-%Day:~-2% %TIME:~0,8%>>%JTMANIFEST%

ECHO creating jar packages
PUSHD %JARDIR%
COPY %JSCH% %SRCDIR%\jTraverser.jar>NUL
%JAR% -umf %JTMANIFEST% %SRCDIR%\jTraverser.jar %TRAV_CLS% %TRAV_GIF% %MDS_CLS% %TOOLS_CLS% lib
POPD

:cleanup
ECHO cleaning up
PUSHD %JARDIR%
RMDIR /Q/S .
POPD

:jtraveser
IF %ERROR% NEQ 0 GOTO:end
ECHO start jTraverser?
PAUSE
CLS
java -jar "%SRCDIR%\jTraverser.jar"

:end
PAUSE
EXIT /B ERROR
