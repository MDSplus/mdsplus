include $(MAKE_OPTIONS)


JFLAGS = -g   -d ../java/classes

CLASSES = ../java/classes/*.java

OBJECTS = javamds.o ServerSupport.o
EXPORTS = javamds.export

javascope : 	../shlib ../shlib/libJavaMds.$(SHARE_TYPE) \
		${MDS_PATH}/JavaOpen.fun \
		${MDS_PATH}/JavaClose.fun \
		${MDS_PATH}/JavaResample.fun \
		${MDS_PATH}/JavaDim.fun \
		../java ../java/classes ../java/classes/jScope.class 
	@ echo javaScope done

${MDS_PATH}/JavaOpen.fun : JavaOpen.fun
	cp JavaOpen.fun ${MDS_PATH}/JavaOpen.fun

${MDS_PATH}/JavaClose.fun : JavaClose.fun
	cp JavaClose.fun ${MDS_PATH}/JavaClose.fun

${MDS_PATH}/JavaResample.fun : JavaResample.fun
	cp JavaResample.fun ${MDS_PATH}/JavaResample.fun

${MDS_PATH}/JavaDim.fun : JavaDim.fun
	cp JavaDim.fun ${MDS_PATH}/JavaDim.fun


../java : ; mkdir ../java

../java/classes : ; mkdir ../java/classes

../java/classes/jScope.class : Setup.java \
			      ColorDialog.java \
			      DataProvider.java \
			      ErrorMessage.java \
			      LocalProvider.java \
			      NetworkProvider.java \
			      RowColumnLayout.java \
			      ScopePositionDialog.java \
			      SetupDefaults.java \
			      SetupDataDialog.java \
			      Slider.java \
			      Signal.java \
			      WaveInterface.java \
			      WavePanel.java \
			      WaveSetup.java \
			      MultiWaveform.java \
			      Waveform.java \
			      RotateFilter.java \
			      jScope.java
	javac $(JFLAGS) *.java



clean :
	rm $(CLASSES)
	rm *.o
	rm ../shlib/libJavaMds.$(SHARE_TYPE)

.depend :
	echo no depend needed

../shlib/libJavaMds.$(SHARE_TYPE) : $(OBJECTS) 
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) $(OBJECTS) -L../shlib -lMdsShr -lTreeShr -lTdiShr -lc $(DLD)

../shlib :
	mkdir ../shlib

clean :
	rm *.o
	rm ../shlib/libMdsShr.$(SHARE_TYPE)
	rm .depend





