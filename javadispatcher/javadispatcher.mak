..\java\classes\MDSplus.jar : jDispatcher.class
	%JDK_DIR%\bin\jar.exe -uf ..\java\classes\MDSplus.jar *.class

jDispatcher.class : *.java
	set CLASSPATH=..\java\classes\MDSplus.jar
	%JDK_DIR%\bin\javac.exe *.java

