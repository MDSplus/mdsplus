..\java\classes\MDSplus.jar : jScope.class CompositeWaveDisplay.class
	%JDK_DIR%\bin\jar.exe -uf ..\java\classes\MDSplus.jar *.class

jScope.class : *.java
	%JDK_DIR%\bin\javac.exe jScope.java TextorBrowseSignals.java

CompositeWaveDisplay.class : *.java
	%JDK_DIR%\bin\javac.exe CompositeWaveDisplay.java


