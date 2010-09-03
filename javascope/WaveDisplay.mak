..\java\classes\WaveDisplay.jar : ..\java ..\java\classes WaveDisplay.class
	"$(JDK_DIR)\bin\jar.exe" -cf ..\java\classes\WaveDisplay.jar jScope\*.class

WaveDisplay.class : *.java
	"$(JDK_DIR)\bin\javac.exe" jScope\WaveDisplay.java

..\java :
	- mkdir ..\java

..\java\classes :
	- mkdir ..\java\classes

