# Microsoft Developer Studio Project File - Name="tdishr" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=tdishr - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "tdishr.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tdishr.mak" CFG="tdishr - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tdishr - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "tdishr - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tdishr - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /profile /machine:I386 /out:"../Release/tdishr.dll"

!ELSEIF  "$(CFG)" == "tdishr - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../Debug/tdishr.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "tdishr - Win32 Release"
# Name "tdishr - Win32 Debug"
# Begin Source File

SOURCE=.\CvtConvertFloat.c
# End Source File
# Begin Source File

SOURCE=.\DummyRoutines.c
# End Source File
# Begin Source File

SOURCE=.\IsRoprand.c
# End Source File
# Begin Source File

SOURCE=.\TdiAbs.c
# End Source File
# Begin Source File

SOURCE=.\TdiAdd.c
# End Source File
# Begin Source File

SOURCE=.\TdiAnd.c
# End Source File
# Begin Source File

SOURCE=.\TdiArray.c
# End Source File
# Begin Source File

SOURCE=.\TdiBinary.c
# End Source File
# Begin Source File

SOURCE=.\TdiBound.c
# End Source File
# Begin Source File

SOURCE=.\TdiBuild.c
# End Source File
# Begin Source File

SOURCE=.\TdiCall.c
# End Source File
# Begin Source File

SOURCE=.\TdiChar.c
# End Source File
# Begin Source File

SOURCE=.\TdiCompile.c
# End Source File
# Begin Source File

SOURCE=.\TdiCompileDependency.c
# End Source File
# Begin Source File

SOURCE=.\TdiConstant.c
# End Source File
# Begin Source File

SOURCE=.\TdiConvert.c
# End Source File
# Begin Source File

SOURCE=.\TdiCull.c
# End Source File
# Begin Source File

SOURCE=.\TdiCvtArgs.c
# End Source File
# Begin Source File

SOURCE=.\TdiCvtDxDx.c
# End Source File
# Begin Source File

SOURCE=.\TdiDecompile.c
# End Source File
# Begin Source File

SOURCE=.\TdiDecompileDependency.c
# End Source File
# Begin Source File

SOURCE=.\TdiDecompileR.c
# End Source File
# Begin Source File

SOURCE=.\TdiDecompress.c
# End Source File
# Begin Source File

SOURCE=.\TdiDefCat.c
# End Source File
# Begin Source File

SOURCE=.\TdiDefFunction.c
# End Source File
# Begin Source File

SOURCE=.\TdiDim.c
# End Source File
# Begin Source File

SOURCE=.\TdiDivide.c
# End Source File
# Begin Source File

SOURCE=.\TdiDoTask.c
# End Source File
# Begin Source File

SOURCE=.\TdiDtypeRange.c
# End Source File
# Begin Source File

SOURCE=.\TdiEq.c
# End Source File
# Begin Source File

SOURCE=.\TdiEvaluate.c
# End Source File
# Begin Source File

SOURCE=.\TdiExponent.c
# End Source File
# Begin Source File

SOURCE=.\TdiExpt.c
# End Source File
# Begin Source File

SOURCE=.\TdiExtFunction.c
# End Source File
# Begin Source File

SOURCE=.\TdiFaultHandler.c
# End Source File
# Begin Source File

SOURCE=.\TdiGetArgs.c
# End Source File
# Begin Source File

SOURCE=.\TdiGetData.c
# End Source File
# Begin Source File

SOURCE=.\TdiGetDbi.c
# End Source File
# Begin Source File

SOURCE=.\TdiGetNci.c
# End Source File
# Begin Source File

SOURCE=.\TdiGetShape.c
# End Source File
# Begin Source File

SOURCE=.\TdiGetSlope.c
# End Source File
# Begin Source File

SOURCE=.\TdiHash.c
# End Source File
# Begin Source File

SOURCE=.\TdiIand.c
# End Source File
# Begin Source File

SOURCE=.\TdiIntrinsic.c
# End Source File
# Begin Source File

SOURCE=.\TdiIo.c
# End Source File
# Begin Source File

SOURCE=.\TdiItoX.c
# End Source File
# Begin Source File

SOURCE=.\TdiLex.c
# End Source File
# Begin Source File

SOURCE=.\TdiMakeFunctionTable.c
# End Source File
# Begin Source File

SOURCE=.\TdiMasterData.c
# End Source File
# Begin Source File

SOURCE=.\TdiMath1.c
# End Source File
# Begin Source File

SOURCE=.\TdiMath2.c
# End Source File
# Begin Source File

SOURCE=.\TdiMatrix.c
# End Source File
# Begin Source File

SOURCE=.\TdiMaxVal.c
# End Source File
# Begin Source File

SOURCE=.\TdiMinMax.c
# End Source File
# Begin Source File

SOURCE=.\TdiOpcDef.c
# End Source File
# Begin Source File

SOURCE=.\TdiPack.c
# End Source File
# Begin Source File

SOURCE=.\TdiPower.c
# End Source File
# Begin Source File

SOURCE=.\TdiSame.c
# End Source File
# Begin Source File

SOURCE=.\TdiScalar.c
# End Source File
# Begin Source File

SOURCE=.\TdiSetRange.c
# End Source File
# Begin Source File

SOURCE=.\TdiShowVm.c
# End Source File
# Begin Source File

SOURCE=.\TdiShr.def
# End Source File
# Begin Source File

SOURCE=.\TdiSort.c
# End Source File
# Begin Source File

SOURCE=.\TdiSql.c
# End Source File
# Begin Source File

SOURCE=.\TdiSquare.c
# End Source File
# Begin Source File

SOURCE=.\TdiStatement.c
# End Source File
# Begin Source File

SOURCE=.\TdiSubscript.c
# End Source File
# Begin Source File

SOURCE=.\TdiTrans.c
# End Source File
# Begin Source File

SOURCE=.\TdiTrim.c
# End Source File
# Begin Source File

SOURCE=.\TdiUnary.c
# End Source File
# Begin Source File

SOURCE=.\TdiUndef.c
# End Source File
# Begin Source File

SOURCE=.\TdiVar.c
# End Source File
# Begin Source File

SOURCE=.\TdiVector.c
# End Source File
# Begin Source File

SOURCE=.\TdiXxx.c
# End Source File
# Begin Source File

SOURCE=.\TdiXxxOf.c
# End Source File
# Begin Source File

SOURCE=.\TdiYacc.c
# End Source File
# Begin Source File

SOURCE=.\TdiYaccSubs.c
# End Source File
# End Target
# End Project
