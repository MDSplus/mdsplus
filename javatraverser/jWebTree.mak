all : ..\java\classes\jWebTree.class
	echo All done

..\java\classes\jWebTree.class : jWebTree.java
	%JDK_DIR%\bin\javac.exe -d ..\java\classes jWebTree.java