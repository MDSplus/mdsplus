include $(MAKE_OPTIONS)


JFLAGS = -g -depend -deprecation -d ../java/classes

CLASSES = \
	../java/classes/AsynchUpdater.class \
	../java/classes/CInputStream.class \
	../java/classes/DataProvider.class \
	../java/classes/ErrorMessage.class \
	../java/classes/RotateFilter.class \
	../java/classes/Waveform.class \
	../java/classes/WaveSetup.class

javascope : ../java ../java/classes ../java/classes/Scope.class
	@ echo javascope done

../java : ; mkdir ../java

../java/classes : ; mkdir ../java/classes

../java/classes/Scope.class : ../java/classes/Setup.class \
			      ../java/classes/Waveform.class \
			      ../java/classes/MultiWaveform.class \
			      ../java/classes/PopScopes.class \
			      Scope.java
	javac $(JFLAGS) Scope.java

../java/classes/Setup.class : 	../java/classes/SetupData.class \
				../java/classes/WaveSetup.class \
				Setup.java
	javac $(JFLAGS) Setup.java

../java/classes/PopScopes.class : PopScopes.java
	javac $(JFLAGS) PopScopes.java

../java/classes/Signal.class : Signal.java
	javac $(JFLAGS) Signal.java

../java/classes/RotateFilter.class : RotateFilter.java
	javac $(JFLAGS) RotateFilter.java

../java/classes/Waveform.class \
../java/classes/WaveSetup.class \
../java/classes/MultiWaveform.class \
../java/classes/WaveInterface.class :	../java/classes/Signal.class \
				 	../java/classes/RotateFilter.class \
					WaveInterface.java \
					Waveform.java \
					WaveSetup.java \
					MultiWaveform.java
	javac $(JFLAGS) WaveInterface.java Waveform.java WaveSetup.java MultiWaveform.java

../java/classes/SetupData.class : 	../java/classes/ScopePositionDialog.class \
					../java/classes/WaveformConf.class \
					../java/classes/ExpandExpressionDialog.class \
					SetupData.java
	javac $(JFLAGS) SetupData.java

../java/classes/ScopePositionDialog.class : ScopePositionDialog.java
	javac $(JFLAGS) ScopePositionDialog.java

../java/classes/WaveformConf.class : WaveformConf.java
	javac $(JFLAGS) WaveformConf.java

../java/classes/ExpandExpressionDialog.class : ExpandExpressionDialog.java
	javac $(JFLAGS) ExpandExpressionDialog.java

clean :
	rm $(CLASSES)

.depend :
	echo no depend needed

