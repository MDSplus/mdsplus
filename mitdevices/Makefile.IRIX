.SUFFIXES:

.SUFFIXES: .uid .uil

.uil.uid :
	uil -wmd ../shlib/xmdsuil.wmd -o $@ $<
	ln -f $@ ../uid

UIDS = \
A12.uid \
A14.uid \
A3204.uid \
B2408.uid \
B3224.uid \
B5910A.uid \
DSP2904.uid \
EC727.uid \
FERA.uid \
H908.uid \
H911.uid \
H912.uid \
HM650.uid \
HV1440.uid \
HV1443.uid \
HV4032.uid \
HV4032A1.uid \
IDL.uid \
INCAA16.uid \
INCAA4.uid \
INCAA6.uid \
J1819.uid \
J221.uid \
J412.uid \
JOERGER_ADCP.uid \
JOERGER_CG.uid \
JOERGER_DAC16.uid \
JOERGER_TR16.uid \
JOERGER_TR612.uid \
JOERGER_TR812.uid \
L2232.uid \
L2256.uid \
L2415.uid \
L3512.uid \
L4202.uid \
L6810.uid \
L8100.uid \
L8201.uid \
L8206.uid \
L8210.uid \
L8210_HEADERS.uid \
L8212.uid \
L8212_HEADERS.uid \
L8501.uid \
L8590.uid \
L8590_MEM.uid \
L8590_SCLR.uid \
L8818.uid \
L8828.uid \
MDSDCL.uid \
MIT_DECODER.uid \
MIT_ENCODER.uid \
MPB_DECODER.uid \
PARAGON_HIST.uid \
PARAGON_RPT.uid \
PREAMP.uid \
RETICON120.uid \
T4012.uid \
UMCCD.uid \
U_OF_M_SPECT.uid

all : $(UIDS)

clean:
	rm *.uid

