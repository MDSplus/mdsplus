!include Sections.nsh
!include LogicLib.nsh
!include x64.nsh
!include StrFunc.nsh
!include WinMessages.nsh
!define SelectSection    '!insertmacro "SelectSection"'
!define UnselectSection  '!insertmacro "UnselectSection"'
!define ClearSectionFlag '!insertmacro "ClearSectionFlag"'
!define SetSectionFlag   '!insertmacro "SetSectionFlag"'

!macro _in SubString String IfCase ElseCase
  Push `${String}`
  Push `${SubString}`
  Call StrStr
  Pop $0
  StrCmp $0 "" `${ElseCase}` `${IfCase}`
!macroend
Function StrStr
  Exch $R0 ; SubString	(input)
  Exch	; next
  Exch $R1 ; String	(input)
  Push $R2 ; SubStringLen
  Push $R3 ; StrLen
  Push $R4 ; StartCharPos
  Push $R5 ; TempStr
  StrLen $R2 $R0
  StrLen $R3 $R1
  StrCpy $R4 0				;Start "StartCharPos" counter
  ${Do}					;Loop until "SubString" is found or "String" reaches its end
    StrCpy $R5 $R1 $R2 $R4			;Remove everything before and after the searched part ("TempStr")
    ${IfThen} $R5 == $R0 ${|} ${ExitDo} ${|}	;Compare "TempStr" with "SubString"
    ${IfThen} $R4 >= $R3 ${|} ${ExitDo} ${|}	;If not "SubString", this could be "String"'s end
    IntOp $R4 $R4 + 1				;If not, continue the loop
  ${Loop}
  Pop $R5 ; cleanup
  Pop $R4
  Pop $R3
  Pop $R2
  Pop $R1
  Exch $R0 ; return result
FunctionEnd
###############################

InstType "Typical" 
InstType "Full"
InstType "Minimal"
SetCompressor /FINAL LZMA

Name "MDSplus${FLAVOR} ${MAJOR}.${MINOR}-${RELEASE}"
Icon mdsplus.ico
UninstallIcon mdsplus.ico

RequestExecutionLevel user ; highest
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
	Pop $0
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
!define VerifyUserIsAdmin '!insertmacro "VerifyUserIsAdmin"'

Function install_core_pre
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

	SetOutPath "$INSTDIR\tdi"
	File /r /x local /x MitDevices /x RfxDevices /x KbsiDevices tdi/*.*
	SetOutPath "$INSTDIR\xml"
	File /r xml\*.*

	${IF} $7 == 1
		FileOpen $0 "$INSTDIR\installer.dat" w
		StrCpy $5 "$SYSDIR"
		StrCpy $6 "$SYSDIR"
	${ELSE}
		StrCpy $6 "$INSTDIR\bin_x86"
	${ENDIF}
FunctionEnd

Function install_core_post
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
FunctionEnd

SectionGroup "!core"
 Section "64 bit" bin64
	SectionIn RO
	Call install_core_pre
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
			Rename "$SYSDIR\$2" "$SYSDIR\$2-inuse"
			Delete "$SYSDIR\$2-inuse"
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
        Call install_core_post
 SectionEnd ; 64 bit

 Section "32 bit" bin32
	${IfNot} ${RunningX64}
	        Call install_core_pre
	${Else}
		FileOpen $0 "$INSTDIR\installer.dat" a
		FileSeek $0 0 END
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
			Rename "$5\$2" "$5\$2-inuse"
			Delete "$5\$2-inuse"
			Rename "$INSTDIR\bin_x86\$2" "$5\$2"
			FindNext $1 $2
			Goto loop_32
		done_32:
		FindClose $1
		RMDir "$INSTDIR\bin_x86"
		FileClose $0
	${ENDIF}
	${IfNot} ${RunningX64}
		Call install_core_post
	${EndIf}
 SectionEnd ; 32 bit
SectionGroupEnd ; core

SectionGroup devices
 SectionGroup tdi
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
 SectionGroup "pydevices" pydevices
  Section HTS pydevices_hts
	SectionIn 2
	SetOutPath "$INSTDIR\pydevices\HtsDevices"
	File /r pydevices/HtsDevices/*.*
  SectionEnd ; HTS
  Section MIT pydevices_mit
	SectionIn 2
	SetOutPath "$INSTDIR\pydevices\MitDevices"
	File /r pydevices/MitDevices/*.*
	File /workspace/releasebld/64/pydevices/MitDevices/_version.py
  SectionEnd ; MIT
  Section RFX pydevices_rfx
	SectionIn 2
	SetOutPath "$INSTDIR\pydevices\RfxDevices"
	File /r pydevices/RfxDevices/*.*
	File /workspace/releasebld/64/pydevices/RfxDevices/_version.py
  SectionEnd ; RFX
  Section W7X pydevices_w7x
	SectionIn 2
	SetOutPath "$INSTDIR\pydevices\W7xDevices"
	File /r pydevices/W7xDevices/*.*
	File /workspace/releasebld/64/pydevices/W7xDevices/_version.py
  SectionEnd ; W7X
  Section "setup MDS_PYDEVICE_PATH" pydevpath
	SectionIn 2 RO
	${IF} $7 == 1
		ReadRegStr $R0 HKLM "${ENVREG_ALL}" "MDS_PYDEVICE_PATH"
	${ELSE}
		ReadRegStr $R0 HKCU "${ENVREG_USR}" "MDS_PYDEVICE_PATH"
	${ENDIF}
	StrCmp $R0 "" new_pydevpath
	${IfNot} "$INSTDIR\pydevices" in "$R0"
		${IF} $7 == 1
			WriteRegStr HKLM "${ENVREG_ALL}" "MDS_PYDEVICE_PATH" "$R0;$INSTDIR\pydevices"
		${ELSE}
			WriteRegStr HKCU "${ENVREG_USR}" "MDS_PYDEVICE_PATH" "$R0;$INSTDIR\pydevices"
		${ENDIF}
	${EndIf}
	Goto done_pydevpath
	new_pydevpath:
	${IF} $7 == 1
		WriteRegStr HKLM "${ENVREG_ALL}" "MDS_PYDEVICE_PATH" "$INSTDIR\pydevices"
	${ELSE}
		WriteRegStr HKCU "${ENVREG_USR}" "MDS_PYDEVICE_PATH" "$INSTDIR\pydevices"
	${ENDIF}
	done_pydevpath:
  SectionEnd ; pydevpath
 SectionGroupEnd ; pydevices
SectionGroupEnd ; devices

Section "Java"
	SectionIn 1 2
	SetOutPath $INSTDIR/java
	File /r java/classes
	CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\Scope.lnk" javaw '-cp "$INSTDIR\java\Classes\jScope.jar";"$INSTDIR\java\Classes" -Xmx1G jScope' "$6\icons.exe" 4 SW_SHOWMINIMIZED
	CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\Traverser.lnk"  javaw '-cp "$INSTDIR\java\Classes\jTraverser.jar" jTraverser' $6\icons.exe" 3 SW_SHOWMINIMIZED
	CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\Traverser2.lnk" javaw '-jar "$INSTDIR\java\Classes\jTraverser2.jar"' $6\icons.exe" 3 SW_SHOWMINIMIZED
SectionEnd ; Java

SectionGroup /e "!APIs"
 Section "EPICS"
	SectionIn 2
	SetOutPath "$INSTDIR\epics"
	File /r epics/*.*
 SectionEnd ; EPICS
 Section "IDL"
	SectionIn 2
	SetOutPath "$INSTDIR\idl"
	File /r idl/*.*
 SectionEnd ; IDL
 SectionGroup "LabView"
  Section "LV 5/6 (1998-2000)"
	SectionIn 2
	SetOutPath "$INSTDIR\LabView\MDSplus"
	File LabView/*.vi
  SectionEnd ; LV 5/6 (1998-2000)
  Section "LV2010"
	SectionIn 2
	SetOutPath "$INSTDIR\LabView\MDSplus_LV2010"
	File /r LabView/MDSplus/*.*
  SectionEnd ; LV2010
  Section "LV2012"
	SectionIn 2
	SetOutPath "$INSTDIR\LabView"
	File /r LabView/MDSplus_LV2012
  SectionEnd ; LV2012
  Section "LV2015"
	SectionIn 2
	SetOutPath "$INSTDIR\LabView"
	File /r LabView/MDSplus_LV2015
  SectionEnd ; LV2015
 SectionGroupEnd ; LabView
 Section "MATLAB"
	SectionIn 2
	SetOutPath "$INSTDIR\matlab"
	File /r matlab/*.*
 SectionEnd ; MATLAB
 SectionGroup /e "!python" python
  Section "MDSplus package" python_cp
	SectionIn 1 2
	SetOutPath "$INSTDIR\python\MDSplus"
	File /r /x makedoc.sh mdsobjects/python/*.*
	File /workspace/releasebld/64/mdsobjects/python/_version.py
  SectionEnd ; python_cp
  Section "run 'python setup.py install'" python_su
	SectionIn 2
	SetOutPath "$INSTDIR\python\MDSplus"
	nsExec::Exec /OEM /TIMEOUT=10000 "python setup.py install"
	Pop $0
  SectionEnd ; python_su
 SectionGroupEnd ; python
SectionGroupEnd ; APIs

Section "devel"
	SectionIn 2
	SetOutPath "$INSTDIR\include"
	File /r include/*.*
	${If} ${RunningX64}
		CreateDirectory "$INSTDIR\devtools\lib64\mingw"
		SetOutPath "$INSTDIR\devtools\lib64\mingw"
		File "/oname=mdsshr.lib" bin_x86_64/MdsShr.dll.a
		File "/oname=treeshr.lib" bin_x86_64/TreeShr.dll.a
		File "/oname=tdishr.lib" bin_x86_64/TdiShr.dll.a
		File "/oname=mdsdcl.lib" bin_x86_64/Mdsdcl.dll.a
		File "/oname=mdsipshr.lib" bin_x86_64/MdsIpShr.dll.a
		File "/oname=mdslib.lib" bin_x86_64/MdsLib.dll.a
		File "/oname=mdsobjectscppshr.lib" bin_x86_64/MdsObjectsCppShr.dll.a
		File "/oname=mdsservershr.lib" bin_x86_64/MdsServerShr.dll.a
		File "/oname=xtreeshr.lib" bin_x86_64/XTreeShr.dll.a
		CreateDirectory "$INSTDIR\devtools\lib64\visual_studio"
		SetOutPath "$INSTDIR\devtools\lib64\visual_studio"
		File "/oname=mdsshr.lib" bin_x86_64/MdsShr.lib
		File "/oname=treeshr.lib" bin_x86_64/TreeShr.lib
		File "/oname=tdishr.lib" bin_x86_64/TdiShr.lib
		File "/oname=mdsdcl.lib" bin_x86_64/Mdsdcl.lib
		File "/oname=mdsipshr.lib" bin_x86_64/MdsIpShr.lib
		File "/oname=mdslib.lib" bin_x86_64/MdsLib.lib
		File "/oname=mdsobjectscppshr-vs.lib" bin_x86_64/MdsObjectsCppShr-VS.lib
		File "/oname=mdsservershr.lib" bin_x86_64/MdsServerShr.lib
	${EndIf}
	CreateDirectory "$INSTDIR\devtools\lib32\mingw"
	SetOutPath "$INSTDIR\devtools\lib32\mingw"
	File "/oname=mdsshr.lib" bin_x86/MdsShr.dll.a
	File "/oname=treeshr.lib" bin_x86/TreeShr.dll.a
	File "/oname=tdishr.lib" bin_x86/TdiShr.dll.a
	File "/oname=mdsdcl.lib" bin_x86/Mdsdcl.dll.a
	File "/oname=mdsipshr.lib" bin_x86/MdsIpShr.dll.a
	File "/oname=mdslib.lib" bin_x86/MdsLib.dll.a
	File "/oname=mdsobjectscppshr.lib" bin_x86/MdsObjectsCppShr.dll.a
	File "/oname=mdsservershr.lib" bin_x86/MdsServerShr.dll.a
	File "/oname=xtreeshr.lib" bin_x86/XTreeShr.dll.a
	CreateDirectory "$INSTDIR\devtools\lib32\visual_studio"
	SetOutPath "$INSTDIR\devtools\lib32\visual_studio"
	File "/oname=mdsshr.lib" bin_x86/MdsShr.lib
	File "/oname=treeshr.lib" bin_x86/TreeShr.lib
	File "/oname=tdishr.lib" bin_x86/TdiShr.lib
	File "/oname=mdsdcl.lib" bin_x86/mdsdcl.lib
	File "/oname=mdsipshr.lib" bin_x86/mdsipshr.lib
	File "/oname=mdslib.lib" bin_x86/MdsLib.lib
	File "/oname=mdsservershr.lib" bin_x86/MdsServerShr.lib
SectionEnd

Section "sample trees"
	SectionIn 2
	${IF} $7 == 1
		WriteRegStr HKLM "${ENVREG_ALL}" "main_path"    "$INSTDIR\trees"
		WriteRegStr HKLM "${ENVREG_ALL}" "subtree_path" "$INSTDIR\trees\subtree"
	${ELSE}
		WriteRegStr HKCU "${ENVREG_USR}" "main_path"    "$INSTDIR\trees"
		WriteRegStr HKCU "${ENVREG_USR}" "subtree_path" "$INSTDIR\trees\subtree"
	${ENDIF}
	SetOutPath "$INSTDIR\trees"
	File /r trees/*.*
SectionEnd

Section "add to PATH" appendpath
	ReadRegStr $R0 HKCU "${ENVREG_USR}" "PATH"
	StrCmp $R0 "" new_path
	${IfNot} "$6" in "$R0"
		WriteRegStr HKCU "${ENVREG_USR}" "PATH" "$R0;$6"
	${EndIf}
	Goto done_path

	new_path:
	WriteRegStr HKCU "${ENVREG_USR}" "PATH" "$6"

	done_path:
SectionEnd

Function .onSelChange
	; pydevices depend on python
	${If} $0 == ${python_cp}
		${IfNot} ${python_cp} SectionFlagIsSet ${SF_SELECTED}
			${UnselectSection} ${python_su}
			${UnselectSection} ${pydevices}
		${EndIf}
	${Else}
		${If}   ${python_su} SectionFlagIsSet ${SF_PSELECTED}
		${OrIf} ${pydevices} SectionFlagIsSet ${SF_PSELECTED}
			${SelectSection} ${python_cp}
		${EndIf}
	${EndIf}
	${If}   ${pydevices_hts} SectionFlagIsSet ${SF_SELECTED}
	${OrIf} ${pydevices_mit} SectionFlagIsSet ${SF_SELECTED}
	${OrIf} ${pydevices_rfx} SectionFlagIsSet ${SF_SELECTED}
	${OrIf} ${pydevices_w7x} SectionFlagIsSet ${SF_SELECTED}
		${ClearSectionFlag}  ${pydevpath} ${SF_RO}
	${Else}
		${UnselectSection} ${pydevpath}
		${SetSectionFlag}  ${pydevpath} ${SF_RO}
	${EndIf}
FunctionEnd

Function .onInit
	${If} ${RunningX64}
		SectionSetInstTypes ${bin32} 2 ; include 32bit in full
		SectionSetInstTypes ${bin64} 7 ; always include 64bit
	${Else}
		${SetSectionFlag}   ${bin32} ${SF_RO}
		${SelectSection}    ${bin32}
		SectionSetInstTypes ${bin32} 7 ; always include 32bit
		SectionSetInstTypes ${bin64} 0 ; never include 64bit
	${EndIf}
	${VerifyUserIsAdmin}
	${If} $7 == 1
		ReadRegStr $R0 HKLM ${UNINSTALL_KEY} ${UNINSTALL_VAL}
		SectionSetFlags ${appendpath} ${SF_RO}
	${Else}
		ReadRegStr $R0 HKCU ${UNINSTALL_KEY} ${UNINSTALL_VAL}
		SectionSetFlags ${appendpath} ${SF_SELECTED}
	${EndIf}
	StrCmp $R0 "" done
	MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION \
	"MDSplus is already installed. $\n$\nClick `OK` to remove the \
	previous version or `Cancel` to cancel this upgrade." IDOK uninst
		Abort
	; Run the uninstaller

	uninst:
	ClearErrors
  	ExecWait '$R0 _?=$INSTDIR' ;Do not copy the uninstaller to a temp file

	done:
FunctionEnd

Function .onGUIEnd
	SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
FunctionEnd


# Uninstaller
 
Function un.onInit
	#Verify the uninstaller - last chance to back out
	MessageBox MB_OKCANCEL "Permanantly remove MDSplus${FLAVOR}?" IDOK next
		Abort
	next:
	${VerifyUserIsAdmin}
functionEnd

Function un.onGUIEnd
	SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
FunctionEnd
 
Section "uninstall"
	IfFileExists "$INSTDIR\mdsobjects\python" 0 skip_python_remove
	SetOutPath "$INSTDIR\mdsobjects\python"
	nsExec::ExecToLog /OEM /TIMEOUT=10000 'python setup.py remove'
	Pop $0
	skip_python_remove:
	SetOutPath "$INSTDIR"
	Delete uninstall.exe
	RMDir /r "$SMPROGRAMS\MDSplus${FLAVOR}"
	${IF} $7 == 1
		nsExec::ExecToLog /OEM /timeout=5000 '"$SYSDIR\mdsip_service.exe" "-r -p 8100"'
		Pop $0
		nsExec::ExecToLog /OEM /timeout=5000 '"$SYSDIR\mdsip_service.exe" "-r -p 8000"'
		Pop $0
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
		Delete installer.dat

		DeleteRegValue HKLM "${ENVREG_ALL}" MDS_PATH
		DeleteRegValue HKLM "${ENVREG_ALL}" MDSPLUS_DIR
		DeleteRegValue HKLM "${ENVREG_ALL}" main_path
		DeleteRegValue HKLM "${ENVREG_ALL}" subtree_path
		DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus"
	${ELSE}
		DeleteRegValue HKCU "${ENVREG_USR}" MDS_PATH
		DeleteRegValue HKCU "${ENVREG_USR}" MDSPLUS_DIR
		DeleteRegValue HKCU "${ENVREG_USR}" main_path
		DeleteRegValue HKCU "${ENVREG_USR}" subtree_path
		DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus"
	${ENDIF}
	SetOutPath "$SYSDIR"
	RMDir /r "$INSTDIR"
SectionEnd
