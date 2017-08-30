EXTRA_DIST = *.form
CLEANFILES = $(java_DATA)

JAVAROOT = $(builddir)
CLASSPATH_ENV = CLASSPATH=$(top_builddir)/javatraverser/jTraverser.jar
javadir = $(exec_prefix)/java/classes

# Giant list of source files follow
SOURCES = \
ParameterSetting.java \
ParameterSettingOld.java\
DEMOADCSetup.java\
DIO4Setup.java\
TRCHSetup.java\
ADC265Setup.java\
E1463Setup.java\
VMEWavesSetup.java\
MPBDecoderSetup.java\
RFXClockSetup.java \
RFXPulseSetup.java \
RFXDClockSetup.java \
RFXGClockSetup.java \
MPBEncoderSetup.java \
RFXTimingSetup.java \
MPBRecorderSetup.java \
TRCFSetup.java \
Aurora14Setup.java \
ACQD265Setup.java \
NI6071ESetup.java\
PELLETSetup.java\
L5613Setup.java\
RFXTimesSetup.java\
RFXPoloidalSetup.java\
RFXToroidalSetup.java\
RFXPMSetupSetup.java\
RFXPCSetupSetup.java \
RFXPC4SetupSetup.java \
RFXPVSetupSetup.java \
RFXTFSetupSetup.java \
RFXABUnitsSetup.java \
RFXChopperSetupSetup.java \
RFXInverterSetupSetup.java \
RFXFeedforwardSetupSetup.java \
RFXPRConfigSetup.java \
RFXPRConfigV1Setup.java \
TR10Setup.java \
K3115Setup.java \
DIO2Setup.java \
DIO2EncoderSetup.java \
WE7000Setup.java \
WE7116Setup.java \
WE7275Setup.java \
WEGroupSetup.java \
RFXControlSetup.java \
T2ControlSetup.java \
TR32Setup.java \
TR6_3Setup.java \
FR10Setup.java \
OAM01Setup.java \
RFXMOPSetup.java \
RFXANSALDOSetup.java \
IPC901Setup.java \
CADHSetup.java \
J221RFXSetup.java \
L6810Setup.java \
RFXVISetupSetup.java \
RFXVIConfigSetup.java \
LASER_NDSetup.java\
EDAMSetupSetup.java\
DEQUSetupSetup.java\
EM_GAINSetup.java\
EM_EQU_TESTSetup.java\
CHVPSSetup.java\
CHVPS_SETSetup.java\
RFXParametersSetup.java\
CAMERASetup.java\
FLU_GAINSetup.java\
DFLUSetupSetup.java\
EM_FLU_TESTSetup.java\
BOLOSetup.java\
XRAYSetup.java\
SXCSetup.java\
RFXAxiControlSetup.java\
RFXAxiToroidalControlSetup.java\
TOMOSetupSetup.java\
TSEdgeCCDSetup.java\
LASER_RUSetup.java\
GEN_GAINSetup.java\
EDAM_CPCISetupSetup.java\
FEGroupSetup.java\
ISIS_GAINSetup.java\
FASTCAMSetup.java\
ST133Setup.java\
RFXVersionSetup.java\
WE800Setup.java \
WE900Setup.java \
CAENV1731Setup.java \
CAENV1740Setup.java \
CYGNET4KSetup.java \
PLRMSetup.java \
DPO7054Setup.java \
L9109Setup.java \
LASER_NDRTSetup.java \
IPControlSetup.java \
EQU_GAINSetup.java \
EM_EQU_TEST_NEWSetup.java\
OAM01Setup.java \
RFXPolControlSetup.java \
RFXAxiControlSetup.java \
RFXTorControlSetup.java \
RFXAxiToroidalControlSetup.java \
RFXMHDSetup.java \
EDAM_CPCISetupV1Setup.java \
MCAMERASetup.java \
HMSPECTROSetup.java \
RFXDiagTimesSetup.java \
RFXDiagTimesV1Setup.java \
MHD_BR_TESTSetup.java\
EDA3VmeConfigSetup.java\
MCUSetup.java\
SIS3820Setup.java\
FEMTOSetup.java\
CAENDT5720Setup.java\
CAENDT5724Setup.java\
RFXVConfigSetup.java\
MARTESetup.java\
MARTE_MHD_BRSetup.java\
MARTE_MHD_CTRLSetup.java\
MARTE_MHD_ISetup.java\
MARTE_MHD_AC_BCSetup.java\
MARTE_MHD_BTSetup.java\
MARTE_DEQUSetup.java\
MARTE_EDA1Setup.java\
MARTE_EDA3Setup.java\
MARTE_EDA1_OUTSetup.java\
MARTE_EDA3_OUTSetup.java\
MARTE_CONFIGSetup.java\
MARTE_WAVEGENSetup.java\
MARTE_BREAKDOWNSetup.java\
MARTE_RTSMSetup.java\
MARTE_XRAYSetup.java\
MARTE_NESetup.java\
LASER_YAGSetup.java\
SPIDER_SMSetup.java\
SPIDERSetup.java\
ZELOS2150GVSetup.java\
NI6259AISetup.java\
FAKECAMERASetup.java\
AWG2021Setup.java\
RFXWAVESETUPSetup.java\
RFX_PROTECTIONSSetup.java\
FLIRSC65XSetup.java\
BASLERACASetup.java\
NI6368AISetup.java\
ACQIPPSETUPSetup.java\
DIO2_ENCDECSetup.java\
MARTE_DEVICESetup.java \
RFXVICONTROLSetup.java \
REDPYTADCSetup.java \
JRG_ADC32ASetup.java \
A3248Setup.java

java_DATA = jDevices.jar

if USE_JARS_DIR

jDevices.jar: @JARS_DIR@/javadevices/jDevices.jar
	cp $< $@

else

java_JAVA = $(SOURCES)

# Build the final jar

jDevices.jar: classjava.stamp
	$(JAR) c0f $@ $(java_JAVA:.java=*.class)

endif

