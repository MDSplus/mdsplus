..\java\classes\MDSplus.jar : ..\javascope\jScope.class
	cd ..\javascope
	%JDK_DIR%\bin\jar.exe -cvf MDSplus.jar *.class

jScope.class : ..\javascope\*.java
	%JDK_DIR%\bin\javac.exe -classpath MRJclasses.zip ..\javascope\*.java

