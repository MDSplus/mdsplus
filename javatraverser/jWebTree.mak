all : ..\java\classes\jWebTree.class
	echo All done

..\java\classes\jWebTree.class : jWebTree.java
	%JDK_DIR%\bin\javac.exe -d ..\java\classes -classpath servlet-2_3-fcs-classfiles.zip;. jWebTree.java