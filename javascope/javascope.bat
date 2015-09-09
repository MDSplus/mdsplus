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
SET DOCS=jScope.html jScope.jpg popup.jpg about_jscope.jpg ^
CompositeWaveDisplay.html ^
ConnectionEvent.html ConnectionListener.html ^
DataProvider.html ^
FrameData.html ^
ReadMe.html data_popup.jpg data_setup.jpg frame_popup.jpg image_setup.jpg ^
TWU_image_message.html ^
UpdateEvent.html UpdateEventListener.html ^
WaveData.html WaveDisplay.html

SET TOPDOCS=jScope.html ^
CompositeWaveDisplay.html ^
ConnectionEvent.html ConnectionListener.html ^
DataProvider.html ^
FrameData.html ^
ReadMe.html ^
TWU_image_message.html ^
UpdateEvent.html UpdateEventListener.html ^
WaveData.html WaveDisplay.html

SET SUBDOCS=jScope.html jScope.jpg popup.jpg ^
CompositeWaveDisplay.html ^
ConnectionEvent.html ConnectionListener.html ^
DataProvider.html ^
FrameData.html ^
data_setup.jpg frame_popup.jpg image_setup.jpg ^
UpdateEvent.html UpdateEventListener.html ^
WaveData.html

SET COMMON_SRC=^
jScope\AboutWindow.java jScope\ASCIIDataProvider.java jScope\AsdexDataProvider.java ^
jScope\AsynchDataSource.java ^
jScope\Base64.java jScope\ColorDialog.java jScope\ColorMapDialog.java ^
jScope\ColorMap.java jScope\ConnectionEvent.java jScope\ConnectionListener.java ^
jScope\ContourSignal.java jScope\DataAccess.java jScope\DataAccessURL.java ^
jScope\DataCached.java jScope\DataCacheObject.java jScope\DataProvider.java ^
jScope\DataServerItem.java jScope\Descriptor.java jScope\FakeTWUProperties.java ^
jScope\FontSelection.java jScope\FrameData.java jScope\Frames.java ^
jScope\FtuDataProvider.java jScope\Grid.java jScope\ImageTransferable.java ^
jScope\JetDataProvider.java jScope\JetMdsDataProvider.java jScope\JiDataSource.java ^
jScope\JiDim.java jScope\JiNcSource.java jScope\JiNcVarByte.java ^
jScope\JiNcVarChar.java jScope\JiNcVarDouble.java jScope\JiNcVarFloat.java ^
jScope\JiNcVarImp.java jScope\JiNcVarInt.java jScope\JiNcVar.java ^
jScope\JiNcVarShort.java jScope\JiSlabIterator.java jScope\JiSlab.java ^
jScope\JiVarImpl.java jScope\JiVar.java jScope\jScopeBrowseSignals.java ^
jScope\jScopeBrowseUrl.java jScope\jScopeDefaultValues.java jScope\jScopeFacade.java ^
jScope\jScopeMultiWave.java jScope\jScopeProperties.java jScope\jScopeWaveContainer.java ^
jScope\jScopeWavePopup.java jScope\MdsAccess.java jScope\MdsConnection.java ^
jScope\MdsDataProvider.java jScope\MdsMessage.java jScope\MdsplusParser.java ^
jScope\MdsWaveInterface.java jScope\MultiWaveform.java jScope\MultiWavePopup.java ^
jScope\NotConnectedDataProvider.java jScope\ProfileDialog.java jScope\PropertiesEditor.java ^
jScope\RandomAccessData.java jScope\RowColumnContainer.java jScope\SignalListener.java ^
jScope\RowColumnLayout.java jScope\SetupDataDialog.java jScope\SetupDefaults.java ^
jScope\SetupWaveformParams.java jScope\SignalBox.java ^
jScope\Signal.java jScope\SignalsBoxDialog.java jScope\SshTunneling.java ^
jScope\TSDataProvider.java jScope\TwuAccess.java jScope\TwuDataProvider.java ^
jScope\TWUFetchOptions.java jScope\TwuNameServices.java jScope\TWUProperties.java ^
jScope\TWUSignal.java jScope\TwuSimpleFrameData.java jScope\TwuSingleSignal.java ^
jScope\TwuWaveData.java jScope\UniversalDataProvider.java jScope\UpdateEvent.java ^
jScope\UpdateEventListener.java jScope\WaveContainerEvent.java jScope\WaveContainerListener.java ^
jScope\WaveData.java jScope\WaveDisplay.java jScope\WaveformContainer.java ^
jScope\WaveformEditor.java jScope\WaveformEditorListener.java jScope\WaveformEvent.java ^
jScope\Waveform.java jScope\WaveformListener.java jScope\WaveformManager.java ^
jScope\WaveformMetrics.java jScope\WaveInterface.java jScope\WavePopup.java ^
jScope\XYData.java jScope\XYWaveData.java jScope\WaveDataListener.java

SET WAVEDISPLAY_SRC=

SET JSCOPE_SRC=^
jScope.java ^
jScope\CompositeWaveDisplay.java ^
jScope\LocalDataProvider.java ^
jScope\MdsDataClient.java ^
jScope\MdsIOException.java ^
jScope\MdsPlusBrowseSignals.java ^
jScope\TextorBrowseSignals.java

SET EXTRA_CLASS=^
jScope\FakeTWUProperties.class ^
jScope\FontPanel.class ^
jScope\ServerDialog*.class ^
jScope\WindowDialog.class

SET CLASSPATH=-classpath ".;%MDSPLUS_DIR%\java\classes\MindTerm.jar"
SET JAVAC="%JDK_DIR%\bin\javac.exe"
SET JCFLAGS= ||rem -Xlint -deprecation
SET JAR="%JDK_DIR%\bin\jar.exe"
SET JARDIR=..\java\classes
MKDIR  %JARDIR%\docs 2>NUL

ECHO compiling *.java to *.class . . .
%JAVAC% %JCFLAGS% -d %JARDIR% %CLASSPATH% %COMMON_SRC% %JSCOPE_SRC% %WAVEDISPLAY_SRC% ||rem jScope/DEBUG.java
SET /A ERROR=%ERRORLEVEL%
IF %ERROR% NEQ 0 GOTO:cleanup

:gather
ECHO gathering data
COPY /Y jScope.properties %JARDIR%\>NUL
COPY /Y colors1.tbl %JARDIR%\>NUL
FOR %%F IN (%DOCS%) DO COPY /Y %%F %JARDIR%\docs>NUL

:packjar
ECHO creating jar packages
PUSHD %JARDIR%
%JAR% -cf "jScope.jar" jScope.class colors1.tbl jScope.properties jScope docs
%JAR% -cf "WaveDisplay.jar" %COMMON_SRC:.java=.class%
POPD

:cleanup
ECHO cleaning up
PUSHD %JARDIR%
RMDIR /S /Q docs 2>NUL
DEL colors1.tbl jScope.properties *.class 2>NUL
RMDIR /S /Q jScope 2>nul
POPD

:jscope
IF %ERROR% NEQ 0 GOTO:end
ECHO start jScope?
PAUSE
CLS
java -cp "%JARDIR%\jScope.jar" -Xmx128M  jScope

:end
PAUSE
EXIT /B ERROR