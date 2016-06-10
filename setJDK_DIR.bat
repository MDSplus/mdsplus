@ECHO OFF
rem This script locates the current version of
rem "Java Development Kit" and sets the
rem %JDK_PATH% environment variable
SETLOCAL ENABLEEXTENSIONS
SET KEY=HKEY_LOCAL_MACHINE\SOFTWARE\JavaSoft\Java Development Kit
FOR /F "usebackq tokens=2,* skip=2" %%L IN (`reg query "%KEY%" /v CurrentVersion`) DO SET JDKVER=%%M
FOR /F "usebackq tokens=2,* skip=2" %%L IN (`reg query "%KEY%\%JDKVER%" /v JavaHome`) DO SET JDK_DIR="%%M"
SET JDK_DIR=%JDK_DIR%
SETX JDK_DIR %JDK_DIR%