.SUFFIXES: .java .class

.java.class:
	"$(JDK_DIR)\bin\javac.exe" -classpath .;..\java\classes\MindTerm.jar $<

APPLETSOURCES = \
jScope\FtuDataProvider.java        jScope\JiVarImpl.java              jScope\Signal.java                 jScope\WaveformManager.java \
jScope\Base64.java                 jScope\Grid.java		              jScope\SignalCache.java            jScope\WaveformMetrics.java \
jScope\JetDataProvider.java        jScope\MdsAccess.java   \
jScope\JetMdsDataProvider.java     jScope\MdsConnection.java          jScope\TwuAccess.java \
jScope\ConnectionEvent.java        jScope\JiDataSource.java           jScope\MdsDataProvider.java        jScope\TwuDataProvider.java  \
jScope\ConnectionListener.java     jScope\JiDim.java                  jScope\MdsMessage.java             jScope\UpdateEvent.java \
jScope\DataAccess.java             jScope\JiNcSource.java             jScope\UpdateEventListener.java    	 \
jScope\DataAccessURL.java          jScope\JiNcVar.java  \
jScope\DataCacheObject.java        jScope\JiNcVarByte.java            jScope\MultiWavePopup.java\
jScope\DataCached.java             jScope\JiNcVarChar.java            jScope\MultiWaveform.java          jScope\WaveData.java  \
jScope\DataProvider.java           jScope\JiNcVarDouble.java          jScope\WaveDisplay.java  \
jScope\DataServerItem.java         jScope\JiNcVarFloat.java           jScope\RandomAccessData.java       jScope\WaveInterface.java         	\
jScope\JiNcVarImp.java             jScope\RdaAccess.java              jScope\WavePopup.java             	 \
jScope\Descriptor.java             jScope\JiNcVarInt.java             jScope\SshTunneling.java          jScope\Waveform.java             	 \
jScope\JiNcVarShort.java  \
jScope\FrameData.java              jScope\JiSlab.java 		jScope\WaveformEditor.java	    jScope\WaveformEditorListener.java \
jScope\JiSlabIterator.java         			    jScope\WaveformEvent.java  	   jScope\SetupWaveformParams.java 	jScope\SignalBox.java \
jScope\Frames.java  jScope\JiVar.java jScope\UniversalDataProvider.java     jScope\WaveformListener.java jScope\NotConnectedDataProvider.java \
jScope\ASCIIDataProvider.java jScope\ImageTransferable.java 


SOURCES = jScope\AboutWindow.java            jScope\FtuDataProvider.java        jScope\JiVarImpl.java              jScope\Signal.java                 jScope\WaveformManager.java \
jScope\Base64.java                 jScope\Grid.java                   jScope\LocalDataProvider.java      jScope\SignalCache.java            jScope\WaveformMetrics.java \
jScope\ColorDialog.java            jScope\JetDataProvider.java        jScope\MdsAccess.java              jScope\TextorBrowseSignals.java     \
jScope\CompositeWaveDisplay.java   jScope\JetMdsDataProvider.java     jScope\MdsConnection.java          jScope\TwuAccess.java              jScope\jScopeFacade.java \
jScope\ConnectionEvent.java        jScope\JiDataSource.java           jScope\MdsDataProvider.java        jScope\TwuDataProvider.java        jScope\jScopeBrowseSignals.java \
jScope\ConnectionListener.java     jScope\JiDim.java                  jScope\MdsMessage.java             jScope\UpdateEvent.java            jScope\jScopeBrowseUrl.java \
jScope\DataAccess.java             jScope\JiNcSource.java             jScope\MdsPlusBrowseSignals.java   jScope\UpdateEventListener.java    jScope\jScopeDefaultValues.java \
jScope\DataAccessURL.java          jScope\JiNcVar.java                jScope\MdsWaveInterface.java       jScope\WaveContainerEvent.java      \
jScope\DataCacheObject.java        jScope\JiNcVarByte.java            jScope\MultiWavePopup.java         jScope\WaveContainerListener.java  \
jScope\DataCached.java             jScope\JiNcVarChar.java            jScope\MultiWaveform.java          jScope\WaveData.java               jScope\jScopeMultiWave.java \
jScope\DataProvider.java           jScope\JiNcVarDouble.java          jScope\ProfileDialog.java          jScope\WaveDisplay.java             \
jScope\DataServerItem.java         jScope\JiNcVarFloat.java           jScope\RandomAccessData.java       jScope\WaveInterface.java          jScope\WaveformEditor.java \
jScope\jScopeProperties.java       jScope.java



DOCS=ConnectionEvent.html +  DataProvider.html  +  UpdateEvent.html  +   WaveData.html + \
ConnectionListener.html +  FrameData.html  +  UpdateEventListener.html  + jScope.html  +  data_setup.jpg + \
frame_popup.jpg + image_setup.jpg + jScope.jpg  +     popup.jpg + CompositeWaveDisplay.html

CLASSES = $(SOURCES:.java=.class)

all : ..\java\classes\MindTerm.jar ..\java\classes\jScope.jar ..\java\classes\WaveDisplay.jar ..\java\classes\jScope.properties
	rem done

..\java\classes\jScope.jar : jScope.class CompositeWaveDisplay.class $(CLASSES)
	- del/q/f/s docs
	- mkdir docs
	copy $(DOCS) docs
	"$(JDK_DIR)\bin\jar.exe" -cf ..\java\classes\jScope.jar  *.class jScope\*.class *.html docs
	- del/q/f/s docs
	- rmdir docs
..\java\classes\MindTerm.jar : MindTerm.jar
	- mkdir ..\java
	- mkdir ..\java\classes
	copy MindTerm.jar ..\java\classes\MindTerm.jar

jScope.class : $(SOURCES)
	"$(JDK_DIR)\bin\javac.exe" -classpath .;..\java\classes\MindTerm.jar $(SOURCES)

..\java\classes\jScope.properties : jScope.properties
	copy jScope.properties ..\java\classes

..\java\classes\WaveDisplay.jar : $(APPLETSOURCES)
	del/q *.class
	"$(JDK_DIR)\bin\javac.exe" -classpath .;..\java\classes\MindTerm.jar $(APPLETSOURCES)
	"$(JDK_DIR)\bin\jar.exe" -cf ..\java\classes\WaveDisplay.jar jScope\*.class
	
CompositeWaveDisplay.class : $(SOURCES)
	"$(JDK_DIR)\bin\javac.exe" -classpath .;..\java\classes\MindTerm.jar jScope\CompositeWaveDisplay.java	
