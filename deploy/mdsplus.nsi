!include LogicLib.nsh
!include x64.nsh

Name "MDSplus${FLAVOR} ${MAJOR}.${MINOR}.${RELEASE}"
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

function .onINstSuccess
	 ${If} ${RunningX64}
	       ExecWait '"$INSTDIR\bin_x86_64\WinInstall.exe" /Install' $0
	 ${Else}
	       ExecWait '"$INSTDIR\bin_x86\WinInstall.exe" /Install' $0
	 ${EndIf}
functionEnd
 
Section
SetOutPath "$INSTDIR"
SetShellVarContext all
File "/oname=ChangeLog.rtf" ChangeLog
File mdsplus.ico
File MDSplus-License.rtf
writeUninstaller "$INSTDIR\uninstall.exe"
WriteRegStr HKLM "${ENVREG}" MDS_PATH "$INSTDIR\tdi"
WriteRegStr HKLM "${ENVREG}" MDSPLUSDIR "$INSTDIR"
CreateDirectory "$SMPROGRAMS\MDSplus${FLAVOR}"
${If} ${RunningX64}
  CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\tditest(64).lnk" "$INSTDIR\bin_x86_64\tditest.exe" "" "$INSTDIR\bin_x86_64\icons.exe" 0
  CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\tditest(32).lnk" "$INSTDIR\bin_x86\tditest.exe" "" "$INSTDIR\bin_x86\icons.exe" 0
  CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\TCL.lnk" '"$SYSDIR\cmd.exe"' '/c "$INSTDIR\bin_x86_64\mdstcl.bat"' "$INSTDIR\bin_x86_64\icons.exe" 1
${Else}
  CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\tditest.lnk" "$INSTDIR\bin_x86\tditest.exe" "" "$INSTDIR\bin_x86\icons.exe" 0
  CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\TCL.lnk" '"$SYSDIR\cmd.exe"' '/c "$INSTDIR\bin_x86\mdstcl.bat"' "$INSTDIR\bin_x86\icons.exe" 1
${EndIf}
CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\View ChangeLog.lnk" "$INSTDIR\ChangeLog.rtf"
CreateDirectory "$SMPROGRAMS\MDSplus${FLAVOR}\DataServer"
${If} ${RunningX64}
  CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\DataServer\Install mdsip action server on port 8100.lnk" "$INSTDIR\bin_x86_64\mdsip_service.exe" "-i -s -p 8100 -h $\"C:\mdsip.hosts$\""
  CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\DataServer\Install mdsip data server on port 8000.lnk" "$INSTDIR\bin_x86_64\mdsip_service.exe" "-i -p 8000 -h $\"C:\mdsip.hosts$\""
  CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\DataServer\Remove mdsip server on port 8100.lnk" "$INSTDIR\bin_x86_64\mdsip_service.exe" "-r -p 8100"
  CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\DataServer\Remove mdsip server on port 8000.lnk" "$INSTDIR\bin_x86_64\mdsip_service.exe" "-r -p 8000"
${Else}
  CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\DataServer\Install mdsip action server on port 8100.lnk" "$INSTDIR\bin_x86\mdsip_service.exe" "-i -s -p 8100 -h $\"C:\mdsip.hosts$\""
  CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\DataServer\Install mdsip data server on port 8000.lnk" "$INSTDIR\bin_x86\mdsip_service.exe" "-i -p 8000 -h $\"C:\mdsip.hosts$\""
  CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\DataServer\Remove mdsip server on port 8100.lnk" "$INSTDIR\bin_x86\mdsip_service.exe" "-r -p 8100"
  CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\DataServer\Remove mdsip server on port 8000.lnk" "$INSTDIR\bin_x86\mdsip_service.exe" "-r -p 8000"
${EndIf}

File /r /x local  tdi
${If} ${RunningX64}
File /r /x *.a bin_x86_64
SetOutPath "$INSTDIR\bin_x86_64"
File ${MINGWLIB64}/${PTHREADLIB}
File ${MINGWLIB64}/${DLLIB}
File ${MINGWLIB64}/${READLINELIB}
File ${MINGWLIB64}/${TERMCAPLIB}
File ${MINGWLIB64}/${GCC_STDCPP_LIB}
${EndIf}
SetOutPath "$INSTDIR"
File /r /x *.a bin_x86
SetOutPath "$INSTDIR\bin_x86"
File ${MINGWLIB32}/${PTHREADLIB}
File ${MINGWLIB32}/${GCC_S_SJLJ_LIB}
File ${MINGWLIB32}/${DLLIB}
File ${MINGWLIB32}/${READLINELIB}
File ${MINGWLIB32}/${TERMCAPLIB}
File ${MINGWLIB32}/${GCC_STDCPP_LIB}
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
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "InstallSource" "http://www.mdsplus.org/dist/SOURCES/mdsplus${FLAVOR}-${MAJOR}.${MINOR}-${RELEASE}.tgz"
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
CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\Scope.lnk" '"$SYSDIR\cmd.exe"' '/c "$INSTDIR\bin_x86\jScope.bat"' "$INSTDIR\bin_x86\icons.exe" 4 SW_SHOWMINIMIZED
CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\Traverser.lnk" '"$SYSDIR\cmd.exe"' '/c "$INSTDIR\bin_x86\traverser.bat"' $INSTDIR\bin_x86\icons.exe" 3 SW_SHOWMINIMIZED
SectionEnd

Section LabView
SetOutPath "$INSTDIR"
File /r /x MDSplus LabView
SetOutPath "$INSTDIR\mdsobjects\LabView"
File /r MDSplus
SetOutPath "$INSTDIR\mdsobjects\LabView\MDSplus"
File bin_x86/MdsObjectsCppShr.dll
File bin_x86/MDSobjectsLVShr.dll
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
SetOutPath "$INSTDIR"
File /r mdsobjects/python
SectionEnd

Section "DEVEL"
SetOutPath "$INSTDIR"
File /r include
${If} ${RunningX64}
CreateDirectory "$INSTDIR\devtools\lib64"
SetOutPath "$INSTDIR\devtools\lib64"
File "/oname=mdsshr.lib" bin_x86_64/MdsShr.dll.a
File "/oname=treeshr.lib" bin_x86_64/TreeShr.dll.a
File "/oname=tdishr.lib" bin_x86_64/TdiShr.dll.a
File "/oname=mdsdcl.lib" bin_x86_64/MdsDcl.dll.a
File "/oname=mdsipshr.lib" bin_x86_64/MdsIpShr.dll.a
File "/oname=mdslib_client.lib" bin_x86_64/MdsLib_client.dll.a
File "/oname=mdslib.lib" bin_x86_64/MdsLib.dll.a
File "/oname=mdsobjectscppshr.lib" bin_x86_64/MdsObjectsCppShr.dll.a
File "/oname=mdsservershr.lib" bin_x86_64/MdsServerShr.dll.a
File "/oname=rteventsshr.lib" bin_x86_64/RtEventsShr.dll.a
File "/oname=xtreeshr.lib" bin_x86_64/XTreeShr.dll.a
${EndIf}
CreateDirectory "$INSTDIR\devtools\lib32"
SetOutPath "$INSTDIR\devtools\lib32"
File "/oname=mdsshr.lib" bin_x86/MdsShr.dll.a
File "/oname=treeshr.lib" bin_x86/TreeShr.dll.a
File "/oname=tdishr.lib" bin_x86/TdiShr.dll.a
File "/oname=mdsdcl.lib" bin_x86/MdsDcl.dll.a
File "/oname=mdsipshr.lib" bin_x86/MdsIpShr.dll.a
File "/oname=mdslib_client.lib" bin_x86/MdsLib_client.dll.a
File "/oname=mdslib.lib" bin_x86/MdsLib.dll.a
File "/oname=mdsobjectscppshr.lib" bin_x86/MdsObjectsCppShr.dll.a
File "/oname=mdsservershr.lib" bin_x86/MdsServerShr.dll.a
File "/oname=rteventsshr.lib" bin_x86/RtEventsShr.dll.a
File "/oname=xtreeshr.lib" bin_x86/XTreeShr.dll.a
SectionEnd
 
# Uninstaller
 
function un.onInit
	SetShellVarContext all
 
	#Verify the uninstaller - last chance to back out
	MessageBox MB_OKCANCEL "Permanantly remove MDSplus${FLAVOR}?" IDOK next
		Abort
	next:
	!insertmacro VerifyUserIsAdmin
	${If} ${RunningX64}
	  ExecWait '"$INSTDIR\bin_x86_64\WinInstall.exe" /Uninstall' $0
	${Else}
	  ExecWait '"$INSTDIR\bin_x86\WinInstall.exe" /Uninstall' $0
	${EndIf}

functionEnd
 
section "uninstall"
SetOutPath "$INSTDIR"
delete ChangeLog.rtf
delete MDSplus-License.rtf
delete mdsplus.ico
RMdir /r $INSTDIR\tdi
RMdir /r $INSTDIR\include
RMdir /r $INSTDIR\devtools
delete uninstall.exe
RMdir /r /REBOOTOK $INSTDIR\bin_x86_64
RMdir /r /REBOOTOK $INSTDIR\bin_x86
RMdir /r $INSTDIR\trees
RMdir /r "$INSTDIR\idl"
RMdir /r "$INSTDIR\java"
RMdir /r "$INSTDIR\LabView"
RMdir /r "$INSTDIR\epics"
RMdir /r "$INSTDIR\matlab"
RMdir /r "$INSTDIR\python"
RMdir /r "$SMPROGRAMS\MDSplus${FLAVOR}"
RMdir "$INSTDIR"
# Registry information for add/remove programs
DeleteRegValue HKLM "${ENVREG}" MDS_PATH
DeleteRegValue HKLM "${ENVREG}" MDSPLUSDIR
DeleteRegValue HKLM "${ENVREG}" main_path
DeleteRegValue HKLM "${ENVREG}" subtree_path
DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus${FLAVOR}"
SectionEnd




















