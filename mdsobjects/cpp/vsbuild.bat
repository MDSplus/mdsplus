@ECHO OFF

set topdir=..\..
set VS=z:%1
set arch=%2
set outdir=%3
set INCLUDE=.\;%VS%\include;%topdir%\include;%topdir%\tdishr
set LIB=%VS%\%arch%\lib;%outdir%
set PATH=%PATH%;%VS%\%arch%\bin
CALL :build_dll
goto :eof

:build_dll
SETLOCAL
set options=/O2 /Ob1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" ^
/D "MDSOBJECTS_EXPORTS" /D "_VC80_UPGRADE=0x0600" /D "_WINDLL" /D "_MBCS" ^
/GF /EHsc /MT /Gy /W3 /nologo /c
del *.obj 
cl.exe %options% /TP mdsdataobjects.cpp mdstreeobjects.cpp mdseventobjects.cpp mdsipobjects.cpp
if errorlevel 1 exit %errorlevel%
cl.exe %options% /TC mdsdata.c mdstree.c
if errorlevel 1 exit %errorlevel%
lib /MACHINE:%arch% /DEF:%outdir%/MdsShr.def
lib /MACHINE:%arch% /DEF:%outdir%/TdiShr.def
lib /MACHINE:%arch% /DEF:%outdir%/TreeShr.def
lib /MACHINE:%arch% /DEF:%outdir%/RtEventsShr.def
link.exe /OUT:"%outdir%\MdsObjectsCppShr-VS.dll" /INCREMENTAL:NO ^
/DLL /MANIFEST /MANIFESTFILE:"mdsobjects.dll.intermediate.manifest" /MANIFESTUAC:"level='asInvoker' uiAccess='false'" ^
/PDB:"./mdsobjects.pdb" /DYNAMICBASE:NO /IMPLIB:"%outdir%\MdsObjectsCppShr-VS.lib" ^
/MACHINE:%arch% *.obj /LIBPATH:"%outdir%" MdsShr.lib TdiShr.lib TreeShr.lib MdsIpShr.lib RtEventsShr.lib ^
Kernel32.Lib  libcmt.lib  libcpmt.lib  oldnames.lib  Uuid.Lib 
if errorlevel 1 exit %errorlevel%
del *.obj *.lib
ENDLOCAL
goto :eof






