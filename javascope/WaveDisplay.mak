..\java\classes\WaveDisplay.jar : ..\java ..\java\classes WaveDisplay.class
	%JDK_DIR%\bin\jar.exe -cf ..\java\classes\WaveDisplay.jar *.class

WaveDisplay.class : *.java
	%JDK_DIR%\bin\javac.exe WaveDisplay.java

..\java :
	mkdir ..\java

..\java\classes :
	mkdir ..\java\classes

