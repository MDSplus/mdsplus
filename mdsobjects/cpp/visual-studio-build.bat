@ECHO on
set OLDDIR=%CD%
cd %~dp0..\..
set SRCDIR=%CD%
cd %OLDDIR%
set PATH=\vc\bin\x64;\vc\winsdk\bin\x64
set INCLUDE=\vc\include;\vc\winsdk\include\shared;\vc\winsdk\include\ucrt;\vc\winsdk\include\um;
set LIB=\vc\lib\x64;\vc\winsdk\lib\x64
set TMP=Z:\tmp
set TEMP=Z:\tmp
set SystemRoot=C:\Windows
set CPP=%SRCDIR%\mdsobjects\cpp
set BUILD64=\workspace\releasebld\64
set BUILD32=\workspace\releasebld\32

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

lib /def:%SRCDIR%\mdsshr\mdsshr.def /out:%BUILD64%\bin_x86_64\MdsShr.lib /machine:x64
dir %BUILD64%\bin_x86_64\*
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

set LINK_OPTS=/OUT:"%BUILD64%\bin_x86_64\MdsObjectsCppShr-VS.dll" /NXCOMPAT /PDB:"MdsObjectsCppShr-VS.pdb" ^
 /DYNAMICBASE "MdsShr.lib" "TreeShr.lib" "TdiShr.lib" "MdsIpShr.lib" "kernel32.lib" "user32.lib" ^
 "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" ^
 "uuid.lib" "odbc32.lib" "odbccp32.lib" /IMPLIB:"%BUILD64%\bin_x86_64\MdsObjectsCppShr-VS.lib" /DLL /MACHINE:X64 ^
 /OPT:REF /INCREMENTAL:NO  /SUBSYSTEM:WINDOWS /MANIFEST /MANIFESTUAC:"level='asInvoker' uiAccess='false'" ^
 /MANIFEST:embed /OPT:ICF /ERRORREPORT:PROMPT /NOLOGO ^
 /LIBPATH:"%BUILD64%\bin_x86_64" /TLBID:1

link %LINK_OPTS% mdsdata.obj mdsdataobjects.obj mdseventobjects.obj mdsipobjects.obj mdstree.obj mdstreeobjects.obj
