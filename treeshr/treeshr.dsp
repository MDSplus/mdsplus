# Microsoft Developer Studio Project File - Name="treeshr" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=treeshr - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "treeshr.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "treeshr.mak" CFG="treeshr - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "treeshr - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "treeshr - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "treeshr - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /profile /machine:I386 /out:"../Release/treeshr.dll"

!ELSEIF  "$(CFG)" == "treeshr - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../Debug/treeshr.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "treeshr - Win32 Release"
# Name "treeshr - Win32 Debug"
# Begin Source File

SOURCE=.\dummies.c
# End Source File
# Begin Source File

SOURCE=.\RemoteAccess.c
# End Source File
# Begin Source File

SOURCE=.\TreeAddNode.c
# End Source File
# Begin Source File

SOURCE=.\TreeAddTag.c
# End Source File
# Begin Source File

SOURCE=.\TreeCleanDatafile.c
# End Source File
# Begin Source File

SOURCE=.\TreeCreatePulseFile.c
# End Source File
# Begin Source File

SOURCE=.\TreeDeleteNode.c
# End Source File
# Begin Source File

SOURCE=.\TreeDeletePulseFile.c
# End Source File
# Begin Source File

SOURCE=.\TreeDoMethod.c
# End Source File
# Begin Source File

SOURCE=.\TreeFindNode.c
# End Source File
# Begin Source File

SOURCE=.\TreeFindTagWild.c
# End Source File
# Begin Source File

SOURCE=.\TreeGetDbi.c
# End Source File
# Begin Source File

SOURCE=.\TreeGetNci.c
# End Source File
# Begin Source File

SOURCE=.\TreeGetRecord.c
# End Source File
# Begin Source File

SOURCE=.\TreeGetSetShotId.c
# End Source File
# Begin Source File

SOURCE=.\TreeOpen.c
# End Source File
# Begin Source File

SOURCE=.\TreePutRecord.c
# End Source File
# Begin Source File

SOURCE=.\TreeRemoveNodesTags.c
# End Source File
# Begin Source File

SOURCE=.\TreeRenameNode.c
# End Source File
# Begin Source File

SOURCE=.\TreeSerializeNci.c
# End Source File
# Begin Source File

SOURCE=.\TreeSetDefault.c
# End Source File
# Begin Source File

SOURCE=.\TreeSetNci.c
# End Source File
# Begin Source File

SOURCE=.\TreeShr.def
# End Source File
# Begin Source File

SOURCE=.\TreeVerify.c
# End Source File
# End Target
# End Project
