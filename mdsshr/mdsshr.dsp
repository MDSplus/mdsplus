# Microsoft Developer Studio Project File - Name="mdsshr" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=mdsshr - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mdsshr.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mdsshr.mak" CFG="mdsshr - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mdsshr - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mdsshr - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mdsshr - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../include" /I "../ipcs/include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 wsock32.lib ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /profile /machine:I386 /out:"../Release/mdsshr.dll"

!ELSEIF  "$(CFG)" == "mdsshr - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../include" /D "_WIN32" /D "HAVE_WINDOWS_H" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wsock32.lib ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"../Debug/mdsshr.dll" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "mdsshr - Win32 Release"
# Name "mdsshr - Win32 Debug"
# Begin Source File

SOURCE=..\include\config.h_win32

!IF  "$(CFG)" == "mdsshr - Win32 Release"

# Begin Custom Build
InputPath=..\include\config.h_win32

"..\include\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\include\config.h_win32 ..\include\config.h

# End Custom Build

!ELSEIF  "$(CFG)" == "mdsshr - Win32 Debug"

# Begin Custom Build - Creating config.h
InputPath=..\include\config.h_win32

"..\include\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\include\config.h_win32 ..\include\config.h

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dasutil_routines4.c
# End Source File
# Begin Source File

SOURCE=.\librtl.c
# End Source File
# Begin Source File

SOURCE=.\mds_dsc_string.c
# End Source File
# Begin Source File

SOURCE=.\MdsCmprs.c
# End Source File
# Begin Source File

SOURCE=.\MdsCompareXd.c
# End Source File
# Begin Source File

SOURCE=.\MdsCompress.c
# End Source File
# Begin Source File

SOURCE=.\MdsEvents.c
# End Source File
# Begin Source File

SOURCE=.\MdsGet1DxA.c
# End Source File
# Begin Source File

SOURCE=.\MdsGet1DxS.c
# End Source File
# Begin Source File

SOURCE=.\MdsGetSetShotId.c
# End Source File
# Begin Source File

SOURCE=.\mdsmsg.c
# End Source File
# Begin Source File

SOURCE=.\MdsPk.c
# End Source File
# Begin Source File

SOURCE=.\MDSprintf.c
# End Source File
# Begin Source File

SOURCE=.\MdsSerialize.c
# End Source File
# Begin Source File

SOURCE=.\MdsShr.def
# End Source File
# Begin Source File

SOURCE=.\MdsShrThreadSafe.c
# End Source File
# Begin Source File

SOURCE=.\MdsXdRoutines.c
# End Source File
# End Target
# End Project
