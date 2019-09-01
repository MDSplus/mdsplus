include Makefile.common

CLASSPATH = -classpath .;..\java\classes\MindTerm.jar
JAVAC = "$(JDK_DIR)\bin\javac.exe"
JAR = "$(JDK_DIR)\bin\jar.exe"
JARDIR = ..\java\classes

SOURCES = $(COMMON_SRC) $(JSCOPE_SRC) $(WAVEDISPLAY_SRC)
WAVECLASSES = $(COMMON_SRC:.java=*.class)
CLASSES = $(WAVECLASSES) $(JSCOPE_SRC:.java=*.class)
all: $(JARDIR) $(JARDIR)\jScope.properties $(JARDIR)\MindTerm.jar $(JARDIR)\jScope.jar $(JARDIR)\WaveDisplay.jar
	rem done

$(JARDIR):
	- mkdir ..\java
	- mkdir $@

$(JARDIR)\jScope.properties: jScope.properties
	copy $** $@

$(JARDIR)\MindTerm.jar: MindTerm.jar
	copy $** $@

class.stamp: $(SOURCES)
	$(JAVAC) $(CLASSPATH) $(SOURCES)
	echo x > class.stamp

$(JARDIR)\jScope.jar: class.stamp
	- del/q/f/s docs
	- mkdir docs
	for %I in ($(DOCS)) do copy %I docs
	$(JAR) -cf $@ $(CLASSES) docs
	- del/q/f/s docs
	- rmdir docs

$(JARDIR)\WaveDisplay.jar: class.stamp
	$(JAR) -cf $@ $(WAVECLASSES)

