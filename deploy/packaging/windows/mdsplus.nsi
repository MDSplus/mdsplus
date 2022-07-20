Name   "MDSplus${BNAME} ${MAJOR}.${MINOR}-${RELEASE}"
OutFile ${OUTDIR}/MDSplus${BNAME}-${MAJOR}.${MINOR}-${RELEASE}.exe
;SetCompress off
SetCompressor /FINAL LZMA
ShowInstDetails show
InstType "Typical"
InstType "Full"
InstType "Minimal"


!define HELPURL "mailto:mdsplus@psfc.mit.edu"	# "Support Information" link
!define UPDATEURL "http://www.mdsplus.org"	# "Product Updates" link
!define ABOUTURL "http://www.mdsplus.org"	# "Publisher" link
!define INSTALLSOURCE "https://github.com/MDSplus/mdsplus/archive/${BRANCH}_release-${MAJOR}.${MINOR}-${RELEASE}.zip"
!define INSTALLSIZE 145000			# Install data + safty
!define MINGWLIB64 /usr/x86_64-w64-mingw32/sys-root/mingw/bin
!define MINGWLIB32 /usr/i686-w64-mingw32/sys-root/mingw/bin
!define PTHREADLIB libwinpthread-1.dll
!define TERMCAPLIB libtermcap-0.dll
!define DLLIB libdl.dll
!define READLINELIB libreadline8.dll
!define GCC_STDCPP_LIB libstdc++-6.dll
!define GCC_S_SEH_LIB libgcc_s_seh-1.dll
!define GCC_S_DW2_LIB libgcc_s_dw2-1.dll
!define GFORTRAN_LIB libgfortran-5.dll
!define QUADMATH_LIB libquadmath-0.dll
!define LIBXML2_LIB libxml2-2.dll
!define ICONV_LIB iconv.dll
!define ZLIB1_LIB zlib1.dll
!define UNINSTALL_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus"

!define TEMP_DEL_DIR "$WINDIR\Temp\MDSplus_uninstall_relicts.delete_me"

!define MDSPLUS_DIR		"$INSTDIR"
!define MDS_PATH		"$INSTDIR\tdi"
!define MDS_PYDEVICE_PATH	"$INSTDIR\pydevices"
!define PYTHONPATH		"$INSTDIR\python"
!define main_path		"$INSTDIR\trees"
!define subtree_path		"$INSTDIR\trees\subtree"

;;allows admins to decide if the want to install for current or allusers
!define MULTIUSER_EXECUTIONLEVEL Highest
!define MULTIUSER_MUI
!define MULTIUSER_INSTALLMODE_INSTDIR MDSplus
!define MULTIUSER_INSTALLMODE_COMMANDLINE
!define MULTIUSER_INSTALLMODE_FUNCTION   Init
!include MultiUser.nsh

;; tools for If and Loop logics
!include LogicLib.nsh

;; 64bit detection and handling
!include x64.nsh

;; string manipulation; defines ${UnStrTrimNewLines}
!include StrFunc.nsh
${UnStrTrimNewLines}  ; implements ${UnStrTrimNewLines}

;; used to get filename in ignore branch of uninstaller
!include "WordFunc.nsh"

!addincludedir ${INCLUDE}
!include debug.nsh		; define debug tools
!include recycle.nsh		; define Recycle, Delete, and TryRecycle
!include in_operator.nsh	; define in
!include is_operator.nsh	; define is
!include registry.nsh		; define AllUsers, registry and env add/remove
!include getbindir.nsh		; define GetBinDir
!include filelowerext.nsh	; FileLowerExt

;; MUI2 include and definitions determine the design of the installer
!include "MUI2.nsh"
!define MUI_ABORTWARNING
!define MUI_COMPONENTSPAGE_SMALLDESC
!define MUI_ICON			icons/mdsplus.ico
!define MUI_UnICON			icons/mdsplus.ico
!define MUI_STARTMENUPAGE_DEFAULTFOLDER	"MDSplus"
!define MUI_FINISHPAGE_LINK		"mdsplus.org"
!define MUI_FINISHPAGE_LINK_LOCATION	${ABOUTURL}
Var StartMenuFolder
!insertmacro MUI_PAGE_LICENSE "MDSplus-License.rtf"
!insertmacro MULTIUSER_PAGE_INSTALLMODE
;; !insertmacro MUI_PAGE_DIRECTORY
;; replace with warning if selected folder already exists
Function DirectoryLeave
${If} ${FileExists} "$InstDir"
	MessageBox MB_YESNO `"$InstDir" already exists.$\r$\nDo you want to continue and DELETE all it's content?` IDYES yes
	Abort
	yes:
${EndIf}
FunctionEnd
Page Directory "" "" DirectoryLeave
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_STARTMENU "Application" $StartMenuFolder
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH
!insertmacro MUI_LANGUAGE "English"



;; tools to manipulate component selection used e.g. in .onSelChange
!include Sections.nsh
!define SelectSection    '!insertmacro "SelectSection"'
!define UnselectSection  '!insertmacro "UnselectSection"'
!define ClearSectionFlag '!insertmacro "ClearSectionFlag"'
!define SetSectionFlag   '!insertmacro "SetSectionFlag"'



### BEGIN InstallFiles ###
!macro InstallFiles source dest logfile
	!define _id_ ${__LINE__}
	Push $0
	Push $1
	FindFirst $0 $1 "${source}\*"
	ClearErrors
	loop${_id_}:
		StrCmp $1 ""   done${_id_}
		StrCmp $1 "."  next${_id_}
		StrCmp $1 ".." next${_id_}
		retry${_id_}:
		Rename "${source}\$1" "${dest}\$1"
		IfErrors 0 ignore${_id_}
		MessageBox MB_ABORTRETRYIGNORE 'File "$1" cannot be moved to "${dest}". File already exists?!' IDIGNORE ignore${_id_} IDRETRY retry${_id_}
		FindClose $0
		FileClose ${logfile}
		Abort
		ignore${_id_}:
		FileWrite ${logfile} "${dest}\$1$\r$\n"
		next${_id_}:
		FindNext $0 $1
		Goto loop${_id_}
	done${_id_}:
	FindClose $0
	Pop $1
	Pop $0
	!undef _id_
!macroend ; InstallFiles
!define InstallFiles '!insertmacro "InstallFiles"'
### END InstallFiles ###



### BEGIN SECTIONS ###
Function install_core_pre
	${TryRecycle} "$INSTDIR"
	SetOutPath "$INSTDIR"
	writeUninstaller "$INSTDIR\uninstall.exe"
	File icons/mdsplus.ico
;	File "/oname=ChangeLog.rtf" ChangeLog
	File MDSplus-License.rtf

	# Registry information for add/remove programs
	${WriteKeyStr} "${UNINSTALL_KEY}" "DisplayName" "MDSplus${BNAME}"
	${WriteKeyStr} "${UNINSTALL_KEY}" "UninstallString" '"$INSTDIR\uninstall.exe" /$MultiUser.InstallMode'
	${WriteKeyStr} "${UNINSTALL_KEY}" "QuietUninstallString" '"$INSTDIR\uninstall.exe" /S /$MultiUser.InstallMode'
	${WriteKeyStr} "${UNINSTALL_KEY}" "Uninstaller"	"$INSTDIR\uninstall.exe"
	${WriteKeyStr} "${UNINSTALL_KEY}" "InstallMode"	"$MultiUser.InstallMode"
	${WriteKeyStr} "${UNINSTALL_KEY}" "InstallLocation" "$INSTDIR"
	${WriteKeyStr} "${UNINSTALL_KEY}" "DisplayIcon"	"$INSTDIR\mdsplus.ico"
	${WriteKeyStr} "${UNINSTALL_KEY}" "Publisher" "MDSplus Collaboratory"
	${WriteKeyStr} "${UNINSTALL_KEY}" "HelpLink" "${HELPURL}"
	${WriteKeyStr} "${UNINSTALL_KEY}" "InstallSource" "${INSTALLSOURCE}"
	${WriteKeyStr} "${UNINSTALL_KEY}" "URLUpdateInfo" "${UPDATEURL}"
	${WriteKeyStr} "${UNINSTALL_KEY}" "URLInfoAbout" "${ABOUTURL}"
	${WriteKeyStr} "${UNINSTALL_KEY}" "DisplayVersion" "${MAJOR}.${MINOR}.${RELEASE}"
	${WriteKeyDWORD} "${UNINSTALL_KEY}" "VersionMajor" ${MAJOR}
	${WriteKeyDWORD} "${UNINSTALL_KEY}" "VersionMinor" ${MINOR}
	# There is no option for modifying or repairing the install
	${WriteKeyDWORD} "${UNINSTALL_KEY}" "NoModify" 1
	${WriteKeyDWORD} "${UNINSTALL_KEY}" "NoRepair" 1
	# Set the INSTALLSIZE constant (!defined at the top of this script) so Add/Remove Programs can accurately report the size
	${WriteKeyDWORD} "${UNINSTALL_KEY}" "EstimatedSize" ${INSTALLSIZE}

	SetOutPath "$INSTDIR\tdi"
	File /r /x local /x MitDevices /x RfxDevices /x KbsiDevices /x d3d tdi/*
	SetOutPath "$INSTDIR\xml"
	File /r xml\*

	SetOutPath "\"
	SetOverWrite off
	File etc\mdsip.hosts etc\multi.hosts
	SetOverWrite on
FunctionEnd

Function install_core_post
	Push $R0
	${AddToEnv} MDS_PATH	"${MDS_PATH}"
	${WriteEnv} MDSPLUS_DIR	"${MDSPLUS_DIR}"
	!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
	CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
	${If} for AllUsers ?
		CreateDirectory "$SMPROGRAMS\$StartMenuFolder\DataServer"
		CreateShortCut  "$SMPROGRAMS\$StartMenuFolder\DataServer\Install mdsip action server on port 8100.lnk" "$SYSDIR\mdsip_service.exe" "-i -s -p 8100 -h $\"C:\multi.hosts$\""
		CreateShortCut  "$SMPROGRAMS\$StartMenuFolder\DataServer\Install mdsip data server on port 8000.lnk" "$SYSDIR\mdsip_service.exe" "-i -p 8000 -h $\"C:\mdsip.hosts$\""
		CreateShortCut  "$SMPROGRAMS\$StartMenuFolder\DataServer\Remove mdsip server on port 8100.lnk" "$SYSDIR\mdsip_service.exe" "-r -p 8100"
		CreateShortCut  "$SMPROGRAMS\$StartMenuFolder\DataServer\Remove mdsip server on port 8000.lnk" "$SYSDIR\mdsip_service.exe" "-r -p 8000"
	${EndIf}
	${GetBinDir} $R0
	CreateShortCut  "$SMPROGRAMS\$StartMenuFolder\ActLog.lnk" "$R0\actlog.exe"  "" "$R0\actlog.exe"  0
	CreateShortCut  "$SMPROGRAMS\$StartMenuFolder\TDI.lnk"    "$R0\tditest.exe" "" "$R0\tditest.exe" 0
	CreateShortCut  "$SMPROGRAMS\$StartMenuFolder\TCL.lnk"    "$R0\mdsdcl.exe" `-prep "set command tcl_commands -history=.tcl"` "$R0\tcl_commands.dll" 0
;	CreateShortCut  "$SMPROGRAMS\$StartMenuFolder\View ChangeLog.lnk" "$INSTDIR\ChangeLog.rtf"
	!insertmacro MUI_STARTMENU_WRITE_END
	Pop $R0
FunctionEnd

!define binexclude "/x *.a /x *.lib /x build_test.exe /x mdsiptest.exe"
SectionGroup "!core" core
 Section "64 bit" bin64
	Push $R0
	Push $R1
	Push $R2
	SectionIn RO
	Call install_core_pre
	SetOutPath "${BINDIR64}"
	File ${binexclude} bin_x86_64/*
	File ${MINGWLIB64}/${PTHREADLIB}
	File ${MINGWLIB64}/${DLLIB}
	File ${MINGWLIB64}/${READLINELIB}
	File ${MINGWLIB64}/${TERMCAPLIB}
	File ${MINGWLIB64}/${GCC_STDCPP_LIB}
	File ${MINGWLIB64}/${GCC_S_SEH_LIB}
	File ${MINGWLIB64}/${GFORTRAN_LIB}
	File ${MINGWLIB64}/${QUADMATH_LIB}
	File ${MINGWLIB64}/${LIBXML2_LIB}
	File ${MINGWLIB64}/${ICONV_LIB}
	File ${MINGWLIB64}/${ZLIB1_LIB}
	${DisableX64FSRedirection}
	${If} for AllUsers ?
		FileOpen $R0 "$INSTDIR\uninstall.dat" w
		${InstallFiles} "${BINDIR64}" "$SYSDIR" $R0
		FileClose $R0
		SetOutPath "$INSTDIR"  ; avoid access to ${BINDIR64} so it may be deleted
		RMDir "${BINDIR64}"
	${EndIf}
	${EnableX64FSRedirection}
        Call install_core_post
	Pop $R2
	Pop $R1
	Pop $R0
 SectionEnd ; 64 bit

 Section "32 bit" bin32
	Push $R0
	Push $R1
	Push $R2
	Push $R3
	${IfNot} ${RunningX64}
	        Call install_core_pre
	${EndIf}
	SetOutPath "${BINDIR32}"
	File ${binexclude} bin_x86/*
	File ${MINGWLIB32}/${PTHREADLIB}
	File ${MINGWLIB32}/${DLLIB}
	File ${MINGWLIB32}/${READLINELIB}
	File ${MINGWLIB32}/${TERMCAPLIB}
	File ${MINGWLIB32}/${GCC_STDCPP_LIB}
	File ${MINGWLIB32}/${GCC_S_DW2_LIB}
	File ${MINGWLIB32}/${GFORTRAN_LIB}
	File ${MINGWLIB32}/${QUADMATH_LIB}
	File ${MINGWLIB32}/${LIBXML2_LIB}
	File ${MINGWLIB32}/${ICONV_LIB}
	File ${MINGWLIB32}/${ZLIB1_LIB}
	${IF} for AllUsers ?
		${If} ${RunningX64}
			FileOpen $R0 "$INSTDIR\uninstall.dat" a
			FileSeek $R0 0 END
			${InstallFiles} "${BINDIR32}" "$WINDIR\SysWOW64" $R0
		${Else}
			FileOpen $R0 "$INSTDIR\uninstall.dat" w
			${InstallFiles} "${BINDIR32}" "$SYSDIR" $R0
		${EndIf}
		FileClose $R0
		SetOutPath "$INSTDIR"	; avoid access to ${BINDIR32} so it may be deleted
		RMDir "${BINDIR32}"
	${EndIf}
	${IfNot} ${RunningX64}
		Call install_core_post
	${EndIf}
	Pop $R3
	Pop $R2
	Pop $R1
	Pop $R0
 SectionEnd ; 32 bit
 Section "add to PATH" appendpath
	Push $R0
	${IfNot} for AllUsers ?
		${GetBinDir} $R0
		${AddToEnv} "PATH" "$R0"
	${EndIf}
	Pop $R0
 SectionEnd
SectionGroupEnd ; core

SectionGroup devices devices
 SectionGroup tdi tdidevices
  Section KBSI
	SectionIn 2
	SetOutPath "$INSTDIR\tdi"
	File /r tdi/KbsiDevices
  SectionEnd ; KBSI
  Section MIT
	SectionIn 2
	SetOutPath "$INSTDIR\tdi"
	File /r tdi/MitDevices
  SectionEnd ; MIT
  Section RFX
	SectionIn 2
	SetOutPath "$INSTDIR\tdi"
	File /r tdi/RfxDevices
  SectionEnd ; RFX
 SectionGroupEnd ; tdi
 SectionGroup pydevices pydevices
  Section HTS pydevices_hts
	SectionIn 2
	SetOutPath "$INSTDIR\pydevices\HtsDevices"
	File /r pydevices/HtsDevices/*
  SectionEnd ; HTS
  Section MIT pydevices_mit
	SectionIn 2
	SetOutPath "$INSTDIR\pydevices\MitDevices"
	File /r pydevices/MitDevices/*
	File /workspace/releasebld/64/pydevices/MitDevices/_version.py
  SectionEnd ; MIT
  Section RFX pydevices_rfx
	SectionIn 2
	SetOutPath "$INSTDIR\pydevices\RfxDevices"
	File /r pydevices/RfxDevices/*
	File /workspace/releasebld/64/pydevices/RfxDevices/_version.py
  SectionEnd ; RFX
  Section W7X pydevices_w7x
	SectionIn 2
	SetOutPath "$INSTDIR\pydevices\W7xDevices"
	File /r pydevices/W7xDevices/*
	File /workspace/releasebld/64/pydevices/W7xDevices/_version.py
  SectionEnd ; W7X
  Section "setup MDS_PYDEVICE_PATH" pydevpath
	SectionIn 2 RO
	${AddToEnv} "MDS_PYDEVICE_PATH" "${MDS_PYDEVICE_PATH}"
  SectionEnd ; pydevpath
 SectionGroupEnd ; pydevices
SectionGroupEnd ; devices

SectionGroup "site specifics"
 Section D3D d3d
	SectionIn 2
	SetOutPath "$INSTDIR\tdi"
	File /r tdi/d3d
 SectionEnd ; D3D
SectionGroupEnd ; site specifics"

Section "java tools" java
	SectionIn 1 2
	SetOutPath $INSTDIR\java
	File /r java/classes
	File icons/jscope.ico
	File icons/jtraverser.ico
	File icons/jtraverser2.ico
	Push $R0
	${GetBinDir} $R0
	!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\jScope.lnk"      "$R0\jScope.bat"		""	"$INSTDIR\java\jscope.ico"	0 SW_SHOWMINIMIZED
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\jTraverser.lnk"  "$R0\jTraverser.bat"	""	"$INSTDIR\java\jtraverser.ico"	0 SW_SHOWMINIMIZED
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\jTraverser2.lnk" "$R0\jTraverser2.bat"	""	"$INSTDIR\java\jtraverser2.ico"	0 SW_SHOWMINIMIZED
	!insertmacro MUI_STARTMENU_WRITE_END
	Pop $R0
SectionEnd ; java

SectionGroup /e "!APIs" apis
 Section EPICS epics
	SectionIn 2
	SetOutPath "$INSTDIR\epics"
	File /r epics/*
 SectionEnd ; EPICS
 Section IDL idl
	SectionIn 2
	SetOutPath "$INSTDIR\idl"
	File /r idl/*
 SectionEnd ; IDL
 SectionGroup LabView LabView
  Section "LV2017 (17.0)" LV2017
	SectionIn 2
	SetOutPath "$INSTDIR\LabView"
	File /r LabView/MDSplus
  SectionEnd ; LV2017
  !define LVOLD_DESC "https://github.com/MDSplus/mdsplus/$\r$\n"
  Section "LV2015 (15.0) on GitHub" LV2015
	!define LV2015_DESC "mdsobjects/labview/MDSplus_LV2015"
;	SetOutPath "$INSTDIR\LabView\LV2015\MDSplus"
;	File /r LabView/MDSplus_LV2015/*
  SectionEnd ; LV2015
  Section "LV2012 (12.0) on GitHub" LV2012
	!define LV2012_DESC "mdsobjects/labview/MDSplus_LV2012"
;	SetOutPath "$INSTDIR\LabView\LV2012\MDSplus"
;	File /r LabView/MDSplus_LV2012/*
  SectionEnd ; LV2012
  Section "LV2001 (<=6.1) on GitHub" LV2001
	!define LV2001_DESC "LabView"
;	SetOutPath "$INSTDIR\LabView\LV2001\MDSplus"
;	File LabView/*.vi
  SectionEnd ; LV2000
 SectionGroupEnd ; LabView
 Section MATLAB MATLAB
	SectionIn 2
	SetOutPath "$INSTDIR\matlab"
	File /r matlab/*
 SectionEnd ; MATLAB
 SectionGroup /e "!python" python
  Section "!MDSplus package" python_cp
	SectionIn 1 2
	SetOutPath "$INSTDIR\python\MDSplus"
	File /x modpython.py /x setup.py python/MDSplus/*.py
	File /workspace/releasebld/64/python/MDSplus/_version.py
  SectionEnd ; python_cp
  Section "tests" python_tst
	SectionIn 2
	SetOutPath "$INSTDIR\python\MDSplus"
	File /r python/MDSplus/tests
  SectionEnd ; python_tst
  Section "WSGI" python_wsgi
	SectionIn 2
	SetOutPath "$INSTDIR\python\MDSplus"
	File /r python/MDSplus/wsgi
  SectionEnd ; python_wsgi
  Section "glade widgets" python_wdg
	SectionIn 2
	SetOutPath "$INSTDIR\python\MDSplus"
	File /r python/MDSplus/widgets
  SectionEnd ; python_wdg
  Section "mod_python" python_mod
	SectionIn 2
	SetOutPath "$INSTDIR\python\MDSplus"
	File python/MDSplus/modpython.py
  SectionEnd ; python_mod
  Section "add to PYTHONPATH" python_pp
	SectionIn 1 2
	${AddToEnv} "PYTHONPATH" "${PYTHONPATH}"
  SectionEnd ; python_pp
  Section "run 'python setup.py install'" python_su
	SetOutPath "$INSTDIR\python\MDSplus"
	File python/MDSplus/setup.py
	nsExec::Exec /OEM /TIMEOUT=10000 "python setup.py install"
	Exch $R0
	Pop  $R0
  SectionEnd ; python_su
  Section "Compile python" python_comp
	SectionIn 1 2
        nsExec::Exec /OEM /TIMEOUT=10000 'python -m compileall "$INSTDIR\python\MDSplus"'
	Exch $R0
	Pop  $R0
  SectionEnd ; python_comp
 SectionGroupEnd ; python
SectionGroupEnd ; APIs

SectionGroup development devel
 Section headers headers
	SectionIn 2
	SetOutPath "$INSTDIR\include"
	File /r include/*
 SectionEnd ; headers
 SectionGroup devtools devtools
  Section MinGW mingw
	SectionIn 2
	${If} ${RunningX64}
		${FileLowerExt} bin_x86_64 .dll.a "$INSTDIR\devtools\mingw\lib64" .lib
	${EndIf}
	${FileLowerExt} bin_x86 .dll.a "$INSTDIR\devtools\mingw\lib32" .lib
  SectionEnd ; MinGW
  Section "Visual Studio" vs
	SectionIn 2
	${If} ${RunningX64}
		${FileLowerExt} bin_x86_64 .lib "$INSTDIR\devtools\visual_studio\lib64" .lib
	${EndIf}
	${FileLowerExt} bin_x86 .lib "$INSTDIR\devtools\visual_studio\lib32" .lib
  SectionEnd ; Visual Studio
 SectionGroupEnd ; devtools
SectionGroupEnd

Section "sample trees"
	SectionIn 2
	${AddToEnv} "main_path"		"${main_path}"
	${AddToEnv} "subtree_path"	"${subtree_path}"
	SetOutPath "$INSTDIR\trees"
	File /r trees/*
SectionEnd

Section "change log"
	SectionIn 2
	SetOutPath "$INSTDIR"
	File ChangeLog
SectionEnd
### END SECTIONS ###



### BEGIN SECTION DESCRIPTION
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${core}	"Install binaries and kernel tdi functions."
	!insertmacro MUI_DESCRIPTION_TEXT ${bin64}	"Copy binaries for 64bit architecture."
	!insertmacro MUI_DESCRIPTION_TEXT ${bin32}	"Copy binaries for 32bit architecture."
	!insertmacro MUI_DESCRIPTION_TEXT ${appendpath}	"Add '.\bin*' to user's PATH env."
	!insertmacro MUI_DESCRIPTION_TEXT ${devices}	"Copy device classes implemented in tdi or as pydevices"
	!insertmacro MUI_DESCRIPTION_TEXT ${tdidevices}	"Copy device classes implemented in tdi to '.\tdi'"
	!insertmacro MUI_DESCRIPTION_TEXT ${pydevices}	"Copy python device classes to '.\pydevices'"
	!insertmacro MUI_DESCRIPTION_TEXT ${pydevpath}	"Add '.\pydevices' to MDS_PYDEVICE_PATH env."
	!insertmacro MUI_DESCRIPTION_TEXT ${d3d}	"Copy site-specific tdi routines to '.\tdi\d3d'"
	!insertmacro MUI_DESCRIPTION_TEXT ${java}	"Copy jTraverser, jTraverser2, jScope, etc. to '.\java\classes'"
	!insertmacro MUI_DESCRIPTION_TEXT ${epics}	"Copy EPICS Plugin to './epics'"
	!insertmacro MUI_DESCRIPTION_TEXT ${idl}	"Copy IDL Plugin to './idl'"
	!insertmacro MUI_DESCRIPTION_TEXT ${LabView}	"Older versions of LabView are available on github.com"
	!insertmacro MUI_DESCRIPTION_TEXT ${LV2017}	"Copy LabView plugin to '.\LabView'; manually copy content to the 'vi.lib' folder."
	!insertmacro MUI_DESCRIPTION_TEXT ${LV2015}	"${LVOLD_DESC}${LV2015_DESC}"
	!insertmacro MUI_DESCRIPTION_TEXT ${LV2012}	"${LVOLD_DESC}${LV2012_DESC}"
	!insertmacro MUI_DESCRIPTION_TEXT ${LV2001}	"${LVOLD_DESC}${LV2001_DESC}"
	!insertmacro MUI_DESCRIPTION_TEXT ${MATLAB}	"Copy MATLAB plugin to '.\matlab'"
	!insertmacro MUI_DESCRIPTION_TEXT ${python}	"Setup the python package"
	!insertmacro MUI_DESCRIPTION_TEXT ${python_cp}	"Copy MDSplus package to '.\python'"
	!insertmacro MUI_DESCRIPTION_TEXT ${python_tst}	"Copy MDSplus test suite to '.\python\MDSplus\tests'"
	!insertmacro MUI_DESCRIPTION_TEXT ${python_wsgi} "Copy WSGI server support to '.\python\MDSplus[\wsgi]'"
	!insertmacro MUI_DESCRIPTION_TEXT ${python_wdg}	"Copy glade widgets for device setups to '.\python\MDSplus\widgets'"
	!insertmacro MUI_DESCRIPTION_TEXT ${python_mod}	"Copy mod_python module for apache to '.\python\MDSplus'"
	!insertmacro MUI_DESCRIPTION_TEXT ${python_pp}	"Add '.\python' to PYTHONPATH env."
	!insertmacro MUI_DESCRIPTION_TEXT ${python_su}	"Install MDSplus package via python's setup method"
	!insertmacro MUI_DESCRIPTION_TEXT ${python_comp} "Will attempt to compile python files for default python version"
	!insertmacro MUI_DESCRIPTION_TEXT ${devel}	"Copy headers and '*.lib' files for MinGW and VS projects"
	!insertmacro MUI_DESCRIPTION_TEXT ${headers}	"Copy headers to '.\include'"
	!insertmacro MUI_DESCRIPTION_TEXT ${devtools}	"Copy '*.lib' files for MinGW and VS projects to '.\devtools'"
	!insertmacro MUI_DESCRIPTION_TEXT ${mingw}	"Copy '*.lib' files for MinGW projects to '.\devtools\mingw'"
	!insertmacro MUI_DESCRIPTION_TEXT ${vs}		"Copy '*.lib' files for VS projects to '.\devtools\visual_studio'"
!insertmacro MUI_FUNCTION_DESCRIPTION_END
### END SECTION DESCRIPTION ###



### BEGIN SECTION LOGIC ###
Function .onSelChange
	; pydevices depend on python
	${If}   $0 == ${apis}
	${OrIf} $0 == ${python}
	${OrIf} $0 == ${python_cp}
		${IfNot}    $0 is ${SF_SELECTED}
		${AndIfNot} $0 is ${SF_PSELECTED}
			${UnselectSection}  ${pydevices}
			${UnselectSection}  ${python_mod}
			${UnselectSection}  ${python_pp}
			${UnselectSection}  ${python_su}
			${UnselectSection}  ${python_comp}
			${UnselectSection}  ${python_tst}
			${UnselectSection}  ${python_wdg}
			${UnselectSection}  ${python_wsgi}
		${EndIf}
	${ElseIf} $0 == ${devices}
	${OrIf}	  $0 == ${pydevices}
		${If}   $0 is ${SF_SELECTED}
		${OrIf} $0 is ${SF_PSELECTED}
			${ClearSectionFlag} ${pydevpath} ${SF_RO}
			${SelectSection}    ${pydevpath}
			${SelectSection}    ${python_cp}
		${Else}
			Goto lock_pydevpath
		${EndIf}
	${ElseIf} $0 == ${pydevices_hts}
	${OrIf}   $0 == ${pydevices_mit}
	${OrIf}   $0 == ${pydevices_rfx}
	${OrIf}   $0 == ${pydevices_w7x}
		${IfNot}    $R0 is ${SF_SELECTED}
		${AndIfNot} ${pydevices_hts} is ${SF_SELECTED}
		${AndIfNot} ${pydevices_mit} is ${SF_SELECTED}
		${AndIfNot} ${pydevices_rfx} is ${SF_SELECTED}
		${AndIfNot} ${pydevices_w7x} is ${SF_SELECTED}
			lock_pydevpath:
			${UnselectSection}  ${pydevpath}
			${SetSectionFlag}   ${pydevpath} ${SF_RO}
		${Else} ;unlock_pydevpath:
			${ClearSectionFlag} ${pydevpath} ${SF_RO}
			${SelectSection}    ${python_cp}
		${EndIf}
	${ElseIf} $0 == ${python_mod}
	${OrIf}   $0 == ${python_pp}
		${If} $0 is ${SF_SELECTED}
			${SelectSection}    ${python_cp}
		${EndIf}
	${ElseIf} $0 == ${python_su}
	${OrIf}   $0 == ${python_comp}
	${OrIf}   $0 == ${python_tst}
	${OrIf}   $0 == ${python_wdg}
	${OrIf}   $0 == ${python_wsgi}
	${If} $0 is ${SF_SELECTED}
			${SelectSection}    ${python_cp}
		${EndIf}
	${EndIf}
FunctionEnd
### END SECTION LOGIC ###



### BEGIN INSTALLER ###
Function Init
	${ToLog} "BEGIN INIT"
	Push $R0
	${ReadkeyStr} $R0 ${UNINSTALL_KEY} "UninstallString"
	${IfNot}  $R0 == ""
		${ReadkeyStr} $INSTDIR ${UNINSTALL_KEY} "InstallLocation"
		MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION \
		"MDSplus is already installed. $\r$\n$\r$\nClick `OK` to remove the \
		previous version or `Cancel` to cancel this upgrade." IDOK uninst
			Abort
		; Run the uninstaller
		uninst:
		; The uninstaller would create a temp copy before executing
		; This ensures the the original file can be uninstalled
		; however this brease the wait functionality of execwait
		; This is why we create a tempfile manually
		Push $R1
		Push $R2
		${ReadkeyStr} $R2 ${UNINSTALL_KEY} "Uninstaller"
		${If} "$R2" == ""
			ClearErrors
			ExecWait '$R0 _?=$INSTDIR' ; _? prevents temp file creation
		${Else}
			System::Call 'ole32::CoCreateGuid(g .R1)'
			StrCpy $R1 `$TEMP\mdsplus_$R1_uninstall.exe`
			CopyFiles `$R2` `$R1`
			${ReadkeyStr} $R2 ${UNINSTALL_KEY} "InstallMode"
			ClearErrors
			ExecWait '"$R1" /$R2 _?=$INSTDIR' ; _? prevents temp file creation
			Delete `$R1`
		${EndIf}
		Pop $R2
		Pop $R1
	${Else}
		${ReadEnv} $INSTDIR MDSPLUS_DIR
	${EndIf}
	Pop $R0
	${If} ${RunningX64}
		SectionSetInstTypes ${bin32} 2 ; include 32bit in full
		SectionSetInstTypes ${bin64} 7 ; always include 64bit
	${Else}
		${SetSectionFlag}   ${bin32} ${SF_RO}
		${SelectSection}    ${bin32}
		SectionSetInstTypes ${bin32} 7 ; always include 32bit
		SectionSetInstTypes ${bin64} 0 ; never include 64bit
	${EndIf}
	${If} for AllUsers ?
		${If} `$INSTDIR` == ""
			${If} ${RunningX64}
				StrCpy $INSTDIR $PROGRAMFILES64\MDSplus
			${Else}
				StrCpy $INSTDIR $PROGRAMFILES32\MDSplus
			${EndIf}
		${EndIf}
		SectionSetFlags ${appendpath} ${SF_RO}
	${Else}
		${If} $INSTDIR == ""
			StrCpy $INSTDIR $PROFILE\MDSplus
		${EndIf}
		SectionSetInstTypes ${appendpath} 7 ; always include on user level
		SectionSetFlags ${appendpath} ${SF_SELECTED}
	${EndIf}
	SectionSetFlags ${LV2015} ${SF_RO}
	SectionSetFlags ${LV2012} ${SF_RO}
	SectionSetFlags ${LV2001} ${SF_RO}
	${ToLog} "END INIT"
FunctionEnd ; Init
Function .onInit
	${ToLog} "BEGIN .onInit"
	!insertmacro MULTIUSER_INIT
	${ToLog} "END .onInit"
FunctionEnd ; .onInit

Function .onGUIEnd
	SendMessage ${HWND_BROADCAST} ${WM_SETTINGCHANGE} 0 "STR:Environment" /TIMEOUT=5000
	${ShowLog}
FunctionEnd
### END INSTALLER ###



### BEGIN UNINSTALLER ###
Function un.onInit
	!insertmacro MULTIUSER_UNINIT
	${ReadkeyStr} $INSTDIR ${UNINSTALL_KEY} "InstallLocation"
	${If} "$INSTDIR" == ""
		Abort
	${EndIf}
functionEnd ; un.onInit

Function un.onGUIEnd
	SendMessage ${HWND_BROADCAST} ${WM_SETTINGCHANGE} 0 "STR:Environment" /TIMEOUT=5000
FunctionEnd

Section uninstall
	Push $R0
	Push $R1
	Push $R2
	IfFileExists "$INSTDIR\python\MDSplus\setup.py" 0 skip_python_remove
	SetOutPath "$INSTDIR\python\MDSplus"
	nsExec::ExecToLog /OEM /TIMEOUT=10000 'python setup.py remove'
	Pop $R0
	skip_python_remove:
	SetOutPath "$INSTDIR"
	${IF} for AllUsers ?
		ReadEnvStr $R1 COMSPEC
		nsExec::ExecToLog '"$R1" /C NET STOP "MDSplus 8000"'
		Pop $R0
		nsExec::ExecToLog '"$R1" /C SC DELETE "MDSplus 8000"'
		Pop $R0
		nsExec::ExecToLog '"$R1" /C NET STOP "MDSplus 8100"'
		Pop $R0
		nsExec::ExecToLog '"$R1" /C SC DELETE "MDSplus 8100"'
		Pop $R0
		FileOpen $R0 "$INSTDIR\uninstall.dat" r
		${DisableX64FSRedirection}
		CreateDirectory "${TEMP_DEL_DIR}"
		ClearErrors
		loop:
			FileRead $R0 $R1
			StrCmp $R1 "" done
			${UnStrTrimNewLines} $R2 $R1
			retry:
			Delete "$R2"
			IfErrors 0 loop
			MessageBox MB_ABORTRETRYIGNORE 'File "$R2" could not be deleted. Is it still in use.' IDIGNORE ignore IDRETRY retry
			FileClose $R0
			RmDir /r /REBOOTOK "${TEMP_DEL_DIR}"
			Abort
			ignore:
			System::Call 'kernel32::GetTickCount()i .R3'
			${WordFind} "$R2" "\" "-1" $R4
			Rename "$R2" "${TEMP_DEL_DIR}\$R4$R3"
			ClearErrors
			Goto loop
		done:
		FileClose $R0
		${EnableX64FSRedirection}
		RmDir /r /REBOOTOK "${TEMP_DEL_DIR}"
		Delete uninstall.dat
	${Else}
		${GetBinDir} $R0
		${RemoveFromEnv}	PATH		"$R0"
	${EndIf}
	!insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
	RMDir /r "$SMPROGRAMS\$StartMenuFolder"
	${DeleteEnv}		MDSPLUS_DIR
	${RemoveFromEnv}	MDS_PATH		"${MDS_PATH}"
	${RemoveFromEnv}	main_path		"${main_path}"
	${RemoveFromEnv}	subtree_path		"${subtree_path}"
	${RemoveFromEnv}	PYTHONPATH		"${PYTHONPATH}"
	${RemoveFromEnv}        MDS_PYDEVICE_PATH	"${MDS_PYDEVICE_PATH}"
	${DeleteKey} "${UNINSTALL_KEY}"
	SetOutPath "$SYSDIR" ; avoid access to $INSTDIR so it may be deleted
	${TryRecycle} "$INSTDIR"
	Pop $R2
	Pop $R1
	Pop $R0
	${ShowLog}
SectionEnd
### END UNINSTALLER ###
