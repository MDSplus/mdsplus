all : ..\java\classes\MindTerm.jar ..\java\classes\MDSplus.jar
	rem done

..\java\classes\MDSplus.jar : jScope.class CompositeWaveDisplay.class
	%JDK_DIR%\bin\jar.exe -cf ..\java\classes\MDSplus.jar *.class

..\java\classes\MindTerm.jar : MindTerm.jar
	copy MindTerm.jar ..\java\classes\MindTerm.jar

jScope.class : *.java
	%JDK_DIR%\bin\javac.exe -classpath .;..\java\classes\MindTerm.jar jScope.java TextorBrowseSignals.java *Provider.java

CompositeWaveDisplay.class : *.java
	%JDK_DIR%\bin\javac.exe -classpath .;..\java\classes\MindTerm.jar CompositeWaveDisplay.java


