APPLETSOURCES = \
FtuDataProvider.java        JiVarImpl.java              Signal.java                 WaveformManager.java \
Base64.java                 Grid.java  SignalCache.java            WaveformMetrics.java \
JetDataProvider.java        MdsAccess.java   \
JetMdsDataProvider.java     MdsConnection.java          TwuAccess.java \
ConnectionEvent.java        JiDataSource.java           MdsDataProvider.java        TwuDataProvider.java  \
ConnectionListener.java     JiDim.java                  MdsMessage.java             UpdateEvent.java \
DataAccess.java             JiNcSource.java             UpdateEventListener.java    	 \
DataAccessURL.java          JiNcVar.java  \
DataCacheObject.java        JiNcVarByte.java            MultiWavePopup.java\
DataCached.java             JiNcVarChar.java            MultiWaveform.java          WaveData.java  \
DataProvider.java           JiNcVarDouble.java          WaveDisplay.java  \
DataServerItem.java         JiNcVarFloat.java           RandomAccessData.java       WaveInterface.java         	\
JiNcVarImp.java             RdaAccess.java              WavePopup.java             	 \
Descriptor.java             JiNcVarInt.java             SshTunneling.java          Waveform.java             	 \
JiNcVarShort.java  \
FrameData.java              JiSlab.java 		WaveformEditor.java	    WaveformEditorListener.java\
JiSlabIterator.java         			    WaveformEvent.java  	   SetupWaveformParams.java 	SignalBox.java\
Frames.java                 JiVar.java              UniversalDataProvider.java     WaveformListener.java NotConnectedDataProvider.java \
ASCIIDataProvider.java      ImageTransferable.java

SOURCES = AboutWindow.java            FtuDataProvider.java        JiVarImpl.java              Signal.java                 WaveformManager.java \
Base64.java                 Grid.java                   LocalDataProvider.java      SignalCache.java            WaveformMetrics.java \
ColorDialog.java            JetDataProvider.java        MdsAccess.java              TextorBrowseSignals.java     \
CompositeWaveDisplay.java   JetMdsDataProvider.java     MdsConnection.java          TwuAccess.java              jScope.java \
ConnectionEvent.java        JiDataSource.java           MdsDataProvider.java        TwuDataProvider.java        jScopeBrowseSignals.java \
ConnectionListener.java     JiDim.java                  MdsMessage.java             UpdateEvent.java            jScopeBrowseUrl.java \
DataAccess.java             JiNcSource.java             MdsPlusBrowseSignals.java   UpdateEventListener.java    jScopeDefaultValues.java \
DataAccessURL.java          JiNcVar.java                MdsWaveInterface.java       WaveContainerEvent.java      \
DataCacheObject.java        JiNcVarByte.java            MultiWavePopup.java         WaveContainerListener.java  \
DataCached.java             JiNcVarChar.java            MultiWaveform.java          WaveData.java               jScopeMultiWave.java \
DataProvider.java           JiNcVarDouble.java          ProfileDialog.java          WaveDisplay.java             \
DataServerItem.java         JiNcVarFloat.java           RandomAccessData.java       WaveInterface.java          jScopeProperties.java \
DemoDataProvider.java       JiNcVarImp.java             RdaAccess.java              WavePopup.java              jScopeWaveContainer.java \
Descriptor.java             JiNcVarInt.java             SshTunneling.java            Waveform.java                \
FontSelection.java          JiNcVarShort.java           RowColumnContainer.java     WaveformContainer.java      jScopeWavePopup.java \
FrameData.java              JiSlab.java                 RowColumnLayout.java           \
JiSlabIterator.java         SetupDataDialog.java        WaveformEvent.java          DemoAccess.java		UniversalDataProvider.java\
Frames.java                 JiVar.java                  SetupDefaults.java          WaveformListener.java	NotConnectedDataProvider.java\
SignalsBoxDialog.java	    SignalBox.java		MdsDataClient.java	    MdsIOException.java		SetupWaveformParams.java \
TSDataProvider.java	    AsdexDataProvider.java	WaveformEditor.java	    WaveformEditorListener.java MdsplusParser.java \
ASCIIDataProvider.java      ImageTransferable.java

DOCS=ConnectionEvent.html +  DataProvider.html  +  UpdateEvent.html  +   WaveData.html + \
ConnectionListener.html +  FrameData.html  +  UpdateEventListener.html  + jScope.html  +  data_setup.jpg + \
frame_popup.jpg + image_setup.jpg + jScope.jpg  +     popup.jpg + CompositeWaveDisplay.html

CLASSES = $(SOURCES:.java=.class)

all : ..\java\classes\MindTerm.jar ..\java\classes\jScope.jar ..\java\classes\WaveDisplay.jar ..\java\classes\jScope.properties
	rem done

..\java\classes\jScope.jar : jScope.class CompositeWaveDisplay.class
	- rmdir/s/q docs
	mkdir docs
	copy $(DOCS) docs
	$(JDK_DIR)\bin\jar.exe -cf ..\java\classes\jScope.jar *.class *.html docs
	- rmdir/s/q docs

..\java\classes\MindTerm.jar : MindTerm.jar
	copy MindTerm.jar ..\java\classes\MindTerm.jar

jScope.class : $(SOURCES)
	$(JDK_DIR)\bin\javac.exe -classpath .;..\java\classes\MindTerm.jar $(SOURCES)

..\java\classes\jScope.properties : jScope.properties
	copy jScope.properties ..\java\classes

..\java\classes\WaveDisplay.jar : $(APPLETSOURCES)
	del/q *.class
	$(JDK_DIR)\bin\javac.exe -classpath .;..\java\classes\MindTerm.jar $(APPLETSOURCES)
	$(JDK_DIR)\bin\jar.exe -cf ..\java\classes\WaveDisplay.jar *.class
	
CompositeWaveDisplay.class : $(SOURCES)
	$(JDK_DIR)\bin\javac.exe -classpath .;..\java\classes\MindTerm.jar CompositeWaveDisplay.java	
