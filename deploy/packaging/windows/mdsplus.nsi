!include LogicLib.nsh
!include x64.nsh
!include StrFunc.nsh
!include WinMessages.nsh
Var STR_HAYSTACK
Var STR_NEEDLE
Var STR_CONTAINS_VAR_1
Var STR_CONTAINS_VAR_2
Var STR_CONTAINS_VAR_3
Var STR_CONTAINS_VAR_4
Var STR_RETURN_VAR
 
Function StrContains
  Exch $STR_NEEDLE
  Exch 1
  Exch $STR_HAYSTACK
  ; Uncomment to debug
  ;MessageBox MB_OK 'STR_NEEDLE = $STR_NEEDLE STR_HAYSTACK = $STR_HAYSTACK '
    StrCpy $STR_RETURN_VAR ""
    StrCpy $STR_CONTAINS_VAR_1 -1
    StrLen $STR_CONTAINS_VAR_2 $STR_NEEDLE
    StrLen $STR_CONTAINS_VAR_4 $STR_HAYSTACK
    loop:
      IntOp $STR_CONTAINS_VAR_1 $STR_CONTAINS_VAR_1 + 1
      StrCpy $STR_CONTAINS_VAR_3 $STR_HAYSTACK $STR_CONTAINS_VAR_2 $STR_CONTAINS_VAR_1
      StrCmp $STR_CONTAINS_VAR_3 $STR_NEEDLE found
      StrCmp $STR_CONTAINS_VAR_1 $STR_CONTAINS_VAR_4 done
      Goto loop
    found:
      StrCpy $STR_RETURN_VAR $STR_NEEDLE
      Goto done
    done:
   Pop $STR_NEEDLE ;Prevent "invalid opcode" errors and keep the
   Exch $STR_RETURN_VAR  
FunctionEnd
 
!macro _StrContainsConstructor OUT NEEDLE HAYSTACK
  Push `${HAYSTACK}`
  Push `${NEEDLE}`
  Call StrContains
  Pop `${OUT}`
!macroend
 
!define StrContains '!insertmacro "_StrContainsConstructor"'

InstType "Typical" 
InstType "Full"
InstType "Minimal"
SetCompressor /FINAL LZMA

Name "MDSplus${FLAVOR} ${MAJOR}.${MINOR}-${RELEASE}"
Icon mdsplus.ico
UninstallIcon mdsplus.ico

RequestExecutionLevel highest
OutFile ${OUTDIR}/MDSplus${FLAVOR}-${MAJOR}.${MINOR}-${RELEASE}.exe
!define HELPURL "mailto:mdsplus@psfc.mit.edu" # "Support Information" link
!define UPDATEURL "http://www.mdsplus.org" # "Product Updates" link
!define ABOUTURL "http://www.mdsplus.org" # "Publisher" link
!define INSTALLSIZE 90000
!define ENVREG_ALL "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"
!define ENVREG_USR "Environment"
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
!define GFORTRAN_LIB libgfortran-4.dll
!define QUADMATH_LIB libquadmath-0.dll
!define LIBXML2_LIB libxml2-2.dll
!define ICONV_LIB iconv.dll
!define ZLIB1_LIB zlib1.dll
!define UNINSTALL_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus"
!define UNINSTALL_VAL "UninstallString"
LicenseData "MDSplus-License.rtf"
Page license
Page directory
Page components /ENABLECANCEL
Page instfiles

${UnStrTrimNewLines} 
 
!macro VerifyUserIsAdmin
UserInfo::GetAccountType
pop $0
${If} $0 == "admin"
        SetShellVarContext all
        ${If} ${RunningX64}
		StrCpy $INSTDIR $PROGRAMFILES64\MDSplus
        ${Else}
		StrCpy $INSTDIR $PROGRAMFILES32\MDSplus
        ${EndIf}
        StrCpy $7 1
${Else}
	SetShellVarContext current
	StrCpy $INSTDIR $PROFILE\MDSplus
	StrCpy $7 0
${EndIf}
!macroend

 
Section
SectionIn 1 2 3 RO
SetOutPath "$INSTDIR"
File "/oname=ChangeLog.rtf" ChangeLog
File ${srcdir}/mdsplus.ico
File ${srcdir}/MDSplus-License.rtf
writeUninstaller "$INSTDIR\uninstall.exe"
${If} $7 == 1
	WriteRegStr HKLM "${ENVREG_ALL}" MDS_PATH "$INSTDIR\tdi"
	WriteRegStr HKLM "${ENVREG_ALL}" MDSPLUS_DIR "$INSTDIR"
${ELSE}
	WriteRegStr HKCU "${ENVREG_USR}" MDS_PATH "$INSTDIR\tdi"
	WriteRegStr HKCU "${ENVREG_USR}" MDSPLUS_DIR "$INSTDIR"
${ENDIF}

File /r /x local  tdi xml

${IF} $7 == 1
	FileOpen $0 "$INSTDIR\installer.dat" w
	StrCpy $5 "$SYSDIR"
	StrCpy $6 "$SYSDIR"
${ELSE}
	StrCpy $6 "$INSTDIR\bin_x86"
${ENDIF}
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
	${IF} $7 == 1
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
		RMDir "$INSTDIR\bin_x86_64"
	${ELSE}
		StrCpy $6 "$INSTDIR\bin_x86_64"
	${ENDIF}
	${EnableX64FSRedirection}
	SetOutPath "$INSTDIR"
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
${IF} $7 == 1
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
	RMDir "$INSTDIR\bin_x86"
	FileClose $0
${ENDIF}

SetOutPath "$6"
CreateDirectory "$SMPROGRAMS\MDSplus${FLAVOR}"
CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\Tdi.lnk" "$6\tditest.exe" "" "$6\icons.exe" 0
CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\TCL.lnk" '"$6\mdsdcl"' '-prep "set command tcl"' "$6\icons.exe" 1
CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\View ChangeLog.lnk" "$INSTDIR\ChangeLog.rtf"
${IF} $7 == 1
	CreateDirectory "$SMPROGRAMS\MDSplus${FLAVOR}\DataServer"
	CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\DataServer\Install mdsip action server on port 8100.lnk" "$SYSDIR\mdsip_service.exe" "-i -s -p 8100 -h $\"C:\mdsip.hosts$\""
	CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\DataServer\Install mdsip data server on port 8000.lnk" "$SYSDIR\mdsip_service.exe" "-i -p 8000 -h $\"C:\mdsip.hosts$\""
	CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\DataServer\Remove mdsip server on port 8100.lnk" "$SYSDIR\mdsip_service.exe" "-r -p 8100"
	CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\DataServer\Remove mdsip server on port 8000.lnk" "$SYSDIR\mdsip_service.exe" "-r -p 8000"
${ENDIF}

SetOutPath "\"
SetOverWrite off
File etc\mdsip.hosts
SetOverWrite on

# Registry information for add/remove programs
${IF} $7 == 1
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "DisplayName" "MDSplus${FLAVOR}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "UninstallString" "$INSTDIR\uninstall.exe"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "QuietUninstallString" "$INSTDIR\uninstall.exe /S"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "InstallLocation" "$INSTDIR"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "DisplayIcon" "INSTDIR\mdsplus.ico"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "Publisher" "MDSplus Collaboratory"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "HelpLink" "${HELPURL}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "InstallSource" "https://github.com/MDSplus/mdsplus/archive/${BRANCH}_release-${MAJOR}.${MINOR}-${RELEASE}.zip"
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
${ELSE}
	WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "DisplayName" "MDSplus${FLAVOR}"
	WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "UninstallString" "$INSTDIR\uninstall.exe"
	WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "QuietUninstallString" "$INSTDIR\uninstall.exe /S"
	WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "InstallLocation" "$INSTDIR"
	WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "DisplayIcon" "INSTDIR\mdsplus.ico"
	WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "Publisher" "MDSplus Collaboratory"
	WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "HelpLink" "${HELPURL}"
	WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "InstallSource" "https://github.com/MDSplus/mdsplus/archive/${BRANCH}_release-${MAJOR}.${MINOR}-${RELEASE}.zip"
	WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "URLUpdateInfo" "${UPDATEURL}"
	WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "URLInfoAbout" "${ABOUTURL}"
	WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "DisplayVersion" "${MAJOR}.${MINOR}.${RELEASE}"
	WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "VersionMajor" ${MAJOR}
	WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "VersionMinor" ${MINOR}
	# There is no option for modifying or repairing the install
	WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "NoModify" 1
	WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "NoRepair" 1
	# Set the INSTALLSIZE constant (!defined at the top of this script) so Add/Remove Programs can accurately report the size
	WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "EstimatedSize" ${INSTALLSIZE}
${ENDIF}

SectionEnd


Section "Java"
SectionIn 1 2
SetOutPath $INSTDIR
File /r /x desktop java
SetOutPath "$6"
CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\Scope.lnk" '"$SYSDIR\cmd.exe"' '/c start "" javaw -cp "$INSTDIR\java\Classes\jScope.jar";"$INSTDIR\java\Classes" -Xmx1G jScope' "$6\icons.exe" 4 SW_SHOWMINIMIZED
CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\Traverser.lnk" '"$SYSDIR\cmd.exe"' '/c start "" javaw -cp "$INSTDIR\java\Classes\jTraverser.jar" jTraverser' $6\icons.exe" 3 SW_SHOWMINIMIZED
SectionEnd


Section EPICS
SectionIn 2
SetOutPath "$INSTDIR"
File /r epics
SectionEnd


Section "IDL"
SectionIn 2
SetOutPath "$INSTDIR"
File /r idl
SectionEnd


Section LabView
SectionIn 2
SetOutPath "$INSTDIR\LabView"
File LabView/*.vi
SetOutPath "$INSTDIR\mdsobjects\LabView"
File /r LabView/MDSplus
File /r LabView/MDSplus_LV2012
File /r LabView/MDSplus_LV2015
SectionEnd


Section "MATLAB"
SectionIn 2
SetOutPath "$INSTDIR"
File /r matlab
SectionEnd


Section "python"
SectionIn 2
SetOutPath "$INSTDIR\mdsobjects"
File /r /x MDSplus /x .gitignore ${srcdir}/mdsobjects/python
SetOutPath "$INSTDIR\mdsobjects\python"
File /workspace/releasebld/64/mdsobjects/python/_version.py
SetOutPath "$INSTDIR\mdsobjects\python"
Exec "python setup.py install" 
SectionEnd


Section "devel"
SectionIn 2
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


Section "sample trees"
SectionIn 2
SetOutPath "$INSTDIR"
${IF} $7 == 1
	WriteRegStr HKLM "${ENVREG_ALL}" "main_path" "$INSTDIR\trees"
	WriteRegStr HKLM "${ENVREG_ALL}" "subtree_path" "$INSTDIR\trees\subtree"
${ELSE}
	WriteRegStr HKCU "${ENVREG_USR}" "main_path" "$INSTDIR\trees"
	WriteRegStr HKCU "${ENVREG_USR}" "subtree_path" "$INSTDIR\trees\subtree"
${ENDIF}
File /r trees
SectionEnd

 
Section "append to PATH" appendpath
ReadRegStr $R0 HKCU ${ENVREG_USR} "PATH"
StrCmp $R0 "" new_path
${StrContains} $0 "$6" "$R0"
StrCmp $0 "" append_path
Goto done_path
append_path:
WriteRegStr HKCU "${ENVREG_USR}" "PATH" "$R0;$6"
Goto done_path
new_path:
WriteRegStr HKCU "${ENVREG_USR}" "PATH" "$6"
done_path:
SectionEnd


function .onInit
	!insertmacro VerifyUserIsAdmin
	${If} $7 == 1
		ReadRegStr $R0 HKLM ${UNINSTALL_KEY} ${UNINSTALL_VAL}
		SectionSetFlags ${appendpath} ${SF_RO}
	${Else}
		ReadRegStr $R0 HKCU ${UNINSTALL_KEY} ${UNINSTALL_VAL}
		SectionSetFlags ${appendpath} 0
	${EndIf}
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


# Uninstaller
 
function un.onInit
 
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
${IF} $7 == 1
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
# Registry information for add/remove programs
	DeleteRegValue HKLM "${ENVREG_ALL}" MDS_PATH
	DeleteRegValue HKLM "${ENVREG_ALL}" MDSPLUS_DIR
	DeleteRegValue HKLM "${ENVREG_ALL}" main_path
	DeleteRegValue HKLM "${ENVREG_ALL}" subtree_path
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus"
${ELSE}
	${If} ${RunningX64}
		RMDir /r "$INSTDIR\bin_x86_64"
	${ENDIf}
	RMDir /r "$INSTDIR\bin_x86"
	DeleteRegValue HKCU "${ENVREG_USR}" MDS_PATH
	DeleteRegValue HKCU "${ENVREG_USR}" MDSPLUS_DIR
	DeleteRegValue HKCU "${ENVREG_USR}" main_path
	DeleteRegValue HKCU "${ENVREG_USR}" subtree_path
	DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus"
${ENDIF}
SetOutPath "$SYSDIR"
RMDir "$INSTDIR"
SectionEnd
		
