SOURCES = MdsPlus.java MdsPlusDescriptor.java MdsPlusEvent.java MdsPlusEventMonitor.java MdsPlusEvents.java \
MdsPlusException.java MdsPlusJavascriptEvent.java

CLASSES = $(SOURCES:.java=.class)

all : ..\java\classes\MDSplus.jar
	rem done

..\java\classes\MDSplus.jar : MdsPlus.class
	"$(JDK_DIR)\bin\jar.exe" -cf ..\java\classes\MDSplus.jar $(CLASSES)

MdsPlus.class : $(SOURCES)
	"$(JDK_DIR)\bin\javac.exe" -classpath "$(JDK_DIR)\jre\lib\plugin.jar" $(SOURCES)
