@ECHO OFF
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
set PATH=E:\PortablePrograms\mingw-w64\x86_64-5.2.0-win32-seh-rt_v4-rev1\mingw64\bin;%PATH%
SET INCLUDEJAVA=-I"%JDK_HOME%\include" -I"%JDK_HOME%\include\win32"
SET INCLUDEMDS=-IC:\git\mdsplus\include
SET GCC=x86_64-w64-mingw32-gcc
SET SOURCE=java\mds\mdslib\JavaMdsLib.c
SET OPTIONS=-Wall -g -O3 -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fpic -fsigned-char -fno-strict-aliasing -pthread -shared
SET LINKEDLIBS64=-m64 -LC:\PortablePrograms\MDSplus\bin_x86_64 -lMdsShr -lTdiShr
SET LINKEDLIBS32=-m32 -LC:\PortablePrograms\MDSplus\bin_x86 -lMdsShr -lTdiShr

SET CMDLINE=%GCC% %OPTIONS% %INCLUDEJAVA% %INCLUDEMDS% %LINKEDLIBS64% -o lib\amd64\JavaMdsLib.dll %SOURCE%
ECHO %CMDLINE%
%CMDLINE%
