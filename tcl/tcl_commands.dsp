# Microsoft Developer Studio Project File - Name="tcl_commands" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=tcl_commands - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "tcl_commands.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tcl_commands.mak" CFG="tcl_commands - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tcl_commands - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "tcl_commands - Win32 Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tcl_commands - Win32 Release"

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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../Release/tcl_commands.dll"

!ELSEIF  "$(CFG)" == "tcl_commands - Win32 Debug"

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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../Debug/tcl_commands.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "tcl_commands - Win32 Release"
# Name "tcl_commands - Win32 Debug"
# Begin Source File

SOURCE=.\mds_dsc_string.c
# End Source File
# Begin Source File

SOURCE=.\tcl_add_node.c
# End Source File
# Begin Source File

SOURCE=.\tcl_add_tag.c
# End Source File
# Begin Source File

SOURCE=.\tcl_clean_datafile.c
# End Source File
# Begin Source File

SOURCE=.\tcl_close.c
# End Source File
# Begin Source File

SOURCE=.\tcl_commands.c
# End Source File
# Begin Source File

SOURCE=.\tcl_commands.def
# End Source File
# Begin Source File

SOURCE=.\tcl_commands.ocld

!IF  "$(CFG)" == "tcl_commands - Win32 Release"

# Begin Custom Build
InputPath=.\tcl_commands.ocld

"tcl_commands.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	SET PATH=..\Release 
	cdu < "$(InputPath)" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "tcl_commands - Win32 Debug"

# Begin Custom Build
InputPath=.\tcl_commands.ocld

"tcl_commands.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	SET PATH=..\Debug 
	cdu < "$(InputPath)" 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\tcl_compress_datafile.c
# End Source File
# Begin Source File

SOURCE=.\tcl_create_pulse.c
# End Source File
# Begin Source File

SOURCE=.\tcl_decompile.c
# End Source File
# Begin Source File

SOURCE=.\tcl_delete_node.c
# End Source File
# Begin Source File

SOURCE=.\tcl_delete_pulse.c
# End Source File
# Begin Source File

SOURCE=.\tcl_directory.c
# End Source File
# Begin Source File

SOURCE=.\tcl_directory_tag.c
# End Source File
# Begin Source File

SOURCE=.\tcl_dispatch.c
# End Source File
# Begin Source File

SOURCE=.\tcl_do_method.c
# End Source File
# Begin Source File

SOURCE=.\tcl_do_node.c
# End Source File
# Begin Source File

SOURCE=.\tcl_dummy.c
# End Source File
# Begin Source File

SOURCE=.\tcl_edit.c
# End Source File
# Begin Source File

SOURCE=.\tcl_remove_tag.c
# End Source File
# Begin Source File

SOURCE=.\tcl_rename.c
# End Source File
# Begin Source File

SOURCE=.\tcl_set_callbacks.c
# End Source File
# Begin Source File

SOURCE=.\tcl_set_current.c
# End Source File
# Begin Source File

SOURCE=.\tcl_set_default.c
# End Source File
# Begin Source File

SOURCE=.\tcl_set_node.c
# End Source File
# Begin Source File

SOURCE=.\tcl_set_tree.c
# End Source File
# Begin Source File

SOURCE=.\tcl_show_current.c
# End Source File
# Begin Source File

SOURCE=.\tcl_show_data.c
# End Source File
# Begin Source File

SOURCE=.\tcl_show_db.c
# End Source File
# Begin Source File

SOURCE=.\tcl_verify_tree.c
# End Source File
# Begin Source File

SOURCE=.\tcl_write.c
# End Source File
# End Target
# End Project
