# Microsoft Developer Studio Project File - Name="idlsql" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=idlsql - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "idlsql.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "idlsql.mak" CFG="idlsql - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "idlsql - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "idlsql - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "idlsql - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "%SQL_DIR%\DevTools\Include" /I "%IDL_DIR%\external" /I "..\include" /I "$(SQL_DIR)\DevTools\Include" /I "$(IDL_DIR)\external" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "DBNTWIN32" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../Release/idlsql.dll"

!ELSEIF  "$(CFG)" == "idlsql - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "$(SQL_DIR)\DevTools\Include" /I "$(IDL_DIR)\external" /I "..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "DBNTWIN32" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../Debug/idlsql.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "idlsql - Win32 Release"
# Name "idlsql - Win32 Debug"
# Begin Group "idl_pros"

# PROP Default_Filter "*.pro"
# Begin Source File

SOURCE=..\idl\Logbook\bust_string.pro
# End Source File
# Begin Source File

SOURCE=..\idl\Logbook\commit.pro
# End Source File
# Begin Source File

SOURCE=..\idl\Logbook\dbinfo.pro
# End Source File
# Begin Source File

SOURCE=..\idl\Logbook\dbinfo_cmod.pro
# End Source File
# Begin Source File

SOURCE=..\idl\Logbook\dsql.pro
# End Source File
# Begin Source File

SOURCE=..\idl\Logbook\entry_display.pro
# End Source File
# Begin Source File

SOURCE=..\idl\Logbook\get_database.pro
# End Source File
# Begin Source File

SOURCE=..\idl\Logbook\isql.pro
# End Source File
# Begin Source File

SOURCE=.\isql.pro
# End Source File
# Begin Source File

SOURCE=..\idl\Logbook\rollback.pro
# End Source File
# Begin Source File

SOURCE=..\idl\Logbook\set_database.pro
# End Source File
# Begin Source File

SOURCE=..\idl\Logbook\sql.pro
# End Source File
# Begin Source File

SOURCE=..\idl\Logbook\sql_finish.pro
# End Source File
# End Group
# Begin Source File

SOURCE=.\dynamic_syb.c
# End Source File
# Begin Source File

SOURCE="$(IDL_DIR)\bin\bin.x86\idl32.lib"
# End Source File
# Begin Source File

SOURCE=.\IdlSql.def
# End Source File
# Begin Source File

SOURCE=.\idlsql_syb.c
# End Source File
# Begin Source File

SOURCE="$(SQL_DIR)\DevTools\Lib\ntwdblib.lib"
# End Source File
# Begin Source File

SOURCE=.\user_syb.c
# End Source File
# End Target
# End Project
