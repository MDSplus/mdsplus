@ECHO OFF

set VS=z:\mdsplus\visual-studio-2008
CALL :build_dll x86
CALL :build_dll x86_64
goto :eof

:build_dll
SETLOCAL
set INCLUDE=.\;%VS%\include;..\..\include;..\..\tdishr
set arch=%1
set LIB=%VS%\%arch%\lib
set compiler=%VS%\%arch%\bin\cl.exe
set options=/O2 /Ob1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" ^
/D "MDSOBJECTS_EXPORTS" /D "_VC80_UPGRADE=0x0600" /D "_WINDLL" /D "_MBCS" /GF /EHsc /MT /Gy /Fp"mdsobjects.pch" /W3 /nologo /c
del *.obj *.lib
%compiler% %options% /TP mdsdataobjects.cpp mdstreeobjects.cpp mdseventobjects.cpp mdsipobjects.cpp
if errorlevel 1 exit %errorlevel%
%compiler% %options% /TC mdsdata.c mdstree.c
if errorlevel 1 exit %errorlevel%
copy ..\..\bin_%arch%\MdsShr.dll.a .\MdsShr.lib
copy ..\..\bin_%arch%\TdiShr.dll.a .\TdiShr.lib
copy ..\..\bin_%arch%\TreeShr.dll.a .\TreeShr.lib
copy ..\..\bin_%arch%\MdsIpShr.dll.a .\MdsIpShr.lib
copy ..\..\bin_%arch%\RtEventsShr.dll.a .\RtEventsShr.lib
%compiler% /LD /o ..\..\bin_%arch%\MdsObjectsCppShr-VS.dll *.obj MdsShr.lib TdiShr.lib TreeShr.lib MdsIpShr.lib rteventsshr.lib
if errorlevel 1 exit %errorlevel%
del *.obj *.lib
ENDLOCAL
goto :eof






