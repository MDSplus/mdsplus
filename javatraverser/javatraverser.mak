..\java\classes\MDSplus.jar : jTraverser\jTraverser.class
	%JDK_DIR%\bin\jar.exe -uvf MDSplus.jar jTraverser\*.class

jTraverser\jTraverser.class : *.java
	%JDK_DIR%\bin\javac.exe -d . *.java

