include $(MAKE_OPTIONS)


JFLAGS = -g   -d ../java/classes

CLASSES = ../java/classes/*.java

OBJECTS = javamds.o

javascope : 	../shlib ../shlib/libJavaMds.$(SHARE_TYPE) \
		../java ../java/classes ../java/classes/javaScope.class 
	@ echo javaScope done

../java : ; mkdir ../java

../java/classes : ; mkdir ../java/classes

../java/classes/javaScope.class : ../java/classes/Setup.class \
			      ../java/classes/ColorDialog.class \
			      ../java/classes/DataProvider.class \
			      ../java/classes/ErrorMessage.class \
			      ../java/classes/EvaluateWaveform.class \
			      ../java/classes/LocalProvider.class \
			      ../java/classes/NetworkProvider.class \
			      ../java/classes/RowColumnLayout.class \
			      ../java/classes/ScopePositionDialog.class \
			      ../java/classes/ScopeConfiguration.class \
			      ../java/classes/SetupDefaults.class \
			      ../java/classes/Slider.class \
			      ../java/classes/WaveInterface.class \
			      ../java/classes/WavePanel.class \
			      ../java/classes/WaveSetup.class \
			      ../java/classes/WaveformConf.class \
			      ../java/classes/MultiWaveform.class \
			      javaScope.java
	javac $(JFLAGS) javaScope.java


../java/classes/ColorDialog.class : ../java/classes/ScopePositionDialog.class \
				    ColorDialog.java
	javac $(JFLAGS) ColorDialog.java

../java/classes/DataProvider.class : DataProvider.java
	javac $(JFLAGS) DataProvider.java

../java/classes/ErrorMessage.class :  ../java/classes/ScopePositionDialog.class \
				      ErrorMessage.java
	javac $(JFLAGS) ErrorMessage.java

../java/classes/EvaluateWaveform.class :  ../java/classes/MultiWaveform.class \
					  ../java/classes/WaveSetupData.class \
					  ../java/classes/ErrorMessage.class \
					  EvaluateWaveform.java
	javac $(JFLAGS) EvaluateWaveform.java

../java/classes/ExpandExpressionDialog.class :  ../java/classes/SetupDataDialog.class \
						ExpandExpressionDialog.java
	javac $(JFLAGS) ExpandExpressionDialog.java


../java/classes/LocalProvider.class :  LocalProvider.java
	javac $(JFLAGS) LocalProvider.java


../java/classes/MultiWaveform.class :  ../java/classes/Waveform.class \
				       ../java/classes/Signal.class \
				       ../java/classes/Setup.class \
				       MultiWaveform.java
	javac $(JFLAGS) MultiWaveform.java

../java/classes/NetworkProvider.class :  NetworkProvider.java
	javac $(JFLAGS) NetworkProvider.java

../java/classes/RotateFilter.class :  RotateFilter.java
	javac $(JFLAGS) RotateFilter.java

../java/classes/RowColumnLayout.class : RowColumnLayout.java
	javac $(JFLAGS) RowColumnLayout.java

../java/classes/ScopeConfiguration.class :   ../java/classes/WaveformConf.class \
					     ScopeConfiguration.java
	javac $(JFLAGS) ScopeConfiguration.java

../java/classes/ScopePositionDialog.class : ScopePositionDialog.java
	javac $(JFLAGS) ScopePositionDialog.java

../java/classes/Setup.class :  ../java/classes/WaveSetup.class \
			       ../java/classes/MultiWaveform.class \
				Setup.java
	javac $(JFLAGS) Setup.java

../java/classes/SetupDataDialog.class :   ../java/classes/Setup.class \
					  ../java/classes/SignalError.class \
					  ../java/classes/ErrorMessage.class \
					  ../java/classes/EvaluateWaveform.class \
					  ../java/classes/WaveformConf.class \
					  ../java/classes/ExpandExpression.class \
					  SetupDataDialog.java
	javac $(JFLAGS) SetupDataDialog.java


../java/classes/SetupDefaults.class :   ../java/classes/EvaluateWaveform.class \
					  SetupDefaults.java
	javac $(JFLAGS) SetupDefaults.java

../java/classes/Signal.class : Signal.java
	javac $(JFLAGS) Signal.java

../java/classes/SignalError.class :   ../java/classes/WaveSetupData.class \
					  SignalError.java
	javac $(JFLAGS) SignalError.java

../java/classes/Slider.class : Slider.java
	javac $(JFLAGS) Slider.java


../java/classes/WaveInterface.class :   ../java/classes/Signal.class \
					  WaveInterface.java
	javac $(JFLAGS) WaveInterface.java


../java/classes/WavePanel.class :   ../java/classes/ScopeConfiguration.class \
				    ../java/classes/MultiWaveform.class \
					  WaveInterface.java
	javac $(JFLAGS) WavePanel.java

../java/classes/WaveSetup.class : WaveSetup.java
	javac $(JFLAGS) WaveSetup.java

../java/classes/WaveSetupData.class : WaveSetupData.java
	javac $(JFLAGS) WaveSetupData.java


../java/classes/Waveform.class :  ../java/classes/Signal.class \
				  ../java/classes/RotateFilter.class \
			          Waveform.java
	javac $(JFLAGS) Waveform.java

../java/classes/WaveformConf.class : WaveformConf.java
	javac $(JFLAGS) WaveformConf.java


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





