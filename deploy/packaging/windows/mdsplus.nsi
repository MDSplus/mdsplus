!include Sections.nsh
!include LogicLib.nsh
!include x64.nsh
!include StrFunc.nsh
!include WinMessages.nsh
!define SelectSection    '!insertmacro "SelectSection"'
!define UnselectSection  '!insertmacro "UnselectSection"'
!define ClearSectionFlag '!insertmacro "ClearSectionFlag"'
!define SetSectionFlag   '!insertmacro "SetSectionFlag"'

!macro ToLog line
	Push `${line}`
	Exch $0
	Push $R0
	FileOpen  $R0 `$INSTDIR\log.log` a
	FileSeek  $R0 0 END
	FileWrite $R0 `$0`
	FileClose $R0
	Pop $R0
	Pop $0
!macroend ; ToLog
!define ToLog '!insertmacro "ToLog"'

Var ISVAR
!macro _in substr string t f
	; workaround bug in LogicLib.nsh
	!ifdef _c=false
	!define _c=false_
	!undef _c=false
	!endif
	!ifdef _c=true
	!define _c=true_
	!undef _c=true
	!endif
	StrCpy $ISVAR -1
	Push `${string}`
	Push `${substr}`
	Exch $0
	Exch
	Exch $1
	Push $R0 ; substr len
	Push $R1 ; string len
	Push $R2 ; char pos
	Push $R3 ; tmpstr
	${ToLog} "$0 in $1$\n"
	StrLen $R0 $0
	StrLen $R1 $1
	IntOp  $R1 $R1 - $R0
	StrCpy $R2 0				;init char pos
	${Do}					;Loop until "substr" is found or "string" reaches its end
		${If} $R2 > $R1
			${ToLog} `not found "$0" $ISVAR$\n`
			${ExitDo} ;Check if end of "string"
		${EndIf}
		StrCpy $R3 `$1` $R0 $R2	;Trim "tmpstr" to len of "substr"
		${If} $R3 == `$0`	;Compare "tmpStr" with "substr"
			StrCpy $ISVAR $R2	;set found flag
			${ToLog} `found "$0" at $ISVAR$\n`
			${ExitDo}
		${EndIf}
		IntOp $R2 $R2 + 1				;If not, continue the loop
	${Loop}
	Pop $R3
	Pop $R2
	Pop $R1
	Pop $R0
	Pop $1
	Pop $0
	!ifdef _c=false_
	!define _c=false
	!undef _c=false_
	!endif
	!ifdef _c=true_
	!define _c=true
	!undef _c=true_
	!endif
	IntCmp $ISVAR 0 `${t}` `${f}` `${t}`
!macroend ; _in

!macro _is sec flag t f
	Push `${flag}`
	Push `${sec}`
	Exch $0
	Exch
	Exch $1
	Call isfun
	Pop $ISVAR
	Pop $1
	Pop $0
	IntCmp $ISVAR 0 `${f}` `${t}` `${t}`
!macroend ; _is
Function isfun
	!insertmacro SectionFlagIsSet $0 $1 true false
	true:
	Push 1
	Goto done
	false:
	Push 0
	done:
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

!define MDSPLUS_DIR		"$INSTDIR"
!define MDS_PATH		"$INSTDIR\tdi"
!define MDS_PYDEVICE_PATH	"$INSTDIR\pydevices"
!define PYTHONPATH		"$INSTDIR\python"
!define main_path		"$INSTDIR\trees"
!define subtree_path		"$INSTDIR\trees\subtree"

LicenseData "MDSplus-License.rtf"
Page license
Page directory
Page components /ENABLECANCEL
Page instfiles

${UnStrTrimNewLines} 

!macro _UserIs var type t f
	UserInfo::GetAccountType
	Pop ${var}
	StrCmp `${var}` `${type}` `${t}` `${f}`
!macroend ; _UserIs

!macro WriteKeyStr key name value
	Push $R0
	${If} $R0 UserIs admin
		Pop $R0
		WriteRegStr HKLM `${key}` `${name}` `${value}`
	${Else}
		Pop $R0
		WriteRegStr HKCU `${key}` `${name}` `${value}`
	${EndIf}
!macroend ; WriteKeyStr
!define WriteKeyStr '!insertmacro "WriteKeyStr"'

!macro WriteKeyDWORD key name value
	Push $R0
	${If} $R0 UserIs admin
		Pop $R0
		WriteRegDWORD HKLM `${key}` `${name}` `${value}`
	${Else}
		Pop $R0
		WriteRegDWORD HKCU `${key}` `${name}` `${value}`
	${EndIf}
!macroend ; WriteKeyDWORD
!define WriteKeyDWORD '!insertmacro "WriteKeyDWORD"'

!macro DeleteKey key
	Push $R0
	${If} $R0 UserIs admin
		Pop $R0
		DeleteRegKey HKLM `${key}`
	${Else}
		Pop $R0
		DeleteRegKey HKCU `${key}`
	${EndIf}
!macroend ; DeleteKey
!define DeleteKey '!insertmacro "DeleteKey"'

!macro DeleteEnv name
	${ToLog} `DEL "${name}"$\n`
	Push $R0
	${If} $R0 UserIs admin
		Pop $R0
		DeleteRegValue HKLM "${ENVREG_ALL}" `${name}`
	${Else}
		Pop $R0
		DeleteRegValue HKCU "${ENVREG_USR}" `${name}`
	${EndIf}
!macroend ; DeleteEnv
!define DeleteEnv '!insertmacro "DeleteEnv"'

!macro WriteEnv name value
	${ToLog} `SET "${name}"="${value}"$\n`
	Push $R0
	${If} $R0 UserIs admin
		Pop $R0
		WriteRegStr HKLM "${ENVREG_ALL}" `${name}` `${value}`
	${Else}
		Pop $R0
		WriteRegStr HKCU "${ENVREG_USR}" `${name}` `${value}`
	${EndIf}
!macroend ; WriteEnv
!define WriteEnv '!insertmacro "WriteEnv"'

!macro ReadEnv var name
	${If} ${var} UserIs admin
		ReadRegStr ${var} HKLM "${ENVREG_ALL}" `${name}`
	${Else}
		ReadRegStr ${var} HKCU "${ENVREG_USR}" `${name}`
	${EndIf}
	${ToLog} `GET "${name}"="${var}"$\n`
!macroend ; ReadEnv
!define ReadEnv '!insertmacro "ReadEnv"'

!macro FileLowerExt pathin extin pathout extout
	; exchange ok because callers dont use vars
	Push ${pathout}
	Exch $0
	Push ${extin}
	Exch $1
	Push ${extout}
	Exch $2
	SetOutPath "${pathout}"
	File "${pathin}/*${extin}"
	Call FileLowerExt
	Pop $2
	Pop $1
	Pop $0
!macroend ; FileLowerExt
Function FileLowerExt ; path extin extout
	Push $R0		; find handle
	Push $R1		; source file name
	Push $R2
	StrLen $R2 $1
	IntOp $R2 0 - $R2	; - len of extin
	FindFirst $R0 $R1 "$0\*$1"
	${Do}
		${IfThen} $R1 == "" ${|} ${ExitDo} ${|}
		StrCpy $1 $R1 $R2	; remove 'extin'
		System::Call "User32::CharLowerA(t r1 r1)t"
		Rename "$0\$R1" "$0\$1$2"
		FindNext $R0 $R1
	${Loop}
	FindClose $R0
	Pop $R2
	Pop $R1
	Pop $R0
FunctionEnd ; FileLowerExt
!define FileLowerExt '!insertmacro "FileLowerExt"'

!macro AddToEnv name value
	Push `${value}`
	Push `${name}`
	Exch $0
	Exch
	Exch $1
	Call AddToEnv
	Pop $1
	Pop $0
!macroend ; AddToEnv
Function AddToEnv ; name value
	Push $R0
	${ReadEnv} $R0 `$0`
	${If} `$R0` == ""
		${WriteEnv} `$0` `$1`
	${ElseIfNot} `;$1;` in `;$R0;`
		Push $R1
		StrCpy $R1 $R0 1 -1
		${If} `$R1` == ";" ; preserve tailing ;
			${WriteEnv} `$0` `$R0$1;`
		${Else}
			${WriteEnv} `$0` `$R0;$1`
		${EndIf}
		Pop $R1
	${EndIf}
	Pop $R0
FunctionEnd ; AddToEnv
!define AddToEnv '!insertmacro "AddToEnv"'

!macro RemoveFromEnv name value
	Push ${value}
	Push ${name}
	Exch $0
	Exch
	Exch $1
	Call un.RemoveFromEnv
	Pop $1
	Pop $0
!macroend ; RemoveFromEnv
Function un.RemoveFromEnv ; name value
	Push $R0
	${ReadEnv} $R0 `$0`
	${If}   `$R0` == ""
	${OrIf} `$R0` == `$1`
	${OrIf} `$R0` == `$1;`
		${DeleteEnv} `$0`
	${Else}
		Push $R1
		Push $R2
		Push $R3
		StrLen $R1 $1
		${If} `;$1;` in `;$R0;`
			${Do}
				${ToLog} `$1 found $ISVAR$\n`
				${If} $ISVAR == 0 ; remove <value>;
					StrLen $R2 $R0
					IntOp  $ISVAR $R1 + 1
					StrCpy $R0 $R0 $R2 $ISVAR	; post
				${Else} ; remove ;<value>
					IntOp  $R2 $ISVAR - 1
					StrCpy $R3 $R0 $R2		; pre
					IntOp  $ISVAR $R1 + $ISVAR	; off post
					StrLen $R2 $R0
					StrCpy $R0 $R0 $R2 $ISVAR	; post
					StrCpy $R0 `$R3$R0`		; strcat
				${EndIf}
				${IfNotThen} `;$1;` in `;$R0;` ${|} ${ExitDo} ${|}
			${Loop}
			${WriteEnv} `$0` `$R0`
		${EndIf}
		Pop $R3
		Pop $R2
		Pop $R1
	${EndIf}
	Pop $R0
FunctionEnd ; RemoveFromEnv
!define RemoveFromEnv '!insertmacro "RemoveFromEnv"'

!macro GetBinDir var
	${If} ${var} UserIs admin
		StrCpy ${var} "$SYSDIR"
	${ElseIf} ${RunningX64}
		StrCpy ${var} "$INSTDIR\bin_x86_64"
	${Else}
		StrCpy ${var} "$INSTDIR\bin_x86"
	${EndIf}
!macroend
!define GetBinDir '!insertmacro "GetBinDir"'

Function install_core_pre
	Push $R0
	SetOutPath "$INSTDIR"
	File "/oname=ChangeLog.rtf" ChangeLog
	File ${srcdir}/mdsplus.ico
	File ${srcdir}/MDSplus-License.rtf
	writeUninstaller "$INSTDIR\uninstall.exe"
	${AddToEnv} MDS_PATH	"${MDS_PATH}"
	${WriteEnv} MDSPLUS_DIR	"${MDSPLUS_DIR}"
	SetOutPath "$INSTDIR\tdi"
	;File /r /x local /x MitDevices /x RfxDevices /x KbsiDevices /x d3d tdi/*
	SetOutPath "$INSTDIR\xml"
	;File /r xml\*
	Pop $R0
FunctionEnd

Function install_core_post
	Push $R0
	${If} $R0 UserIs admin
		CreateDirectory "$SMPROGRAMS\MDSplus${FLAVOR}\DataServer"
		CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\DataServer\Install mdsip action server on port 8100.lnk" "$SYSDIR\mdsip_service.exe" "-i -s -p 8100 -h $\"C:\mdsip.hosts$\""
		CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\DataServer\Install mdsip data server on port 8000.lnk" "$SYSDIR\mdsip_service.exe" "-i -p 8000 -h $\"C:\mdsip.hosts$\""
		CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\DataServer\Remove mdsip server on port 8100.lnk" "$SYSDIR\mdsip_service.exe" "-r -p 8100"
		CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\DataServer\Remove mdsip server on port 8000.lnk" "$SYSDIR\mdsip_service.exe" "-r -p 8000"
	${EndIf}
	${GetBinDir} $R0
	CreateDirectory "$SMPROGRAMS\MDSplus${FLAVOR}"
	CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\Tdi.lnk" "$R0\tditest.exe" "" "$R0\icons.exe" 0
	CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\TCL.lnk" '"$R0\mdsdcl"' '-prep "set command tcl"' "$R0\icons.exe" 1
	CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\View ChangeLog.lnk" "$INSTDIR\ChangeLog.rtf"

	SetOutPath "\"
	SetOverWrite off
	File etc\mdsip.hosts
	SetOverWrite on

	# Registry information for add/remove programs
	${WriteKeyStr} "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "DisplayName" "MDSplus${FLAVOR}"
	${WriteKeyStr} "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "UninstallString" "$INSTDIR\uninstall.exe"
	${WriteKeyStr} "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "QuietUninstallString" "$INSTDIR\uninstall.exe /S"
	${WriteKeyStr} "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "InstallLocation" "$INSTDIR"
	${WriteKeyStr} "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "DisplayIcon" "INSTDIR\mdsplus.ico"
	${WriteKeyStr} "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "Publisher" "MDSplus Collaboratory"
	${WriteKeyStr} "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "HelpLink" "${HELPURL}"
	${WriteKeyStr} "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "InstallSource" "https://github.com/MDSplus/mdsplus/archive/${BRANCH}_release-${MAJOR}.${MINOR}-${RELEASE}.zip"
	${WriteKeyStr} "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "URLUpdateInfo" "${UPDATEURL}"
	${WriteKeyStr} "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "URLInfoAbout" "${ABOUTURL}"
	${WriteKeyStr} "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "DisplayVersion" "${MAJOR}.${MINOR}.${RELEASE}"
	${WriteKeyDWORD} "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "VersionMajor" ${MAJOR}
	${WriteKeyDWORD} "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "VersionMinor" ${MINOR}
	# There is no option for modifying or repairing the install
	${WriteKeyDWORD} "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "NoModify" 1
	${WriteKeyDWORD} "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "NoRepair" 1
	# Set the INSTALLSIZE constant (!defined at the top of this script) so Add/Remove Programs can accurately report the size
	${WriteKeyDWORD} "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus" "EstimatedSize" ${INSTALLSIZE}
	Pop $R0
FunctionEnd

SectionGroup "!core"
 Section "64 bit" bin64
	Push $R0
	Push $R1
	Push $R2
	SectionIn RO
	Call install_core_pre
	SetOutPath "$INSTDIR\bin_x86_64"
	;File /x *.a /x *.lib bin_x86_64/*
	;File ${MINGWLIB64}/${PTHREADLIB}
	;File ${MINGWLIB64}/${DLLIB}
	;File ${MINGWLIB64}/${READLINELIB}
	;File ${MINGWLIB64}/${TERMCAPLIB}
	;File ${MINGWLIB64}/${GCC_STDCPP_LIB}
	;File ${MINGWLIB64}/${GCC_S_SEH_LIB}
	;File ${MINGWLIB64}/${GFORTRAN_LIB}
	;File ${MINGWLIB64}/${QUADMATH_LIB}
	;File ${MINGWLIB64}/${LIBXML2_LIB}
	;File ${MINGWLIB64}/${ICONV_LIB}
	;File ${MINGWLIB64}/${ZLIB1_LIB}
	${DisableX64FSRedirection}
	${If} $R0 UserIs admin
		FileOpen $R0 "$INSTDIR\installer.dat" w
		FindFirst $R1 $R2 "$INSTDIR\bin_x86_64\*"
		loop_64:
			StrCmp $R2 "" done_64
			FileWrite $R0 "$SYSDIR\$R2$\n"
			Delete "$SYSDIR\$R2"
			Rename "$INSTDIR\bin_x86_64\$R2" "$SYSDIR\$R2"
			FindNext $R1 $R2
			Goto loop_64
		done_64:
		FindClose $R1
		FileClose $R0
		RMDir "$INSTDIR\bin_x86_64"
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
	SetOutPath "$INSTDIR\bin_x86"
	;File /x *.a /x *.lib bin_x86/*
	;File ${MINGWLIB32}/${PTHREADLIB}
	;File ${MINGWLIB32}/${GCC_S_SJLJ_LIB}
	;File ${MINGWLIB32}/${DLLIB}
	;File ${MINGWLIB32}/${READLINELIB}
	;File ${MINGWLIB32}/${TERMCAPLIB}
	;File ${MINGWLIB32}/${GCC_STDCPP_LIB}
	;File ${MINGWLIB32}/${GFORTRAN_LIB}
	;File ${MINGWLIB32}/${QUADMATH_LIB}
	;File ${MINGWLIB32}/${LIBXML2_LIB}
	;File ${MINGWLIB32}/${ICONV_LIB}
	;File ${MINGWLIB32}/${ZLIB1_LIB}
	${IF} $R0 UserIs admin
		${If} ${RunningX64}
			StrCpy $R3 "$WINDIR\SysWOW64"
			FileOpen $R0 "$INSTDIR\installer.dat" a
			FileSeek $R0 0 END
		${Else}
			StrCpy $R3 "$SYSDIR"
			FileOpen $R0 "$INSTDIR\installer.dat" w
		${EndIf}
		FindFirst $R1 $R2 "$INSTDIR\bin_x86\*"
		loop_32:
			StrCmp $R2 "" done_32
			FileWrite $R0 "$R3\$R2$\n"
			Rename "$INSTDIR\bin_x86\$R2" "$R3\$R2"
			FindNext $R1 $R2
			Goto loop_32
		done_32:
		FindClose $R1
		FileClose $R0
		RMDir "$INSTDIR\bin_x86"
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
	${IfNot} $R0 UserIs admin
		${GetBinDir} $R0
		${AddToEnv} "PATH" "$R0"
	${EndIf}
	Pop $R0
 SectionEnd
SectionGroupEnd ; core

SectionGroup devices devices
 SectionGroup tdi
  Section KBSI
	SectionIn 2
	SetOutPath "$INSTDIR\tdi"
	;File /r tdi/KbsiDevices
  SectionEnd ; KBSI
  Section MIT
	SectionIn 2
	SetOutPath "$INSTDIR\tdi"
	;File /r tdi/MitDevices
  SectionEnd ; MIT
  Section RFX
	SectionIn 2
	SetOutPath "$INSTDIR\tdi"
	;File /r tdi/RfxDevices
  SectionEnd ; RFX
 SectionGroupEnd ; tdi
 SectionGroup "pydevices" pydevices
  Section HTS pydevices_hts
	SectionIn 2
	SetOutPath "$INSTDIR\pydevices\HtsDevices"
	;File /r pydevices/HtsDevices/*
  SectionEnd ; HTS
  Section MIT pydevices_mit
	SectionIn 2
	SetOutPath "$INSTDIR\pydevices\MitDevices"
	;File /r pydevices/MitDevices/*
	File /workspace/releasebld/64/pydevices/MitDevices/_version.py
  SectionEnd ; MIT
  Section RFX pydevices_rfx
	SectionIn 2
	SetOutPath "$INSTDIR\pydevices\RfxDevices"
	;File /r pydevices/RfxDevices/*
	File /workspace/releasebld/64/pydevices/RfxDevices/_version.py
  SectionEnd ; RFX
  Section W7X pydevices_w7x
	SectionIn 2
	SetOutPath "$INSTDIR\pydevices\W7xDevices"
	;File /r pydevices/W7xDevices/*
	File /workspace/releasebld/64/pydevices/W7xDevices/_version.py
  SectionEnd ; W7X
  Section "setup MDS_PYDEVICE_PATH" pydevpath
	SectionIn 2 RO
	${AddToEnv} "MDS_PYDEVICE_PATH" "${MDS_PYDEVICE_PATH}"
  SectionEnd ; pydevpath
 SectionGroupEnd ; pydevices
SectionGroupEnd ; devices

SectionGroup "site specifics"
 Section D3D
	SetOutPath "$INSTDIR\tdi"
	;File /r tdi/d3d
 SectionEnd ; D3D
SectionGroupEnd ; site specifics"


Section "java tools"
	SectionIn 1 2
	SetOutPath $INSTDIR\java
	;File /r java/classes
	Push $R0
	${GetBinDir} $R0
	CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\Scope.lnk" javaw '-cp "$INSTDIR\java\Classes\jScope.jar";"$INSTDIR\java\Classes" -Xmx1G jScope' "$0\icons.exe" 4 SW_SHOWMINIMIZED
	CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\Traverser.lnk"  javaw '-cp "$INSTDIR\java\Classes\jTraverser.jar" jTraverser' $0\icons.exe" 3 SW_SHOWMINIMIZED
	CreateShortCut "$SMPROGRAMS\MDSplus${FLAVOR}\Traverser2.lnk" javaw '-jar "$INSTDIR\java\Classes\jTraverser2.jar"' $0\icons.exe" 3 SW_SHOWMINIMIZED
	Pop $R0
SectionEnd ; java

SectionGroup /e "!APIs" apis
 Section "EPICS"
	SectionIn 2
	SetOutPath "$INSTDIR\epics"
	;File /r epics/*
 SectionEnd ; EPICS
 Section "IDL"
	SectionIn 2
	SetOutPath "$INSTDIR\idl"
	;File /r idl/*
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
	;File /r LabView/MDSplus/*
  SectionEnd ; LV2010
  Section "LV2012"
	SectionIn 2
	SetOutPath "$INSTDIR\LabView"
	;File /r LabView/MDSplus_LV2012
  SectionEnd ; LV2012
  Section "LV2015"
	SectionIn 2
	SetOutPath "$INSTDIR\LabView"
	;File /r LabView/MDSplus_LV2015
  SectionEnd ; LV2015
 SectionGroupEnd ; LabView
 Section "MATLAB"
	SectionIn 2
	SetOutPath "$INSTDIR\matlab"
	;File /r matlab/*
 SectionEnd ; MATLAB
 SectionGroup /e "!python" python
  Section "MDSplus package" python_cp
	SectionIn 1 2
	SetOutPath "$INSTDIR\python\MDSplus"
	;File /r /x makedoc.sh mdsobjects/python/*
	File /workspace/releasebld/64/mdsobjects/python/_version.py
  SectionEnd ; python_cp
  Section "add to PYTHONPATH" python_pp
	SectionIn 1 2
	${AddToEnv} "PYTHONPATH" "${PYTHONPATH}"
  SectionEnd ; python_pp
  Section "run 'python setup.py install'" python_su
	SetOutPath "$INSTDIR\python\MDSplus"
	nsExec::Exec /OEM /TIMEOUT=10000 "python setup.py install"
	Exch $R0
	Pop  $R0
  SectionEnd ; python_su
 SectionGroupEnd ; python
SectionGroupEnd ; APIs

SectionGroup development
 Section headers
	SectionIn 2
	SetOutPath "$INSTDIR\include"
	;File /r include/*
 SectionEnd ; headers
 SectionGroup devtools
  Section MinGW
	SectionIn 2
	${If} ${RunningX64}
		${FileLowerExt} bin_x86_64 .dll.a "$INSTDIR\devtools\mingw\lib64" .lib
	${EndIf}
	${FileLowerExt} bin_x86 .dll.a "$INSTDIR\devtools\mingw\lib32" .lib
  SectionEnd ; MinGW
  Section "Visual Studio"
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
	;File /r trees/*
SectionEnd

Function .onSelChange
	; pydevices depend on python
	${If}   $0 == ${apis}
	${OrIf} $0 == ${python}
	${OrIf} $0 == ${python_cp}
		${IfNot}    $0 is ${SF_SELECTED}
		${AndIfNot} $0 is ${SF_PSELECTED}
			${UnselectSection} ${pydevices}
			${UnselectSection} ${python_pp}
			${UnselectSection} ${python_su}
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
	${ElseIf} $0 == ${python_pp}
	${ElseIf} $0 == ${python_su}
		${If} $0 is ${SF_SELECTED}
			${SelectSection}    ${python_cp}
		${EndIf}
	${EndIf}
FunctionEnd

Function .onInit
	Push $R0
	${If} ${RunningX64}
		SectionSetInstTypes ${bin32} 2 ; include 32bit in full
		SectionSetInstTypes ${bin64} 7 ; always include 64bit
	${Else}
		${SetSectionFlag}   ${bin32} ${SF_RO}
		${SelectSection}    ${bin32}
		SectionSetInstTypes ${bin32} 7 ; always include 32bit
		SectionSetInstTypes ${bin64} 0 ; never include 64bit
	${EndIf}
	${If} $R0 UserIs admin
		SetShellVarContext all
		ReadRegStr $INSTDIR HKLM "${ENVREG_ALL}" MDSPLUS_DIR
		${If} `$INSTDIR` == ""
			${If} ${RunningX64}
				StrCpy $INSTDIR $PROGRAMFILES64\MDSplus
			${Else}
				StrCpy $INSTDIR $PROGRAMFILES32\MDSplus
			${EndIf}
		${EndIf}
		ReadRegStr $R0 HKLM ${UNINSTALL_KEY} ${UNINSTALL_VAL}
		SectionSetFlags ${appendpath} ${SF_RO}
	${Else}
		SetShellVarContext current
		ReadRegStr $INSTDIR HKCU "${ENVREG_USR}" MDSPLUS_DIR
		${If} $INSTDIR == ""
			StrCpy $INSTDIR $PROFILE\MDSplus
		${EndIf}
		ReadRegStr $R0 HKCU ${UNINSTALL_KEY} ${UNINSTALL_VAL}
		SectionSetInstTypes ${appendpath} 7 ; always include on user level
		SectionSetFlags ${appendpath} ${SF_SELECTED}
	${EndIf}
	${IfNot}  $R0 == ""
		MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION \
		"MDSplus is already installed. $\n$\nClick `OK` to remove the \
		previous version or `Cancel` to cancel this upgrade." IDOK uninst
			Abort
		; Run the uninstaller
		uninst:
		ClearErrors
  		ExecWait '$R0 _?=$INSTDIR' ;Do not copy the uninstaller to a temp file
	${EndIf}
	Pop $R0
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
functionEnd

Function un.onGUIEnd
	SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
FunctionEnd
 
Section "uninstall"
	Push $R0
	Push $R1
	Push $R2
	IfFileExists "$INSTDIR\mdsobjects\python" 0 skip_python_remove
	SetOutPath "$INSTDIR\mdsobjects\python"
	nsExec::ExecToLog /OEM /TIMEOUT=10000 'python setup.py remove'
	Pop $R0
	skip_python_remove:
	SetOutPath "$INSTDIR"
	Delete uninstall.exe
	${IF} $R0 UserIs admin
		SetShellVarContext all
		nsExec::ExecToLog /OEM /timeout=5000 '"$SYSDIR\mdsip_service.exe" "-r -p 8100"'
		Pop $R0
		nsExec::ExecToLog /OEM /timeout=5000 '"$SYSDIR\mdsip_service.exe" "-r -p 8000"'
		Pop $R0
		FileOpen $R0 "$INSTDIR\installer.dat" r
		${DisableX64FSRedirection}
		loop_u:
			FileRead $R0 $R1
			StrCmp $R1 "" done_u
			${UnStrTrimNewLines} $R2 $R1
			Delete $R2
			Goto loop_u
		done_u:
		FileClose $R0
		${EnableX64FSRedirection}
		Delete installer.dat
	${Else}
		SetShellVarContext current
		${GetBinDir} $R0
		${RemoveFromEnv}	PATH			"$R0"
	${EndIf}
	RMDir /r "$SMPROGRAMS\MDSplus${FLAVOR}"
	${DeleteEnv}		MDSPLUS_DIR
	${RemoveFromEnv}	MDS_PATH		"${MDS_PATH}"
	${RemoveFromEnv}	main_path		"${main_path}"
	${RemoveFromEnv}	subtree_path		"${subtree_path}"
	${RemoveFromEnv}	PYTHONPATH		"${PYTHONPATH}"
	${RemoveFromEnv}        MDS_PYDEVICE_PATH	"${MDS_PYDEVICE_PATH}"
	${DeleteKey} "Software\Microsoft\Windows\CurrentVersion\Uninstall\MDSplus"
	SetOutPath "$SYSDIR"
	FileOpen  $R0 "$INSTDIR\log.log" r
	StrCpy $R2 ""
	${Do}
		FileRead $R0 $R1
		${IfThen} $R1 == "" ${|} ${ExitDo} ${|}
		StrCpy $R2 "$R2$R1"
	${Loop}
	FileClose $R0
	MessageBox MB_OK $R2
	RMDir /r "$INSTDIR"
	Pop $R2
	Pop $R1
	Pop $R0
SectionEnd
