SOURCES = MdsPlus.java MdsPlusDescriptor.java MdsPlusEvent.java MdsPlusEvents.java \
MdsPlusException.java 

CLASSES = $(SOURCES:.java=.class)

all : ..\java\classes ..\java\classes\MDSplus.jar
	rem done

..\java\classes : ..\java
	- mkdir ..\java\classes
	
..\java :
	- mkdir ..\java

..\java\classes\MDSplus.jar : MdsPlus.class
	"$(JDK_DIR)\bin\jar.exe" -cf ..\java\classes\MDSplus.jar $(CLASSES)

MdsPlus.class : $(SOURCES)
	"$(JDK_DIR)\bin\javac.exe" -classpath "$(JDK_DIR)\jre\lib\plugin.jar" $(SOURCES)
