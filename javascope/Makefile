include $(MAKE_OPTIONS)


JFLAGS = -g   -d ../java/classes

CLASSES = ../java/classes/*.java


javascope :	../java ../java/classes ../java/classes/jScope.class 
	@ echo javaScope done


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

.depend :
	echo no depend needed





