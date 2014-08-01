!include WinMessages.nsh

Name "MDSplus${FLAVOR} ${MAJOR}.${MINOR}.${RELEASE}"
Icon mdsplus.ico
InstallDir $PROGRAMFILES64\MDSplus${FLAVOR}
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
!define DLLIB libdl.dll
!define READLINELIB libreadline6.dll
!define GCC_S_SJLJ_LIB libgcc_s_sjlj-1.dll
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
SetShellVarContext all
File "/oname=$INSTDIR\ChangeLog.rtf" ChangeLog
File mdsplus.ico
File MDSplus-License.rtf
writeUninstaller "$INSTDIR\uninstall.exe"
WriteRegStr HKLM "${ENVREG}" MDS_PATH "$INSTDIR\tdi"
WriteRegStr HKLM "${ENVREG}" MDSPLUSDIR "$INSTDIR"
CreateDirectory "$SMPROGRAMS\MDSplus${FLAVOR}"
CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\tditest(64).lnk" "$INSTDIR\bin_x86_64\tditest.exe" "" "$INSTDIR\bin_x86_64\icons.exe" 0
CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\tditest(32).lnk" "$INSTDIR\bin_x86\tditest.exe" "" "$INSTDIR\bin_x86\icons.exe" 0
CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\TCL.lnk" "$INSTDIR\bin_x86_64\mdstcl.bat" "" "$INSTDIR\bin_x86_64\icons.exe" 1
CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\View ChangeLog.lnk" "$INSTDIR\ChangeLog.rtf"
CreateDirectory "$SMPROGRAMS\MDSplus${FLAVOR}\DataServer"
CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\DataServer\Install mdsip action server on port 8100.lnk" "$INSTDIR\bin_x86_64\mdsip_service.exe" "-i -s -p 8100 -h $\"C:\mdsip.hosts$\""
CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\DataServer\Install mdsip data server on port 8000.lnk" "$INSTDIR\bin_x86_64\mdsip_service.exe" "-i -p 8000 -h $\"C:\mdsip.hosts$\""
CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\DataServer\Remove mdsip server on port 8100.lnk" "$INSTDIR\bin_x86_64\mdsip_service.exe" "-r -p 8100"
CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\DataServer\Remove mdsip server on port 8000.lnk" "$INSTDIR\bin_x86_64\mdsip_service.exe" "-r -p 8000"

File /r /x local  tdi
File /r /x *.a bin_x86_64
File /r /x *.a bin_x86
File "/oname=$INSTDIR\bin_x86_64\${PTHREADLIB}" ${MINGWLIB64}/${PTHREADLIB}
File "/oname=$INSTDIR\bin_x86\${PTHREADLIB}" ${MINGWLIB32}/${PTHREADLIB}
File "/oname=$INSTDIR\bin_x86\${GCC_S_SJLJ_LIB}" ${MINGWLIB32}/${GCC_S_SJLJ_LIB}
File "/oname=$INSTDIR\bin_x86_64\${DLLIB}" ${MINGWLIB64}/${DLLIB}
File "/oname=$INSTDIR\bin_x86\${DLLIB}" ${MINGWLIB32}/${DLLIB}
File "/oname=$INSTDIR\bin_x86_64\${READLINELIB}" ${MINGWLIB64}/${READLINELIB}
File "/oname=$INSTDIR\bin_x86\${READLINELIB}" ${MINGWLIB32}/${READLINELIB}

# Registry information for add/remove programs
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus${FLAVOR}" "DisplayName" "MDSplus${FLAVOR}"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus${FLAVOR}" "UninstallString" "$INSTDIR\uninstall.exe"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus${FLAVOR}" "QuietUninstallString" "$INSTDIR\uninstall.exe /S"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus${FLAVOR}" "InstallLocation" "$INSTDIR"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus${FLAVOR}" "DisplayIcon" "INSTDIR\mdsplus.ico"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus${FLAVOR}" "Publisher" "MDSplus Collaboratory"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus${FLAVOR}" "HelpLink" "${HELPURL}"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus${FLAVOR}" "InstallSource" "http://www.mdsplus.org/dist/SOURCES/mdsplus${FLAVOR}-${MAJOR}.${MINOR}-${RELEASE}.tgz"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus${FLAVOR}" "URLUpdateInfo" "${UPDATEURL}"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus${FLAVOR}" "URLInfoAbout" "${ABOUTURL}"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus${FLAVOR}" "DisplayVersion" "${MAJOR}.${MINOR}.${RELEASE}"
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus${FLAVOR}" "VersionMajor" ${MAJOR}
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus${FLAVOR}" "VersionMinor" ${MINOR}
# There is no option for modifying or repairing the install
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus${FLAVOR}" "NoModify" 1
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus${FLAVOR}" "NoRepair" 1
# Set the INSTALLSIZE constant (!defined at the top of this script) so Add/Remove Programs can accurately report the size
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus${FLAVOR}" "EstimatedSize" ${INSTALLSIZE}
ExecWait '"$INSTDIR\bin_x86_64\WinInstall.exe" /Install' $0
DetailPrint "WinInstall returned $0"
SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
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
CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\Scope.lnk" "$INSTDIR\bin_x86_64\jScope.bat" "" "$INSTDIR\bin_x86_64\icons.exe" 4 SW_SHOWMINIMIZED
CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\Traverser.lnk" "$INSTDIR\bin_x86_64\traverser.bat" "" $INSTDIR\bin_x86_64\icons.exe" 3 SW_SHOWMINIMIZED
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

Section "DEVEL"
SetOutPath "$INSTDIR"
File /r include
CreateDirectory "$INSTDIR\devtools\lib"
File "/oname=$INSTDIR\devtools\lib\mdsshr.lib" bin_x86_64/MdsShr.dll.a
File "/oname=$INSTDIR\devtools\lib\treeshr.lib" bin_x86_64/TreeShr.dll.a
File "/oname=$INSTDIR\devtools\lib\tdishr.lib" bin_x86_64/TdiShr.dll.a
File "/oname=$INSTDIR\devtools\lib\mdsdcl.lib" bin_x86_64/MdsDcl.dll.a
File "/oname=$INSTDIR\devtools\lib\mdsipshr.lib" bin_x86_64/MdsIpShr.dll.a
File "/oname=$INSTDIR\devtools\lib\mdslib_client.lib" bin_x86_64/MdsLib_client.dll.a
File "/oname=$INSTDIR\devtools\lib\mdslib.lib" bin_x86_64/MdsLib.dll.a
File "/oname=$INSTDIR\devtools\lib\mdsobjectscppshr.lib" bin_x86_64/MdsObjectsCppShr.dll.a
File "/oname=$INSTDIR\devtools\lib\mdsservershr.lib" bin_x86_64/MdsServerShr.dll.a
File "/oname=$INSTDIR\devtools\lib\rteventsshr.lib" bin_x86_64/RtEventsShr.dll.a
File "/oname=$INSTDIR\devtools\lib\xtreeshr.lib" bin_x86_64/XTreeShr.dll.a
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
ExecWait '"$INSTDIR\bin_x86_64\WinInstall.exe" /Uninstall' $0
DetailPrint "WinInstall returned $0"
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




















