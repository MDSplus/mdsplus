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
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libg2c.a libgcc.a /nologo /subsystem:windows /dll /machine:I386 /out:"../Release/MdsMath.dll"
# SUBTRACT LINK32 /pdb:none

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libg2c.a libgcc.a kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../Debug/MdsMath.dll" /pdbtype:sept /libpath:"e:\g77\lib"

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

# Begin Custom Build - Compiling
IntDir=.\Release
InputPath=.\caxpy.f
InputName=caxpy

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

# Begin Custom Build - Compiling
IntDir=.\Debug
InputPath=.\caxpy.f
InputName=caxpy

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c -g $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\cgeco.f

!IF  "$(CFG)" == "math - Win32 Release"

# Begin Custom Build - Compiling
IntDir=.\Release
InputPath=.\cgeco.f
InputName=cgeco

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

# Begin Custom Build - Compiling
IntDir=.\Debug
InputPath=.\cgeco.f
InputName=cgeco

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c -g $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\cmul.f

!IF  "$(CFG)" == "math - Win32 Release"

# Begin Custom Build - Compiling
IntDir=.\Release
InputPath=.\cmul.f
InputName=cmul

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

# Begin Custom Build - Compiling
IntDir=.\Debug
InputPath=.\cmul.f
InputName=cmul

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c -g $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\csint-win32.f"

!IF  "$(CFG)" == "math - Win32 Release"

# Begin Custom Build - Compiling
IntDir=.\Release
InputPath=.\csint-win32.f
InputName=csint-win32

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

# Begin Custom Build - Compiling
IntDir=.\Debug
InputPath=.\csint-win32.f
InputName=csint-win32

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c -g $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\csval.f

!IF  "$(CFG)" == "math - Win32 Release"

# Begin Custom Build - Compiling
IntDir=.\Release
InputPath=.\csval.f
InputName=csval

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

# Begin Custom Build - Compiling
IntDir=.\Debug
InputPath=.\csval.f
InputName=csval

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c -g $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\daxpy.f

!IF  "$(CFG)" == "math - Win32 Release"

# Begin Custom Build - Compiling
IntDir=.\Release
InputPath=.\daxpy.f
InputName=daxpy

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

# Begin Custom Build - Compiling
IntDir=.\Debug
InputPath=.\daxpy.f
InputName=daxpy

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c -g $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dcsint.f

!IF  "$(CFG)" == "math - Win32 Release"

# Begin Custom Build - Compiling
IntDir=.\Release
InputPath=.\dcsint.f
InputName=dcsint

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

# Begin Custom Build - Compiling
IntDir=.\Debug
InputPath=.\dcsint.f
InputName=dcsint

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c -g $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dcsval.f

!IF  "$(CFG)" == "math - Win32 Release"

# Begin Custom Build - Compiling
IntDir=.\Release
InputPath=.\dcsval.f
InputName=dcsval

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

# Begin Custom Build - Compiling
IntDir=.\Debug
InputPath=.\dcsval.f
InputName=dcsval

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c -g $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dgeco.f

!IF  "$(CFG)" == "math - Win32 Release"

# Begin Custom Build - Compiling
IntDir=.\Release
InputPath=.\dgeco.f
InputName=dgeco

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

# Begin Custom Build - Compiling
IntDir=.\Debug
InputPath=.\dgeco.f
InputName=dgeco

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c -g $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dmul.f

!IF  "$(CFG)" == "math - Win32 Release"

# Begin Custom Build - Compiling
IntDir=.\Release
InputPath=.\dmul.f
InputName=dmul

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

# Begin Custom Build - Compiling
IntDir=.\Debug
InputPath=.\dmul.f
InputName=dmul

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c -g $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dpermu.f

!IF  "$(CFG)" == "math - Win32 Release"

# Begin Custom Build - Compiling
IntDir=.\Release
InputPath=.\dpermu.f
InputName=dpermu

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

# Begin Custom Build - Compiling
IntDir=.\Debug
InputPath=.\dpermu.f
InputName=dpermu

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c -g $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dppval.f

!IF  "$(CFG)" == "math - Win32 Release"

# Begin Custom Build - Compiling
IntDir=.\Release
InputPath=.\dppval.f
InputName=dppval

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

# Begin Custom Build - Compiling
IntDir=.\Debug
InputPath=.\dppval.f
InputName=dppval

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c -g $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dsrch.f

!IF  "$(CFG)" == "math - Win32 Release"

# Begin Custom Build - Compiling
IntDir=.\Release
InputPath=.\dsrch.f
InputName=dsrch

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

# Begin Custom Build - Compiling
IntDir=.\Debug
InputPath=.\dsrch.f
InputName=dsrch

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c -g $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dsvrgn.f

!IF  "$(CFG)" == "math - Win32 Release"

# Begin Custom Build - Compiling
IntDir=.\Release
InputPath=.\dsvrgn.f
InputName=dsvrgn

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

# Begin Custom Build - Compiling
IntDir=.\Debug
InputPath=.\dsvrgn.f
InputName=dsvrgn

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c -g $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\gn.f

!IF  "$(CFG)" == "math - Win32 Release"

# Begin Custom Build - Compiling
IntDir=.\Release
InputPath=.\gn.f
InputName=gn

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

# Begin Custom Build - Compiling
IntDir=.\Debug
InputPath=.\gn.f
InputName=gn

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c -g $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\isrch.f

!IF  "$(CFG)" == "math - Win32 Release"

# Begin Custom Build - Compiling
IntDir=.\Release
InputPath=.\isrch.f
InputName=isrch

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

# Begin Custom Build - Compiling
IntDir=.\Debug
InputPath=.\isrch.f
InputName=isrch

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c -g $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\librtl-win32.f"

!IF  "$(CFG)" == "math - Win32 Release"

# Begin Custom Build - Compiling
IntDir=.\Release
InputPath=.\librtl-win32.f
InputName=librtl-win32

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	set path=%path%;%g77_dir%\bin 
	g77 -c $(InputName).f -o $(IntDir)\$(InputName).obj -fno-underscoring\
   -fcase-preserve 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

# Begin Custom Build - Compiling
IntDir=.\Debug
InputPath=.\librtl-win32.f
InputName=librtl-win32

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	set path=%path%;%g77_dir%\bin 
	g77 -c $(InputName).f -o $(IntDir)\$(InputName).obj -fno-underscoring\
   -fcase-preserve 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MdsMath.def
# End Source File
# Begin Source File

SOURCE=.\permu.f

!IF  "$(CFG)" == "math - Win32 Release"

# Begin Custom Build - Compiling
IntDir=.\Release
InputPath=.\permu.f
InputName=permu

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

# Begin Custom Build - Compiling
IntDir=.\Debug
InputPath=.\permu.f
InputName=permu

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c -g $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ppval.f

!IF  "$(CFG)" == "math - Win32 Release"

# Begin Custom Build - Compiling
IntDir=.\Release
InputPath=.\ppval.f
InputName=ppval

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

# Begin Custom Build - Compiling
IntDir=.\Debug
InputPath=.\ppval.f
InputName=ppval

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c -g $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\saxpy.f

!IF  "$(CFG)" == "math - Win32 Release"

# Begin Custom Build - Compiling
IntDir=.\Release
InputPath=.\saxpy.f
InputName=saxpy

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

# Begin Custom Build - Compiling
IntDir=.\Debug
InputPath=.\saxpy.f
InputName=saxpy

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c -g $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sgeco.f

!IF  "$(CFG)" == "math - Win32 Release"

# Begin Custom Build - Compiling
IntDir=.\Release
InputPath=.\sgeco.f
InputName=sgeco

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

# Begin Custom Build - Compiling
IntDir=.\Debug
InputPath=.\sgeco.f
InputName=sgeco

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c -g $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\smul.f

!IF  "$(CFG)" == "math - Win32 Release"

# Begin Custom Build - Compiling
IntDir=.\Release
InputPath=.\smul.f
InputName=smul

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

# Begin Custom Build - Compiling
IntDir=.\Debug
InputPath=.\smul.f
InputName=smul

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c -g $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\srch.f

!IF  "$(CFG)" == "math - Win32 Release"

# Begin Custom Build - Compiling
IntDir=.\Release
InputPath=.\srch.f
InputName=srch

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

# Begin Custom Build - Compiling
IntDir=.\Debug
InputPath=.\srch.f
InputName=srch

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c -g $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ssrch.f

!IF  "$(CFG)" == "math - Win32 Release"

# Begin Custom Build - Compiling
IntDir=.\Release
InputPath=.\ssrch.f
InputName=ssrch

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

# Begin Custom Build - Compiling
IntDir=.\Debug
InputPath=.\ssrch.f
InputName=ssrch

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c -g $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\svign.f

!IF  "$(CFG)" == "math - Win32 Release"

# Begin Custom Build - Compiling
IntDir=.\Release
InputPath=.\svign.f
InputName=svign

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

# Begin Custom Build - Compiling
IntDir=.\Debug
InputPath=.\svign.f
InputName=svign

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c -g $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\svrgn.f

!IF  "$(CFG)" == "math - Win32 Release"

# Begin Custom Build - Compiling
IntDir=.\Release
InputPath=.\svrgn.f
InputName=svrgn

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

# Begin Custom Build - Compiling
IntDir=.\Debug
InputPath=.\svrgn.f
InputName=svrgn

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c -g $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\umach-win32.f"

!IF  "$(CFG)" == "math - Win32 Release"

# Begin Custom Build - Compiling
IntDir=.\Release
InputPath=.\umach-win32.f
InputName=umach-win32

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

# Begin Custom Build - Compiling
IntDir=.\Debug
InputPath=.\umach-win32.f
InputName=umach-win32

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c -g $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\zaxpy.f

!IF  "$(CFG)" == "math - Win32 Release"

# Begin Custom Build - Compiling
IntDir=.\Release
InputPath=.\zaxpy.f
InputName=zaxpy

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

# Begin Custom Build - Compiling
IntDir=.\Debug
InputPath=.\zaxpy.f
InputName=zaxpy

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c -g $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\zgeco.f

!IF  "$(CFG)" == "math - Win32 Release"

# Begin Custom Build - Compiling
IntDir=.\Release
InputPath=.\zgeco.f
InputName=zgeco

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

# Begin Custom Build - Compiling
IntDir=.\Debug
InputPath=.\zgeco.f
InputName=zgeco

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c -g $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\zmul.f

!IF  "$(CFG)" == "math - Win32 Release"

# Begin Custom Build - Compiling
IntDir=.\Release
InputPath=.\zmul.f
InputName=zmul

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

# Begin Custom Build - Compiling
IntDir=.\Debug
InputPath=.\zmul.f
InputName=zmul

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo $(IntDir)\$(InputName).f 
	set path=%path%;%G77_DIR%\bin 
	g77 -c -g $(InputName).f -o $(IntDir)\$(InputName).obj -w 
	
# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
