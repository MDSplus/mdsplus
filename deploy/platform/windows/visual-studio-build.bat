@ECHO off
REM This is only used for creating vs compatible lib files included in the Windows installer
REM The scipt only works in the wine environment of the current docker image mdsplus/builder:windows

REM A subset of the Visual Studio binaries, libs and headers
REM \Microsoft-Visual-Studio directory is from the C:\Program Files (x86)\Microsoft Visual Studio\ directory on
REM a Windows system with the free Visual Studio 2017 Community edition installed.
REM \Windows-Kits is from the C:\Program Files (x64)\Windows Kits\ directory.
REM The \vc directory is obtained from the following directories when Visual Studio 2017 Community is
REM installed on a Windows system:

SET MVS=\Microsoft-Visual-Studio\2017\Community\VC\Tools\MSVC\14.12.25827
SET WK=\Windows-Kits\10
SET PATH=%MVS%\bin\HostX64\x64;%WK%\bin\10.0.16299.0\x64
SET INCLUDE=%MVS%\include;%WK%\include\10.0.16299.0\shared;%WK%\include\10.0.16299.0\ucrt;%WK%\include\10.0.16299.0\um;
SET LIB=%MVS%\lib\x64;%WK%\Lib\10.0.16299.0\um\x64

REM Various directories used for compiles, links and lib generation

REM Obtain SRCDIR from location of this script

SET OLDDIR=%CD%
CD %~dp0..\..\..
SET SRCDIR=%CD%
SET CPP=%SRCDIR%\mdsobjects\cpp
SET BUILD64=\workspace\releasebld\64
SET BUILD32=\workspace\releasebld\32
SET DEFS=%BUILD64%\defs
SET TMP=\tmp
SET TEMP=\tmp
echo %CCP%
CD %CPP%

REM Use Visual studio compiler to compile the CPP modules

REM _WINDOWS - required for zlib
SET CL_OPTS=/c /I"%SRCDIR%\include" /I"%BUILD64%\include" /I"%SRCDIR%\tdishr" /MP ^
 /Fp"MdsObjectsCppShr-VS.pch" /diagnostics:classic /Fd"\tmp\vc141.pdb" ^
 /Zc:wchar_t /Zc:inline /fp:precise /Zc:forScope ^
 /D _WINDOWS /D _USRDLL /D _WINDLL /D MDSOBJECTSCPPSHRVS_EXPORTS ^
 /W1 /WX- /WL /D _CRT_SECURE_NO_WARNINGS ^
 /O1 /GS /Gm- /Gw /Gd /MD /EHsc /nologo

cl %CL_OPTS% %CPP%\mdsdata.c
cl %CL_OPTS% %CPP%\mdsdataobjects.cpp
cl %CL_OPTS% %CPP%\mdseventobjects.cpp
cl %CL_OPTS% %CPP%\mdsipobjects.cpp
cl %CL_OPTS% %CPP%\mdstree.c
cl %CL_OPTS% %CPP%\mdstreeobjects.cpp

REM Link MdsObjectsCppShr-VS.DLL

link /OUT:"%BUILD64%\bin_x86_64\MdsObjectsCppShr-VS.dll" /NXCOMPAT /PDB:"MdsObjectsCppShr-VS.pdb" ^
 /DYNAMICBASE "MdsShr.lib" "TreeShr.lib" "TdiShr.lib" "MdsIpShr.lib" "kernel32.lib" "user32.lib" ^
 "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" ^
 "uuid.lib" "odbc32.lib" "odbccp32.lib" /IMPLIB:"%BUILD64%\bin_x86_64\MdsObjectsCppShr-VS.lib" /DLL /MACHINE:X64 ^
 /OPT:REF /INCREMENTAL:NO  /SUBSYSTEM:WINDOWS /MANIFEST /MANIFESTUAC:"level='asInvoker' uiAccess='false'" ^
 /MANIFEST:embed /OPT:ICF /ERRORREPORT:PROMPT /NOLOGO ^
 /LIBPATH:"%BUILD64%\bin_x86_64" /TLBID:1 ^
 mdsdata.obj mdsdataobjects.obj mdseventobjects.obj mdsipobjects.obj mdstree.obj mdstreeobjects.obj

CD %OLDDIR%
