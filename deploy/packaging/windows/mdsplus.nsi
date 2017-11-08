!include LogicLib.nsh
!include x64.nsh
!include StrFunc.nsh
!include WinMessages.nsh

Name "MDSplus${FLAVOR} ${MAJOR}.${MINOR}-${RELEASE}"
Icon mdsplus.ico
UninstallIcon mdsplus.ico

InstallDirRegKey HKLM Software\MDSplus${FLAVOR} InstallLocation
OutFile ${OUTDIR}/MDSplus${FLAVOR}-${MAJOR}.${MINOR}-${RELEASE}.exe
RequestExecutionLevel admin 
!define HELPURL "mailto:mdsplus@psfc.mit.edu" # "Support Information" link
!define UPDATEURL "http://www.mdsplus.org" # "Product Updates" link
!define ABOUTURL "http://www.mdsplus.org" # "Publisher" link
!define INSTALLSIZE 90000
!define ENVREG "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"
!define MINGWLIB64 /usr/x86_64-w64-mingw32/sys-root/mingw/bin
!define MINGWLIB32 /usr/i686-w64-mingw32/sys-root/mingw/bin
!define PTHREADLIB libwinpthread-1.dll
!define TERMCAPLIB libtermcap-0.dll
!define DLLIB libdl.dll
!define READLINELIB libreadline6.dll
!define GCC_S_SJLJ_LIB libgcc_s_sjlj-1.dll
!define GCC_STDCPP_LIB libstdc++-6.dll
!define GCC_S_SW2_LIB libgcc-s_dw2-1.dll
!define GCC_S_SEH_LIB libgcc_s_seh-1.dll
!define GFORTRAN_LIB libgfortran-3.dll
!define QUADMATH_LIB libquadmath-0.dll
!define LIBXML2_LIB libxml2-2.dll
!define ICONV_LIB iconv.dll
!define ZLIB1_LIB zlib1.dll
LicenseData "MDSplus-License.rtf"
Page license
Page directory
Page components /ENABLECANCEL
Page instfiles
 
!macro VerifyUserIsAdmin
UserInfo::GetAccountType
pop $0
${If} $0 != "admin" ;Require admin rights on NT4+
        messageBox mb_iconstop "Administrator rights required!"
        setErrorLevel 740 ;ERROR_ELEVATION_REQUIRED
        quit
${EndIf}
!macroend
${UnStrTrimNewLines} 
function .onInit
	setShellVarContext all
	!insertmacro VerifyUserIsAdmin
	${If} ${RunningX64}
      	      StrCpy $INSTDIR $PROGRAMFILES64\MDSplus
	${Else}
	      StrCpy $INSTDIR $PROGRAMFILES32\MDSplus
	${EndIf}
  	ReadRegStr $R0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "UninstallString"
  	StrCmp $R0 "" done
 
	MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION \
  	"MDSplus is already installed. $\n$\nClick `OK` to remove the \
  	previous version or `Cancel` to cancel this upgrade." \
  	IDOK uninst
  	Abort
 
;Run the uninstaller
uninst:
	ClearErrors
  	ExecWait '$R0 _?=$INSTDIR' ;Do not copy the uninstaller to a temp file
 done:
functionEnd

function .onGUIEnd
  SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
functionEnd
 
Section
SetOutPath "$INSTDIR"
SetShellVarContext all
File "/oname=ChangeLog.rtf" ChangeLog
File ${srcdir}/mdsplus.ico
File ${srcdir}/MDSplus-License.rtf
writeUninstaller "$INSTDIR\uninstall.exe"
WriteRegStr HKLM "${ENVREG}" MDS_PATH "$INSTDIR\tdi"
WriteRegStr HKLM "${ENVREG}" MDSPLUS_DIR "$INSTDIR"
CreateDirectory "$SMPROGRAMS\MDSplus${FLAVOR}"
CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\Tdi.lnk" "$SYSDIR\tditest.exe" "" "$SYSDIR\icons.exe" 0
CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\TCL.lnk" '"$SYSDIR\cmd.exe"' '/c "mdstcl"' "$SYSDIR\icons.exe" 1
CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\View ChangeLog.lnk" "$INSTDIR\ChangeLog.rtf"
CreateDirectory "$SMPROGRAMS\MDSplus${FLAVOR}\DataServer"
CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\DataServer\Install mdsip action server on port 8100.lnk" "$SYSDIR\mdsip_service.exe" "-i -s -p 8100 -h $\"C:\mdsip.hosts$\""
CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\DataServer\Install mdsip data server on port 8000.lnk" "$SYSDIR\mdsip_service.exe" "-i -p 8000 -h $\"C:\mdsip.hosts$\""
CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\DataServer\Remove mdsip server on port 8100.lnk" "$SYSDIR\mdsip_service.exe" "-r -p 8100"
CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\DataServer\Remove mdsip server on port 8000.lnk" "$SYSDIR\mdsip_service.exe" "-r -p 8000"

File /r /x local  tdi xml
FileOpen $0 "$INSTDIR\installer.dat" w
StrCpy $5 "$SYSDIR"
${If} ${RunningX64}
StrCpy $5 "$WINDIR\SysWOW64"
SetOutPath "$INSTDIR\bin_x86_64"
File /x *.a bin_x86_64/*
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
FindFirst $1 $2 "$INSTDIR\bin_x86_64\*"
loop_64:
  StrCmp $2 "" done_64
  FileWrite $0 "$SYSDIR\$2$\n"
  Delete "$SYSDIR\$2"
  Rename "$INSTDIR\bin_x86_64\$2" "$SYSDIR\$2"
  FindNext $1 $2
  Goto loop_64
done_64:
FindClose $1
${EnableX64FSRedirection}
SetOutPath "$INSTDIR"
RMDir "$INSTDIR\bin_x86_64"
${EndIf}
SetOutPath "$INSTDIR\bin_x86"
File /x *.a bin_x86/*
File ${MINGWLIB32}/${PTHREADLIB}
File ${MINGWLIB32}/${GCC_S_SJLJ_LIB}
File ${MINGWLIB32}/${DLLIB}
File ${MINGWLIB32}/${READLINELIB}
File ${MINGWLIB32}/${TERMCAPLIB}
File ${MINGWLIB32}/${GCC_STDCPP_LIB}
File ${MINGWLIB32}/${GFORTRAN_LIB}
File ${MINGWLIB32}/${QUADMATH_LIB}
File ${MINGWLIB32}/${LIBXML2_LIB}
File ${MINGWLIB32}/${ICONV_LIB}
File ${MINGWLIB32}/${ZLIB1_LIB}
FindFirst $1 $2 "$INSTDIR\bin_x86\*"
loop_32:
  StrCmp $2 "" done_32
  FileWrite $0 "$5\$2$\n"
  Delete "$5\$2"
  Rename "$INSTDIR\bin_x86\$2" "$5\$2"
  FindNext $1 $2
  Goto loop_32
done_32:
FindClose $1
FileClose $0
SetOutPath "$INSTDIR"
RMDir "$INSTDIR\bin_x86"
SetOutPath "\"
SetOverWrite off
File etc\mdsip.hosts
SetOverWrite on

# Registry information for add/remove programs
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "DisplayName" "MDSplus${FLAVOR}"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "UninstallString" "$INSTDIR\uninstall.exe"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "QuietUninstallString" "$INSTDIR\uninstall.exe /S"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "InstallLocation" "$INSTDIR"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "DisplayIcon" "INSTDIR\mdsplus.ico"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "Publisher" "MDSplus Collaboratory"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "HelpLink" "${HELPURL}"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "InstallSource" "http://www.mdsplus.org/dist${SRCDIR}S/mdsplus${FLAVOR}-${MAJOR}.${MINOR}-${RELEASE}.tgz"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "URLUpdateInfo" "${UPDATEURL}"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "URLInfoAbout" "${ABOUTURL}"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "DisplayVersion" "${MAJOR}.${MINOR}.${RELEASE}"
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "VersionMajor" ${MAJOR}
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "VersionMinor" ${MINOR}
# There is no option for modifying or repairing the install
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "NoModify" 1
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "NoRepair" 1
# Set the INSTALLSIZE constant (!defined at the top of this script) so Add/Remove Programs can accurately report the size
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "EstimatedSize" ${INSTALLSIZE}
SectionEnd

Section "Sample Trees"
SetOutPath "$INSTDIR"
WriteRegStr HKLM "${ENVREG}" "main_path" "$INSTDIR\trees"
WriteRegStr HKLM "${ENVREG}" "subtree_path" "$INSTDIR\trees\subtree"
File /r trees
SectionEnd

Section "IDL"
SetOutPath "$INSTDIR"
File /r idl
SectionEnd

Section "JAVA"
SetOutPath $INSTDIR
SetShellVarContext all
File /r /x desktop java
CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\Scope.lnk" '"$SYSDIR\cmd.exe"' '/c "jScope.bat"' "$SYSDIR\icons.exe" 4 SW_SHOWMINIMIZED
CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\Traverser.lnk" '"$SYSDIR\cmd.exe"' '/c "traverser.bat"' $SYSDIR\icons.exe" 3 SW_SHOWMINIMIZED
SectionEnd

Section LabView
SetOutPath "$INSTDIR\LabView"
File LabView/*.vi
SetOutPath "$INSTDIR\mdsobjects\LabView"
File /r LabView/MDSplus
File /r LabView/MDSplus_LV2012
File /r LabView/MDSplus_LV2015
SectionEnd

Section EPICS
SetOutPath "$INSTDIR"
File /r epics
SectionEnd

Section "MATLAB"
SetOutPath "$INSTDIR"
File /r matlab
SectionEnd

Section "PYTHON"
SetOutPath "$INSTDIR\mdsobjects"
File /r /x MDSplus ${srcdir}/mdsobjects/python
SetOutPath "$INSTDIR\mdsobjects\python"
File /workspace/releasebld/64/mdsobjects/python/_version.py
SetOutPath "$INSTDIR\mdsobjects\python"
Exec "python setup.py install" 
SectionEnd

Section "DEVEL"
SetOutPath "$INSTDIR"
File /r include
${If} ${RunningX64}
CreateDirectory "$INSTDIR\devtools\lib64\mingw"
SetOutPath "$INSTDIR\devtools\lib64\mingw"
File "/oname=mdsshr.lib" bin_x86_64/MdsShr.dll.a
File "/oname=treeshr.lib" bin_x86_64/TreeShr.dll.a
File "/oname=tdishr.lib" bin_x86_64/TdiShr.dll.a
File "/oname=mdsdcl.lib" bin_x86_64/MdsDcl.dll.a
File "/oname=mdsipshr.lib" bin_x86_64/MdsIpShr.dll.a
File "/oname=mdslib.lib" bin_x86_64/MdsLib.dll.a
File "/oname=mdsobjectscppshr.lib" bin_x86_64/MdsObjectsCppShr.dll.a
File "/oname=mdsservershr.lib" bin_x86_64/MdsServerShr.dll.a
File "/oname=xtreeshr.lib" bin_x86_64/XTreeShr.dll.a
!ifdef VisualStudio
CreateDirectory "$INSTDIR\devtools\lib64\visual_studio"
SetOutPath "$INSTDIR\devtools\lib64\visual_studio"
File "/oname=mdsshr.lib" bin_x86_64/MdsShr.lib
File "/oname=treeshr.lib" bin_x86_64/TreeShr.lib
File "/oname=tdishr.lib" bin_x86_64/TdiShr.lib
File "/oname=mdsdcl.lib" bin_x86_64/mdsdclshr.lib
File "/oname=mdsipshr.lib" bin_x86_64/mdsipshr.lib
File "/oname=mdslib.lib" bin_x86_64/MdsLib.lib
File "/oname=mdsobjectscppshr.lib" bin_x86_64/MdsObjectsCppShr-VS.lib
File "/oname=mdsservershr.lib" bin_x86_64/servershr.lib
!endif
${EndIf}
CreateDirectory "$INSTDIR\devtools\lib32\mingw"
SetOutPath "$INSTDIR\devtools\lib32\mingw"
File "/oname=mdsshr.lib" bin_x86/MdsShr.dll.a
File "/oname=treeshr.lib" bin_x86/TreeShr.dll.a
File "/oname=tdishr.lib" bin_x86/TdiShr.dll.a
File "/oname=mdsdcl.lib" bin_x86/MdsDcl.dll.a
File "/oname=mdsipshr.lib" bin_x86/MdsIpShr.dll.a
File "/oname=mdslib.lib" bin_x86/MdsLib.dll.a
File "/oname=mdsobjectscppshr.lib" bin_x86/MdsObjectsCppShr.dll.a
File "/oname=mdsservershr.lib" bin_x86/MdsServerShr.dll.a
File "/oname=xtreeshr.lib" bin_x86/XTreeShr.dll.a
!ifdef VisualStudio
CreateDirectory "$INSTDIR\devtools\lib64\visual_studio"
SetOutPath "$INSTDIR\devtools\lib32\visual_studio"
File "/oname=mdsshr.lib" bin_x86/MdsShr.lib
File "/oname=treeshr.lib" bin_x86/TreeShr.lib
File "/oname=tdishr.lib" bin_x86/TdiShr.lib
File "/oname=mdsdcl.lib" bin_x86/mdsdclshr.lib
File "/oname=mdsipshr.lib" bin_x86/mdsipshr.lib
File "/oname=mdslib.lib" bin_x86/MdsLib.lib
File "/oname=mdsobjectscppshr.lib" bin_x86/MdsObjectsCppShr-VS.lib
File "/oname=mdsservershr.lib" bin_x86/servershr.lib
!endif
SectionEnd
 
# Uninstaller
 
function un.onInit
	SetShellVarContext all
 
	#Verify the uninstaller - last chance to back out
	MessageBox MB_OKCANCEL "Permanantly remove MDSplus${FLAVOR}?" IDOK next
		Abort
	next:
	!insertmacro VerifyUserIsAdmin
;	${If} ${RunningX64}
;	  ExecWait '"$INSTDIR\bin_x86_64\WinInstall.exe" /Uninstall' $0
;	${Else}
;	  ExecWait '"$INSTDIR\bin_x86\WinInstall.exe" /Uninstall' $0
;	${EndIf}

functionEnd

function un.onGUIEnd
  SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
functionEnd
 
section "uninstall"
IfFileExists "$INSTDIR\mdsobjects\python" 0 +2
  Exec 'python -c "import MDSplus; MDSplus.remove()"'
SetOutPath "$INSTDIR"
delete ChangeLog.rtf
delete MDSplus-License.rtf
delete mdsplus.ico
delete uninstall.exe
Exec '"$SYSDIR\mdsip_service.exe" "-r -p 8100"'
Exec '"$SYSDIR\mdsip_service.exe" "-r -p 8000"'
RMDir /r "$INSTDIR\tdi"
RMDIR /r "$INSTDIR\xml"
RMDir /r "$INSTDIR\include"
RMDir /r "$INSTDIR\devtools"
RMDir /r "$INSTDIR\trees"
RMDir /r "$INSTDIR\idl"
RMDir /r "$INSTDIR\java"
RMDir /r "$INSTDIR\LabView"
RMDir /r "$INSTDIR\epics"
RMDir /r "$INSTDIR\matlab"
RMDir /r "$INSTDIR\mdsobjects"
RMDir /r "$INSTDIR\devtools"
RMDir /r "$SMPROGRAMS\MDSplus${FLAVOR}"
FileOpen $0 "$INSTDIR\installer.dat" r
${DisableX64FSRedirection}
loop_u:
  FileRead $0 $1
  StrCmp $1 "" done_u
  ${UnStrTrimNewLines} $2 $1
  Delete $2
  Goto loop_u
done_u:
FileClose $0
${EnableX64FSRedirection}
delete installer.dat
SetOutPath "$SYSDIR"
RMDir "$INSTDIR"
# Registry information for add/remove programs
DeleteRegValue HKLM "${ENVREG}" MDS_PATH
DeleteRegValue HKLM "${ENVREG}" MDSPLUS_DIR
DeleteRegValue HKLM "${ENVREG}" main_path
DeleteRegValue HKLM "${ENVREG}" subtree_path
DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus"
SectionEnd
