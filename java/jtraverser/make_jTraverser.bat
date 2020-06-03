@ECHO OFF
PUSHD %~dp0
SET PWD=%CD%
ECHO preparing
if defined JDK_HOME GOTO:start
rem This script located the current version of
rem "Java Development Kit" and sets the
rem %JDK_HOME% environment variable
setlocal ENABLEEXTENSIONS
set KEY=HKEY_LOCAL_MACHINE\SOFTWARE\JavaSoft\Java Development Kit
FOR /F "usebackq tokens=2,* skip=2" %%L IN (`reg query "%KEY%" /v CurrentVersion`) DO SET JDKVER=%%M
FOR /F "usebackq tokens=2,* skip=2" %%L IN (`reg query "%KEY%\%JDKVER%" /v JavaHome`) DO SET JDK_HOME="%%M"
SET JDK_HOME=%JDK_HOME:"=%
IF EXIST "%JDK_HOME%" GOTO:start
ECHO JDK not found. Please set %%JDK_HOME%% to the root path of your jdk.
SET /A ERROR=1
GOTO:end

:start
SET JAVAC="%JDK_HOME%\bin\javac.exe"
SET JAR="%JDK_HOME%\bin\jar.exe"
SET JARDIR=%PWD%\classes
SET JCFLAGS=-O -source 6 -g:none
SET CLASSPATH=-classpath ".;%MDSPLUS_DIR%\java\classes\mdsobjects.jar;%MDSPLUS_DIR%\java\classes\jScope.jar"

SET OUTFILE=%PWD%\jTraverser.jar
SET SOURCES=*.java^
 mds/devices/*.java
SET RESOURCES=*.gif

ECHO compiling *.java to *.class . . .
MKDIR %JARDIR% 2>NUL
PUSHD %PWD%\src\main\java
%JAVAC% %JCFLAGS% -d %JARDIR% %CLASSPATH% %SOURCES%
SET /A ERROR=%ERRORLEVEL%
POPD
IF %ERROR% NEQ 0 GOTO:cleanup

ECHO gathering data
XCOPY /Y/S/I %PWD%\src\main\resources %JARDIR% >NUL

ECHO creating jar packages
SET CLASSES=%SOURCES:.java=*.class%
PUSHD %JARDIR%
DEL /Q %OUTFILE%
%JAR% -cf %OUTFILE% %CLASSES% %RESOURCES%
POPD

:cleanup
ECHO cleaning up
PUSHD %JARDIR%
IF %ERROR% NEQ 0 GOTO:end
RMDIR /Q/S . 2>NUL
POPD
RMDIR /Q %JARDIR%

:jtraverser
IF %ERROR% NEQ 0 GOTO:end
IF 0%1 NEQ 0 GOTO:EOF
ECHO start jTraverser?
PAUSE
CLS
"%JDK_HOME%\bin\java.exe" -cp ^
"%OUTFILE%^
;%MDSPLUS_DIR%\java\classes\mdsobjects.jar^
;%MDSPLUS_DIR%\java\classes\jScope.jar^
;%MDSPLUS_DIR%\java\classes\DeviceBeans.jar^
;%MDSPLUS_DIR%\java\classes\jDevices.jar^
" -Xss5M jTraverser

:end
PAUSE
EXIT /B ERROR
