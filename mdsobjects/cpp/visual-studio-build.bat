@ECHO off

REM Obtain SRCDIR from location of this script

set OLDDIR=%CD%
cd %~dp0..\..
set SRCDIR=%CD%
cd %OLDDIR%

REM A subset of the Visual Studio binaries, libs and headers
REM \Microsoft-Visual-Studio directory is from the C:\Program Files (x86)\Microsoft Visual Studio\ directory on
REM a Windows system with the free Visual Studio 2017 Community edition installed.
REM \Windows-Kits is from the C:\Program Files (x64)\Windows Kits\ directory.
REM The \vc directory is obtained from the following directories when Visual Studio 2017 Community is
REM installed on a Windows system:

SET MVS=\Microsoft-Visual-Studio\2017\Community\VC\Tools\MSVC\14.12.25827
SET WK=\Windows-Kits\10
set PATH=%MVS%\bin\HostX64\x64;%WK%\bin\10.0.16299.0\x64
set INCLUDE=%MVS%\include;%WK%\include\10.0.16299.0\shared;%WK%\include\10.0.16299.0\ucrt;%WK%\include\10.0.16299.0\um;
set LIB=%MVS%\lib\x64;%WK%\Lib\10.0.16299.0\um\x64

REM Various directories used for compiles, links and lib generation

set CPP=%SRCDIR%\mdsobjects\cpp
set BUILD64=\workspace\releasebld\64
set BUILD32=\workspace\releasebld\32

REM Build Visual Studio compatible lib files for the C based libraries (both 64 and 32 bit versions)

lib /def:%SRCDIR%\mdsshr\mdsshr.def /out:%BUILD64%\bin_x86_64\MdsShr.lib /machine:x64
lib /def:%SRCDIR%\treeshr\treeshr.def /out:%BUILD64%\bin_x86_64\TreeShr.lib /machine:x64
lib /def:%SRCDIR%\tdishr\tdishr.def /out:%BUILD64%\bin_x86_64\TdiShr.lib /machine:x64
lib /def:%SRCDIR%\mdstcpip\mdsipshr.def /out:%BUILD64%\bin_x86_64\MdsIpShr.lib /machine:x64
lib /def:%SRCDIR%\mdsmisc\mdsmisc.def /out:%BUILD64%\bin_x86_64\MdsMisc.lib /machine:x64
lib /def:%SRCDIR%\servershr\servershr.def /out:%BUILD64%\bin_x86_64\MdsServerShr.lib /machine:x64
lib /def:%SRCDIR%\mdslib\MdsLib.def /out:%BUILD64%\bin_x86_64\MdsLib.lib /machine:x64
lib /def:%SRCDIR%\mdsdcl\mdsdclshr.def /out:%BUILD64%\bin_x86_64\MdsDcl.lib /machine:x64
lib /def:%SRCDIR%\math\MdsMath.def /out:%BUILD64%\bin_x86_64\MdsMath.lib /machine:x64

lib /def:%SRCDIR%\mdsshr\mdsshr.def /out:%BUILD32%\bin_x86\MdsShr.lib /machine:x86
lib /def:%SRCDIR%\treeshr\treeshr.def /out:%BUILD32%\bin_x86\TreeShr.lib /machine:x86
lib /def:%SRCDIR%\tdishr\tdishr.def /out:%BUILD32%\bin_x86\TdiShr.lib /machine:x86
lib /def:%SRCDIR%\mdstcpip\mdsipshr.def /out:%BUILD32%\bin_x86\MdsIpShr.lib /machine:x86
lib /def:%SRCDIR%\mdsmisc\mdsmisc.def /out:%BUILD32%\bin_x86\MdsMisc.lib /machine:x86
lib /def:%SRCDIR%\servershr\servershr.def /out:%BUILD32%\bin_x86\MdsServerShr.lib /machine:x86
lib /def:%SRCDIR%\mdslib\MdsLib.def /out:%BUILD32%\bin_x86\MdsLib.lib /machine:x86
lib /def:%SRCDIR%\mdsdcl\mdsdclshr.def /out:%BUILD32%\bin_x86\MdsDcl.lib /machine:x86
lib /def:%SRCDIR%\math\MdsMath.def /out:%BUILD32%\bin_x86\MdsMath.lib /machine:x86

REM Use Visual studio compiler to compile the CPP modules

set CL_OPTS=/c /GS /W3 /Zc:wchar_t /I"%SRCDIR%\include" /I"%SRCDIR%\tdishr" /Zi /Gm- /O2 ^
 /Fd"\tmp\vc141.pdb" /Zc:inline /fp:precise /D "WIN32" /D "NDEBUG" /D "WINDOWS_H" ^
 /D "_WINDOWS" /D "_USRDLL" /D "MDSOBJECTSCPPSHRVS_EXPORTS" /D "NOMINMAX" ^
 /D "_CRT_SECURE_NO_WARNINGS" /D "_WINDLL" /errorReport:prompt /WX- /Zc:forScope ^
 /Gd /MD /EHsc /nologo  /Fp"MdsObjectsCppShr-VS.pch" /diagnostics:classic

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
