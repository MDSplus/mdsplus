..\java\classes\MDSplus.jar : jScope.class CompositeWaveDisplay.class
	%JDK_DIR%\bin\jar.exe -cf ..\java\classes\MDSplus.jar *.class

jScope.class : *.java
	%JDK_DIR%\bin\javac.exe jScope.java TextorBrowseSignals.java *Provider.java

CompositeWaveDisplay.class : *.java
	%JDK_DIR%\bin\javac.exe CompositeWaveDisplay.java


