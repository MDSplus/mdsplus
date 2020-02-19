@ECHO off

REM ============================= /!\ WARNING /!\ =============================
REM Before running this script, make sure that the DEF export files exist!!
REM It is possible to generate these files by calling command "make defs"
REM from "mdsobjects\cpp" directory under MinGW terminal for example
REM ============================= /!\ WARNING /!\ =============================

REM Change VSVER and VSVERNUM to use your current version of Visual Studio

set VSVER=2019
set VSVERNUM=16.0

call "C:\Program Files (x86)\Microsoft Visual Studio\%VSVER%\Community\VC\Auxiliary\Build\vcvars32.bat"
set VisualStudioVersion=%VSVERNUM%
devenv %~dp0..\mdsobjects\MdsObjectsCppShr-VS.sln /build "Release|X64"
devenv %~dp0..\mdsobjects\MdsObjectsCppShr-VS.sln /build "Release|win32"
