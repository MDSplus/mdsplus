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
SET JCFLAGS=-O -g:none

SET CLASSPATH=-classpath ".;%PWD%\jsch-0.1.54.jar"

SET OUTFILE=%PWD%\mdsplus-api.jar
SET MANIFEST=
SET SOURCES=^
mds\*.java ^
mds\data\*.java ^
mds\data\descriptor\*.java ^
mds\data\descriptor_apd\*.java ^
mds\data\descriptor_a\*.java ^
mds\data\descriptor_r\*.java ^
mds\data\descriptor_r\function\*.java ^
mds\data\descriptor_s\*.java ^
mds\mdsip\*.java

ECHO compiling *.java to *.class . . .
MKDIR %JARDIR% 2>NUL
PUSHD %CD%\src\main\java
%JAVAC% %JCFLAGS% -d %JARDIR% %CLASSPATH% %SOURCES% debug\DEBUG.java
SET /A ERROR=%ERRORLEVEL%
POPD
IF %ERROR% NEQ 0 GOTO:cleanup

ECHO gathering data
REM XCOPY %CD%\src\main\resources /Y/S/I %JARDIR% >NUL

ECHO creating jar packages 
SET CLASSES=%SOURCES:.java=*.class%
PUSHD %JARDIR%
COPY %PWD%\jsch-0.1.54.jar %OUTFILE% >NUL
%JAR% -uf %OUTFILE% %CLASSES% %RESOURCES%
POPD

:cleanup
ECHO cleaning up
PUSHD %JARDIR%
IF %ERROR% NEQ 0 GOTO:end
RMDIR /Q/S . 2>NUL
POPD
RMDIR /Q %JARDIR% 2>NUL

:end
PAUSE
EXIT /B ERROR
