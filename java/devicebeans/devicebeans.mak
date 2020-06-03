CLASSPATH = -classpath .;..\java\classes\jScope.jar
JAVAC = "$(JDK_DIR)\bin\javac.exe"
JAR = "$(JDK_DIR)\bin\jar.exe"
JARDIR = ..\java\classes

.SUFFIXES: .class .java
.java.class:
	$(JAVAC) $*.java


SOURCES = DeviceApply.java\
DeviceApplyBeanInfo.java\
DeviceButtons.java\
DeviceButtonsBeanInfo.java\
DeviceButtonsCustomizer.java\
DeviceCancel.java\
DeviceCancelBeanInfo.java\
DeviceChannel.java\
DeviceChannelBeanInfo.java\
DeviceChannelCustomizer.java\
DeviceChoice.java\
DeviceChoiceBeanInfo.java\
DeviceChoiceCustomizer.java\
DeviceComponent.java\
DeviceControl.java\
DeviceCustomizer.java\
DeviceDispatch.java\
DeviceDispatchBeanInfo.java\
DeviceDispatchField.java\
DeviceField.java\
DeviceFieldBeanInfo.java\
DeviceFieldCustomizer.java\
DeviceOk.java\
DeviceLabel.java\
DeviceLabelBeanInfo.java\
DeviceLabelCustomizer.java\
DeviceMultiComponent.java\
DeviceOk.java\
DeviceOkBeanInfo.java\
DeviceParameters.java\
DeviceParametersBeanInfo.java\
DeviceReset.java\
DeviceResetBeanInfo.java\
DeviceSetup.java\
DeviceSetupBeanInfo.java\
DeviceTable.java\
DeviceTableBeanInfo.java\
DeviceTableCustomizer.java\
DeviceWave.java\
DeviceWaveBeanInfo.java\
DeviceWaveDisplay.java\
DeviceWaveDisplayBeanInfo.java\
DeviceWaveDisplayCustomizer.java\
DeviceWaveParameters.java\
DeviceWaveParametersBeanInfo.java



GIFS = DeviceApply.gif \
       DeviceChoice.gif \
       DeviceReset.gif \
       DeviceButtons.gif \
       DeviceDispatch.gif \
       DeviceSetup.gif \
       DeviceLabel.gif \
       device.gif 

all : $(SOURCES)
	$(JAVAC) $(CLASSPATH) $(SOURCES)
	$(JAR) -cmf DeviceBeansManifest.mf ..\java\classes\DeviceBeans.jar *.class *.gif

