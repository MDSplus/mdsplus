all : jTraverser.class
	%JDK_DIR%\bin\jar.exe -uf ..\java\classes\MDSplus.jar *.class *.gif

jTraverser.class : *.java
	%JDK_DIR%\bin\javac.exe jTraverser.java