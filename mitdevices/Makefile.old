UIDS = \
../uid/A12.uid \
../uid/A14.uid \
../uid/A3204.uid \
../uid/B2408.uid \
../uid/B3224.uid \
../uid/B5910A.uid \
../uid/DSP2904.uid \
../uid/EC727.uid \
../uid/FERA.uid \
../uid/H908.uid \
../uid/H911.uid \
../uid/H912.uid \
../uid/HM650.uid \
../uid/HV1440.uid \
../uid/HV1443.uid \
../uid/HV4032.uid \
../uid/HV4032A1.uid \
../uid/IDL.uid \
../uid/INCAA16.uid \
../uid/INCAA4.uid \
../uid/INCAA6.uid \
../uid/J1819.uid \
../uid/J221.uid \
../uid/J412.uid \
../uid/JOERGER_ADCP.uid \
../uid/JOERGER_CG.uid \
../uid/JOERGER_DAC16.uid \
../uid/JOERGER_TR16.uid \
../uid/JOERGER_TR612.uid \
../uid/JOERGER_TR812.uid \
../uid/L2232.uid \
../uid/L2256.uid \
../uid/L2415.uid \
../uid/L3512.uid \
../uid/L4202.uid \
../uid/L6810.uid \
../uid/L8100.uid \
../uid/L8201.uid \
../uid/L8206.uid \
../uid/L8210.uid \
../uid/L8210_HEADERS.uid \
../uid/L8212.uid \
../uid/L8212_HEADERS.uid \
../uid/L8501.uid \
../uid/L8590.uid \
../uid/L8590_MEM.uid \
../uid/L8590_SCLR.uid \
../uid/L8818.uid \
../uid/L8828.uid \
../uid/MDSDCL.uid \
../uid/MIT_DECODER.uid \
../uid/MIT_ENCODER.uid \
../uid/MPB_DECODER.uid \
../uid/PARAGON_HIST.uid \
../uid/PARAGON_RPT.uid \
../uid/PREAMP.uid \
../uid/RETICON120.uid \
../uid/T4012.uid \
../uid/UMCCD.uid \
../uid/U_OF_M_SPECT.uid

mitdevices : $(UIDS)

clean:
	- rm $(UIDS)
	rm .depend

.depend:
	find . -name '*.uil' -exec ./makeuildep {} \; > .depend

include $(DEPEND)
