# Microsoft Developer Studio Project File - Name="javadispatcher" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=javadispatcher - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "javadispatcher.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "javadispatcher.mak" CFG="javadispatcher - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "javadispatcher - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "javadispatcher - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "javadispatcher - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f javadispatcher.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "javadispatcher.exe"
# PROP BASE Bsc_Name "javadispatcher.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "nmake /f "javadispatcher.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "javadispatcher.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "javadispatcher - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f javadispatcher.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "javadispatcher.exe"
# PROP BASE Bsc_Name "javadispatcher.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "nmake /f "javadispatcher.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "javadispatcher.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "javadispatcher - Win32 Release"
# Name "javadispatcher - Win32 Debug"

!IF  "$(CFG)" == "javadispatcher - Win32 Release"

!ELSEIF  "$(CFG)" == "javadispatcher - Win32 Debug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Action.java
# End Source File
# Begin Source File

SOURCE=.\ActionServer.java
# End Source File
# Begin Source File

SOURCE=.\Balancer.java
# End Source File
# Begin Source File

SOURCE=.\Descriptor.java
# End Source File
# Begin Source File

SOURCE=.\InfoServer.java
# End Source File
# Begin Source File

SOURCE=.\jDispatcher.java
# End Source File
# Begin Source File

SOURCE=.\jDispatcherIp.java
# End Source File
# Begin Source File

SOURCE=.\jDispatchMonitor.java
# End Source File
# Begin Source File

SOURCE=.\MdsHelper.java
# End Source File
# Begin Source File

SOURCE=.\MdsIp.java
# End Source File
# Begin Source File

SOURCE=.\MdsMonitor.java
# End Source File
# Begin Source File

SOURCE=.\MdsMonitorEvent.java
# End Source File
# Begin Source File

SOURCE=.\MdsServer.java
# End Source File
# Begin Source File

SOURCE=.\MdsServerEvent.java
# End Source File
# Begin Source File

SOURCE=.\MdsServerListener.java
# End Source File
# Begin Source File

SOURCE=.\MonitorEvent.java
# End Source File
# Begin Source File

SOURCE=.\MonitorListener.java
# End Source File
# Begin Source File

SOURCE=.\NetworkEvent.java
# End Source File
# Begin Source File

SOURCE=.\NetworkListener.java
# End Source File
# Begin Source File

SOURCE=.\Server.java
# End Source File
# Begin Source File

SOURCE=.\ServerEvent.java
# End Source File
# Begin Source File

SOURCE=.\ServerListener.java
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Makefile"

# PROP Default_Filter "mak"
# Begin Source File

SOURCE=.\javadispatcher.mak
# End Source File
# End Group
# End Target
# End Project
