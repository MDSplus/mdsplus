all : jTraverser.class
	%JDK_DIR%\bin\jar.exe -uvf ..\java\classes\MDSplus.jar *.class

jTraverser.class : *.java
	%JDK_DIR%\bin\javac.exe -classpath ..\javascope -d . *.java