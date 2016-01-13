@ECHO OFF
ECHO preparing
if defined JDK_DIR GOTO:start
rem This script located the current version of
rem "Java Development Kit" and sets the
rem %JDK_PATH% environment variable
setlocal ENABLEEXTENSIONS
set KEY=HKEY_LOCAL_MACHINE\SOFTWARE\JavaSoft\Java Development Kit
FOR /F "usebackq tokens=2,* skip=2" %%L IN (`reg query "%KEY%" /v CurrentVersion`) DO SET JDKVER=%%M
FOR /F "usebackq tokens=2,* skip=2" %%L IN (`reg query "%KEY% ^%JDKVER%" /v JavaHome`) DO SET JDK_DIR="%%M"
SET JDK_DIR=%JDK_DIR:"=%
IF EXIST "%JDK_DIR%" GOTO:start
ECHO JDK not found. Please set %%JDK_DIR%% to the root path of your jdk.
SET /A ERROR=1
GOTO:end

:start
SET DEVICES=ACQD240Setup.java ^
ACQD241Setup.java ^
ACQD265Setup.java ^
ACQD270Setup.java ^
ADC265Setup.java ^
Aurora14Setup.java ^
BOLOSetup.java ^
CADHSetup.java ^
CAMERASetup.java ^
CHVPSSetup.java ^
CHVPS_SETSetup.java ^
DEQUSetupSetup.java ^
DFLUSetupSetup.java ^
DIO2EncoderSetup.java ^
DIO2_ENCDECSetup.java ^
DIO2Setup.java ^
DIO4Setup.java ^
E1463Setup.java ^
EDAMSetupSetup.java ^
EM_EQU_TESTSetup.java ^
EM_FLU_TESTSetup.java ^
EM_GAINSetup.java ^
FLU_GAINSetup.java ^
FR10Setup.java ^
IPC901Setup.java ^
IPControlSetup.java ^
J221RFXSetup.java ^
K3115Setup.java ^
L5613Setup.java ^
L6810Setup.java ^
LASER_NDSetup.java ^
LASER_RUSetup.java ^
MARTESetup.java ^
MARTE_DEVICESetup.java ^
MARTE_MHD_BRSetup.java ^
MARTE_MHD_CTRLSetup.java ^
MARTE_MHD_ISetup.java ^
MARTE_MHD_BTSetup.java ^
MARTE_MHD_AC_BCSetup.java ^
MARTE_DEQUSetup.java ^
MARTE_EDA1Setup.java ^
MARTE_EDA3Setup.java ^
MARTE_EDA1_OUTSetup.java ^
MARTE_EDA3_OUTSetup.java ^
MARTE_CONFIGSetup.java ^
MARTE_WAVEGENSetup.java ^
MARTE_BREAKDOWNSetup.java ^
MARTE_RTSMSetup.java ^
MCUSetup.java ^
MHD_BR_TESTSetup.java ^
MPBDecoderSetup.java ^
MPBEncoderSetup.java ^
MPBRecorderSetup.java ^
NI6071ESetup.java ^
OAM01Setup.java ^
PELLETSetup.java ^
PXD114Setup.java ^
RFXABUnitsSetup.java ^
RFXANSALDOSetup.java ^
RFXAxiControlSetup.java ^
RFXChopperSetupSetup.java ^
RFXClockSetup.java ^
RFXControlSetup.java ^
RFXDClockSetup.java ^
RFXFeedforwardSetupSetup.java ^
RFXGClockSetup.java ^
RFXInverterSetupSetup.java ^
RFXMOPSetup.java ^
RFXParametersSetup.java ^
RFXPConfigSetup.java ^
RFXPCSetupSetup.java ^
RFXPMSetupSetup.java ^
RFXPoloidalSetup.java ^
RFXPRConfigSetup.java ^
RFXPulseSetup.java ^
RFXPVSetupSetup.java ^
RFXTFSetupSetup.java ^
RFXTimesSetup.java ^
RFXTimingSetup.java ^
RFXToroidalSetup.java ^
RFXVIConfigSetup.java ^
RFXVISetupSetup.java ^
RFX_PROTECTIONSSetup.java ^
ST133Setup.java ^
SXCSetup.java ^
SPIDERSetup.java ^
SPIDER_SMSetup.java ^
T2ControlSetup.java ^
TOMOSetupSetup.java ^
TR10Setup.java ^
TR32Setup.java ^
TR6_3Setup.java ^
TRCFSetup.java ^
TRCHSetup.java ^
VMEWavesSetup.java ^
WE7000Setup.java ^
WE7116Setup.java ^
WE7275Setup.java ^
WEGroupSetup.java ^
XRAYSetup.java ^
LASER_YAGSetup.java ^
DPO7054Setup.java ^
L9109Setup.java ^
SPIDER_SMSetup.java ^
ZELOS2150GVSetup.java ^
NI6259AISetup.java

SET CLASSPATH=-classpath ".;%MDSPLUS_DIR%\java\classes\jScope.jar;%MDSPLUS_DIR%\java\classes\jTraverser.jar"
SET JAVAC="%JDK_DIR%\bin\javac.exe"
SET JCFLAGS=
SET JAR="%JDK_DIR%\bin\jar.exe"
SET JARDIR=..\java\classes
MKDIR %JARDIR% 2>NUL

ECHO compiling *.java to *.class . . .
%JAVAC% %JCFLAGS% -d %JARDIR% %CLASSPATH% %DEVICES%
SET /A ERROR=%ERRORLEVEL%
IF %ERROR% NEQ 0 GOTO:cleanup

ECHO creating jar packages
PUSHD %JARDIR%
%JAR% -cf jDevices.jar *.class
POPD

:cleanup
ECHO cleaning up
PUSHD %JARDIR%
DEL /Q *.class 2>NUL
POPD
:end
PAUSE
EXIT /B ERROR