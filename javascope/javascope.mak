CLASSPATH = -classpath .;..\java\classes\MindTerm.jar
JAVAC = "$(JDK_DIR)\bin\javac.exe"
JAR = "$(JDK_DIR)\bin\jar.exe"

.SUFFIXES: .java .class

.java.class:
	$(JAVAC) $(CLASSPATH) $<

CLASSES = $(SOURCES:.java=.class)

all : ..\java\classes\MindTerm.jar ..\java\classes\jScope.jar ..\java\classes\WaveDisplay.jar ..\java\classes\jScope.properties
	rem done

..\java\classes\jScope.jar: jScope.class CompositeWaveDisplay.class $(CLASSES)
	- del/q/f/s docs
	- mkdir docs
	copy $(DOCS) docs
	$(JAR) -cf $@ *.class jScope\*.class *.html docs
	- del/q/f/s docs
	- rmdir docs

..\java\classes\MindTerm.jar: MindTerm.jar
	- mkdir ..\java
	- mkdir ..\java\classes
	copy MindTerm.jar $@

jScope.class : $(SOURCES)
	$(JAVAC) $(CLASSPATH) $(SOURCES)

..\java\classes\jScope.properties: jScope.properties
	copy jScope.properties $@

..\java\classes\WaveDisplay.jar: $(COMMON_SRC) $(WAVEDISPLAY_SRC)
	del/q *.class
	$(JAVAC) $(CLASSPATH) $**
	$(JAR) -cf $@ $(**:.java=*.class)
	
CompositeWaveDisplay.class : $(SOURCES)
	$(JAVAC) $(CLASSPATH) jScope\CompositeWaveDisplay.java

include Makefile.common

