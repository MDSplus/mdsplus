Name "MDSplus${FLAVOR} ${MAJOR}.${MINOR}.${RELEASE}"
Icon mdsplus.ico
InstallDir $PROGRAMFILES64\MDSplus${FLAVOR}
InstallDirRegKey HKLM Software\MDSplus${FLAVOR} InstallLocation
OutFile MDSplus${FLAVOR}-${MAJOR}.${MINOR}.${RELEASE}.exe
RequestExecutionLevel admin 
!define HELPURL "mailto:mdsplus@psfc.mit.edu" # "Support Information" link
!define UPDATEURL "http://www.mdsplus.org" # "Product Updates" link
!define ABOUTURL "http://www.mdsplus.org" # "Publisher" link
!define INSTALLSIZE 90000
!define ENVREG "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"
LicenseData "MDSplus-License.rtf"
 
!include LogicLib.nsh
 
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
functionEnd
 
Section
SetOutPath "$INSTDIR"
File ChangeLog
File MDSplus-License.rtf
writeUninstaller "$INSTDIR\uninstall.exe"
WriteRegStr HKLM "${ENVREG}" MDS_PATH "$INSTDIR\tdi"
WriteRegStr HKLM "${ENVREG}" MDSPLUSDIR "$INSTDIR"
CreateDirectory "$SMPROGRAMS\MDSplus${FLAVOR}"
CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\tditest(64).lnk" "$INSTDIR\bin_x86_64\tditest.exe"
CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\tditest(32).lnk" "$INSTDIR\bin_x86\tditest.exe"
CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\TCL.lnk" "$INSTDIR\bin_x86_64\mdstcl.bat"
ExecWait '"$INSTDIR\bin_x86_64\WinInstall.exe"/Install' $0
DetailPrint "WinInstall returned $0"
File /r /x local  tdi
File /r /x *.a bin_x86_64
File /r /x *.a bin_x86
File "/oname=$INSTDIR\bin_x86_64\libwinpthread-1.dll" /usr/x86_64-w64-mingw32/sys-root/mingw/bin/libwinpthread-1.dll
File "/oname=$INSTDIR\bin_x86\libwinpthread-1.dll" /usr/i686-w64-mingw32/sys-root/mingw/bin/libwinpthread-1.dll
File "/oname=$INSTDIR\bin_x86\libgcc_s_sjlj-1.dll" /usr/i686-w64-mingw32/sys-root/mingw/bin/libgcc_s_sjlj-1.dll
# Registry information for add/remove programs
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus${FLAVOR}" "DisplayName" "MDSplus${FLAVOR}"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus${FLAVOR}" "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus${FLAVOR}" "QuietUninstallString" "$\"$INSTDIR\uninstall.exe$\" /S"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus${FLAVOR}" "InstallLocation" "$\"$INSTDIR$\""
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus${FLAVOR}" "DisplayIcon" "$\"$INSTDIR\mdsplus.ico$\""
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus${FLAVOR}" "Publisher" "MDSplus Collaboratory"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus${FLAVOR}" "HelpLink" "$\"${HELPURL}$\""
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus${FLAVOR}" "URLUpdateInfo" "$\"${UPDATEURL}$\""
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus${FLAVOR}" "URLInfoAbout" "$\"${ABOUTURL}$\""
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus${FLAVOR}" "DisplayVersion" "$\"${MAJOR}.${MINOR}.${RELEASE}$\""
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus${FLAVOR}" "VersionMajor" ${MAJOR}
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus${FLAVOR}" "VersionMinor" ${MINOR}
# There is no option for modifying or repairing the install
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus${FLAVOR}" "NoModify" 1
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus${FLAVOR}" "NoRepair" 1
# Set the INSTALLSIZE constant (!defined at the top of this script) so Add/Remove Programs can accurately report the size
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus${FLAVOR}" "EstimatedSize" ${INSTALLSIZE}
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
File /r /x desktop java
SectionEnd

Section LabView
SetOutPath "$INSTDIR"
File /r LabView
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

 
# Uninstaller
 
function un.onInit
	SetShellVarContext all
 
	#Verify the uninstaller - last chance to back out
	MessageBox MB_OKCANCEL "Permanantly remove MDSplus${FLAVOR}?" IDOK next
		Abort
	next:
	!insertmacro VerifyUserIsAdmin
functionEnd
 
section "uninstall"
ExecWait '"$INSTDIR\bin_x86_64\WinInstall.exe"/Uninstall' $0
DetailPrint "WinInstall returned $0"
SetOutPath "$INSTDIR"
delete ChangeLog
delete MDSplus-License.rtf
RMdir /r $INSTDIR\tdi
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




















