.SUFFIXES: .java .class

.java.class:
	"$(JDK_DIR)\bin\javac.exe" -classpath .;..\java\classes\MindTerm.jar $<

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

include Makefile.common

