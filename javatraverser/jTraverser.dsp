# Microsoft Developer Studio Project File - Name="jTraverser" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Java Virtual Machine External Target" 0x0806

CFG=jTraverser - Java Virtual Machine Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "jTraverser.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "jTraverser.mak"\
 CFG="jTraverser - Java Virtual Machine Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "jTraverser - Java Virtual Machine Release" (based on\
 "Java Virtual Machine External Target")
!MESSAGE "jTraverser - Java Virtual Machine Debug" (based on\
 "Java Virtual Machine External Target")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "jTraverser - Java Virtual Machine Release"

# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir ""
# PROP BASE Cmd_Line "NMAKE /f jTraverser.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "jTraverser.exe"
# PROP BASE Bsc_Name "jTraverser.bsc"
# PROP BASE Target_Dir ""
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Cmd_Line "%JDK_DIR%\bin\javac.exe -O -d ..\java\classes *.java"
# PROP Rebuild_Opt ""
# PROP Target_File "..\java\classes\jTraverser\jTraverser.class"
# PROP Bsc_Name "jTraverser.bsc"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "jTraverser - Java Virtual Machine Debug"

# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir ""
# PROP BASE Cmd_Line "NMAKE /f jTraverser.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "jTraverser.exe"
# PROP BASE Bsc_Name "jTraverser.bsc"
# PROP BASE Target_Dir ""
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Cmd_Line "%JDK_DIR%\bin\javac.exe -d ..\java\classes *.java"
# PROP Rebuild_Opt ""
# PROP Target_File "..\java\classes\jTraverser\jTraverser.class"
# PROP Bsc_Name "jTraverser.bsc"
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "jTraverser - Java Virtual Machine Release"
# Name "jTraverser - Java Virtual Machine Debug"

!IF  "$(CFG)" == "jTraverser - Java Virtual Machine Release"

!ELSEIF  "$(CFG)" == "jTraverser - Java Virtual Machine Debug"

!ENDIF 

# End Target
# End Project
