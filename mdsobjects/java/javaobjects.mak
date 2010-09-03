SOURCES = MDSplus\*.java

CLASSES = $(SOURCES:.java=.class)

all : ..\..\java\classes\MDSobjects.jar
	rem done

..\..\java\classes\MDSobjects.jar : $(SOURCES)
	"$(JDK_DIR)\bin\javac.exe" -classpath ..\..\java\classes\jScope.jar  $(SOURCES)
	"$(JDK_DIR)\bin\jar.exe" -cf ..\..\java\classes\MDSobjects.jar $(CLASSES)
