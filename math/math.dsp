# Microsoft Developer Studio Project File - Name="math" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=math - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "math.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "math.mak" CFG="math - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "math - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "math - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "math - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../Release/MdsMath.dll"

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../Debug/MdsMath.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "math - Win32 Release"
# Name "math - Win32 Debug"
# Begin Source File

SOURCE=.\boxsmooth.c
# End Source File
# Begin Source File

SOURCE=.\caxpy.f

!IF  "$(CFG)" == "math - Win32 Release"

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

# Begin Custom Build - Compiling
InputPath=.\caxpy.f
InputName=caxpy

"$(InputName).o" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	set path=%path%;%g77_dir%\bin 
	g77 -c $(InputName).f 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\cgeco.f
# End Source File
# Begin Source File

SOURCE=.\cmul.f
# End Source File
# Begin Source File

SOURCE=.\csint.F
# End Source File
# Begin Source File

SOURCE=.\csint.f.in
# End Source File
# Begin Source File

SOURCE=.\csint.F.old
# End Source File
# Begin Source File

SOURCE=.\csval.f
# End Source File
# Begin Source File

SOURCE=.\daxpy.f
# End Source File
# Begin Source File

SOURCE=.\dcsint.f
# End Source File
# Begin Source File

SOURCE=.\dcsval.f
# End Source File
# Begin Source File

SOURCE=.\dgeco.f
# End Source File
# Begin Source File

SOURCE=.\dmul.f
# End Source File
# Begin Source File

SOURCE=.\dpermu.f
# End Source File
# Begin Source File

SOURCE=.\dppval.f
# End Source File
# Begin Source File

SOURCE=.\dsrch.f
# End Source File
# Begin Source File

SOURCE=.\dsvrgn.f
# End Source File
# Begin Source File

SOURCE=.\gn.f
# End Source File
# Begin Source File

SOURCE=.\isrch.f
# End Source File
# Begin Source File

SOURCE=.\MdsMath.def
# End Source File
# Begin Source File

SOURCE=.\permu.f
# End Source File
# Begin Source File

SOURCE=.\ppval.f
# End Source File
# Begin Source File

SOURCE=.\saxpy.f
# End Source File
# Begin Source File

SOURCE=.\sgeco.f
# End Source File
# Begin Source File

SOURCE=.\smul.f
# End Source File
# Begin Source File

SOURCE=.\srch.f
# End Source File
# Begin Source File

SOURCE=.\ssrch.f
# End Source File
# Begin Source File

SOURCE=.\svign.f
# End Source File
# Begin Source File

SOURCE=.\svrgn.f
# End Source File
# Begin Source File

SOURCE=.\umach.F
# End Source File
# Begin Source File

SOURCE=.\umach.f.in
# End Source File
# Begin Source File

SOURCE=.\umach.F.old
# End Source File
# Begin Source File

SOURCE=.\zaxpy.f
# End Source File
# Begin Source File

SOURCE=.\zgeco.f
# End Source File
# Begin Source File

SOURCE=.\zmul.f
# End Source File
# End Target
# End Project
