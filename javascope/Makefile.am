JAVAROOT = $(builddir)
EXTRA_DIST = $(MINDTERM)
CLEANFILES = $(bin_SCRIPTS) $(java_DATA)

MINDTERM = $(srcdir)/MindTerm.jar
CLASSPATH_ENV = CLASSPATH=$(MINDTERM)

# Create the shell script that runs the program
bin_SCRIPTS = scripts/jScope
EXTRA_DIST += jScope.template
$(bin_SCRIPTS): jScope.template
	$(INSTALL) -D $< $@

# Documentation
dist_docs_DATA = jScope.html jScope.jpg popup.jpg about_jscope.jpg
dist_docs_DATA += CompositeWaveDisplay.html
dist_docs_DATA += ConnectionEvent.html ConnectionListener.html
dist_docs_DATA += DataProvider.html
dist_docs_DATA += FrameData.html
dist_docs_DATA += ReadMe.html data_popup.jpg data_setup.jpg frame_popup.jpg image_setup.jpg
dist_docs_DATA += UpdateEvent.html UpdateEventListener.html
dist_docs_DATA += WaveData.html WaveDisplay.html
docsdir = $(exec_prefix)/java/classes/docs

dist_java_DATA = jScope.properties MindTerm.jar

java_DATA = jScope.jar WaveDisplay.jar
javadir = $(exec_prefix)/java/classes
$(java_DATA): classdist_java.stamp

EXTRA_DIST += colors1.tbl
jScope.jar: colors1.tbl
	$(JAR) c0f $@ $(JSCOPE_CLASS) $(dist_docs_DATA) $<

WaveDisplay.jar:
	$(JAR) c0f $@ $(WAVEDISPLAY_CLASS)

dist_java_JAVA = $(JSCOPE_SRC) $(WAVEDISPLAY_SRC) $(COMMON_SRC)
COMMON_CLASS = $(COMMON_SRC:.java=*.class)
JSCOPE_CLASS = $(COMMON_CLASS) $(JSCOPE_SRC:.java=*.class)
WAVEDISPLAY_CLASS = $(COMMON_CLASS) $(WAVEDISPLAY_SRC:.java=*.class)
CLEANFILES += $(JSCOPE_CLASS) $(WAVEDISPLAY_CLASS)

COMMON_SRC = \
	jScope/AboutWindow.java               jScope/ASCIIDataProvider.java       jScope/AsdexDataProvider.java     \
	jScope/Base64.java                    jScope/ColorDialog.java             jScope/ColorMapDialog.java        \
	jScope/ColorMap.java                  jScope/ConnectionEvent.java         jScope/ConnectionListener.java    \
	jScope/ContourSignal.java             jScope/DataAccess.java              jScope/DataAccessURL.java         \
	jScope/DataCached.java                jScope/DataCacheObject.java         jScope/DataProvider.java          \
	jScope/DataServerItem.java            jScope/Descriptor.java              jScope/FakeTWUProperties.java     \
	jScope/FontSelection.java             jScope/FrameData.java               jScope/Frames.java                \
	jScope/FtuDataProvider.java           jScope/Grid.java                    jScope/ImageTransferable.java     \
	jScope/JetDataProvider.java           jScope/JetMdsDataProvider.java      jScope/JiDataSource.java          \
	jScope/JiDim.java                     jScope/JiNcSource.java              jScope/JiNcVarByte.java           \
	jScope/JiNcVarChar.java               jScope/JiNcVarDouble.java           jScope/JiNcVarFloat.java          \
	jScope/JiNcVarImp.java                jScope/JiNcVarInt.java              jScope/JiNcVar.java               \
	jScope/JiNcVarShort.java              jScope/JiSlabIterator.java          jScope/JiSlab.java                \
	jScope/JiVarImpl.java                 jScope/JiVar.java                   jScope/jScopeBrowseSignals.java   \
	jScope/jScopeBrowseUrl.java           jScope/jScopeDefaultValues.java     jScope/jScopeFacade.java          \
	jScope/jScopeMultiWave.java           jScope/jScopeProperties.java        jScope/jScopeWaveContainer.java   \
	jScope/jScopeWavePopup.java           jScope/MdsAccess.java               jScope/MdsConnection.java         \
	jScope/MdsDataProvider.java           jScope/MdsMessage.java              jScope/MdsplusParser.java         \
	jScope/MdsWaveInterface.java          jScope/MultiWaveform.java           jScope/MultiWavePopup.java        \
	jScope/NotConnectedDataProvider.java  jScope/ProfileDialog.java           jScope/PropertiesEditor.java      \
	jScope/RandomAccessData.java          jScope/RdaAccess.java               jScope/RowColumnContainer.java    \
	jScope/RowColumnLayout.java           jScope/SetupDataDialog.java         jScope/SetupDefaults.java         \
	jScope/SetupWaveformParams.java       jScope/SignalBox.java               jScope/SignalCache.java           \
	jScope/Signal.java                    jScope/SignalsBoxDialog.java        jScope/SshTunneling.java          \
	jScope/TSDataProvider.java            jScope/TwuAccess.java               jScope/TwuDataProvider.java       \
	jScope/TWUFetchOptions.java           jScope/TwuNameServices.java         jScope/TWUProperties.java         \
	jScope/TWUSignal.java                 jScope/TwuSimpleFrameData.java      jScope/TwuSingleSignal.java       \
	jScope/TwuWaveData.java               jScope/UniversalDataProvider.java   jScope/UpdateEvent.java           \
	jScope/UpdateEventListener.java       jScope/WaveContainerEvent.java      jScope/WaveContainerListener.java \
	jScope/WaveData.java                  jScope/WaveDisplay.java             jScope/WaveformContainer.java     \
	jScope/WaveformEditor.java            jScope/WaveformEditorListener.java  jScope/WaveformEvent.java         \
	jScope/Waveform.java                  jScope/WaveformListener.java        jScope/WaveformManager.java       \
	jScope/WaveformMetrics.java           jScope/WaveInterface.java           jScope/WavePopup.java

WAVEDISPLAY_SRC = 

JSCOPE_SRC = \
	jScope/CompositeWaveDisplay.java \
	jScope/DemoAccess.java           \
	jScope/DemoDataProvider.java     \
	jScope/LocalDataProvider.java    \
	jScope/MdsDataClient.java        \
	jScope/MdsIOException.java       \
	jScope/MdsPlusBrowseSignals.java \
	jScope/TextorBrowseSignals.java

