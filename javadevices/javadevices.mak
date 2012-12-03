.SUFFIXES: .class .java
.java.class:
	"%JDK_DIR%\bin\javac" $*.java


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
DFLUSetupSetup.java\
DIO2EncoderSetup.java\
DIO2Setup.java\
DIO4Setup.java\
E1463Setup.java\
EDAMSetupSetup.java\
EM_EQU_TESTSetup.java\
EM_FLU_TESTSetup.java\
EM_GAINSetup.java\
FLU_GAINSetup.java\
FR10Setup.java\
IPC901Setup.java\
IPControlSetup.java\
J221RFXSetup.java\
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
XRAYSetup.java\
LASER_YAGSetup.java\
DPO7054Setup.java\
L9109Setup.java\
SPIDER_SMSetup.java\
ZELOS2150GVSetup.java\
NI6259AISetup.java



all : $(DEVICES)
	"$(JDK_DIR)\bin\javac" -classpath .;..\java\classes\jScope.jar;..\java\classes\jTraverser.jar $(DEVICES)
	"$(JDK_DIR)\bin\jar" -cf ..\java\classes\jDevices.jar *.class 

