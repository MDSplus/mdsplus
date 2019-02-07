@ECHO OFF
<!-- : --- get date Script ---------------------------
FOR /F "delims=" %%x IN ('cscript //nologo "%~f0?.wsf"') DO %%x
GOTO:rest
-->
<job id="Elevate"><script language="VBScript">
Wscript.Echo("set Year=" & DatePart("yyyy", Date))
Wscript.Echo("set Month=0" & DatePart("m", Date))
Wscript.Echo("set Day=0" & DatePart("d", Date))
</script></job>
:rest

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
SET JARDIR=%CD%\..\java\classes
SET SRCDIR=%CD%\java
SET JSCH=%SRCDIR%\..\jsch-0.1.54.jar
SET CLASSPATH=-classpath ".;%JSCH%"
SET JAVAC="%JDK_HOME%\bin\javac.exe" ||rem -Xlint -deprecation
SET JCFLAGS= -g:none||rem -Xlint -deprecation  -O -source 1.6 -target 1.6
SET JAR="%JDK_HOME%\bin\jar.exe"
SET JTMANIFEST=%JARDIR%\JTMANIFEST.mf

SET DEVICES_SRC=devices\*.java devices\acq4xx\*.java

SET TRAV_SRC=^
jtraverser\*.java ^
jtraverser\dialogs\*.java ^
jtraverser\editor\*.java ^
jtraverser\editor\usage\*.java ^
jTraverser.java

SET TOOLS_SRC=jtraverser\tools\*.java

SET MDS_SRC=^
mds\*.java ^
mds\data\*.java ^
mds\data\descriptor\*.java ^
mds\data\descriptor_apd\*.java ^
mds\data\descriptor_a\*.java ^
mds\data\descriptor_r\*.java ^
mds\data\descriptor_r\function\*.java ^
mds\data\descriptor_s\*.java ^
mds\mdsip\*.java ^
mds\mdslib\*.java

SET TRAV_GIF=jtraverser\*.gif

MKDIR %JARDIR% 2>NUL
SET TRAV_CLS=%TRAV_SRC:.java=.class%
SET TOOLS_CLS=%TOOLS_SRC:.java=.class%
SET MDS_CLS=%MDS_SRC:.java=.class%
SET DEVICES_CLS=%DEVICES_SRC:.java=.class%


ECHO compiling *.java to *.class . . .
PUSHD %SRCDIR%
%JAVAC% %JCFLAGS% -d %JARDIR% %CLASSPATH% %MDS_SRC% %TRAV_SRC% %DEVICES_SRC% %TOOLS_SRC% debug\DEBUG.java
SET /A ERROR=%ERRORLEVEL%
POPD
IF %ERROR% NEQ 0 GOTO:cleanup

ECHO gathering data
MKDIR %JARDIR%\jTraverser 2>NUL
COPY /Y %SRCDIR%\jtraverser\*.gif  %JARDIR%\jtraverser>NUL
rem xcopy /Y/S/I %CD%\lib %JARDIR%\lib>NUL
COPY /Y %CD%\JTMANIFEST.mf %JTMANIFEST%>NUL
ECHO Built-Date: %Year%-%Month:~-2%-%Day:~-2% %TIME:~0,8%>>%JTMANIFEST%

ECHO creating jar packages
PUSHD %JARDIR%
COPY %JSCH% %SRCDIR%\jTraverser.jar>NUL
%JAR% -umf %JTMANIFEST% %SRCDIR%\jTraverser.jar %TRAV_CLS% %TRAV_GIF% %MDS_CLS% %DEVICES_CLS% %TOOLS_CLS%
rem %JAR% -cf %SRCDIR%\MDS.jar %MDS_CLS%
rem %JAR% -cf %SRCDIR%\Tools.jar %TOOLS_CLS%
POPD

:cleanup
ECHO cleaning up
PUSHD %JARDIR%
IF %ERROR% NEQ 0 GOTO:end
RMDIR /Q/S . 2>NUL
POPD

COPY /Y %SRCDIR%\jTraverser.jar %CD%\..\javascope\java\

:jtraverser
IF %ERROR% NEQ 0 GOTO:end
IF 0%1 NEQ 0 GOTO:EOF
ECHO start jTraverser?
PAUSE
CLS
java -jar "%SRCDIR%\jTraverser.jar"

:end
PAUSE
EXIT /B ERROR
