Name   "MDSplus${FLAVOR} ${MAJOR}.${MINOR}-${RELEASE}"
OutFile ${OUTDIR}/MDSplus${FLAVOR}-${MAJOR}.${MINOR}-${RELEASE}.exe
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

!define BINDIR32		"$INSTDIR\bin32"
!define BINDIR64		"$INSTDIR\bin64"

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
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_STARTMENU "Application" $StartMenuFolder
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH
!insertmacro MUI_LANGUAGE "English"



### BEGIN DEBUG TOOLS ###
;Var LOG
;; ShowLog show an OK_BOX with $LOG if $LOG is not empty
!macro ShowLog
;	StrCmp $LOG "" +3 +1
;	MessageBox MB_OK $LOG
;	StrCpy $LOG ""
!macroend ; ToLog
!define ShowLog '!insertmacro "ShowLog"'
;; ToLog appends the passed string to $INSTDIR\log.log or $LOG
!macro ToLog line
;	Push `${line}`
;	Exch $0
;	Push $R0
;	StrCpy $LOG `$LOG$0$\n`
;	FileOpen  $R0 `$INSTDIR\log.log` a
;	FileSeek  $R0 0 END
;	FileWrite $R0 `$0$\n`
;	FileClose $R0
;	Pop $R0
;	Pop $0
!macroend ; ToLog
!define ToLog '!insertmacro "ToLog"'
### END DEBUG TOOLS ###



;; tools for If and Loop logics
!include LogicLib.nsh

;; 64bit detection and handling
!include x64.nsh

;; string manipulation; defines ${UnStrTrimNewLines}
!include StrFunc.nsh
${UnStrTrimNewLines}  ; implements ${UnStrTrimNewLines}



### BEGIN _in ###
;; looks for substring in string and stores starting pos in INPOS
;; INPOS == -1 if substring not found in string
;; ${If} "${needle}" in "${haystack}"
Var INPOS
!macro _in substr string t f
	; work around bug in LogicLib.nsh
	; these definitions need to be disables temporarely
	!ifdef _c=false
	!define _c=false_
	!undef _c=false
	!endif
	!ifdef _c=true
	!define _c=true_
	!undef _c=true
	!endif
	Push `${string}`	;; In order to process two args without
	Push `${substr}`	;; destroying the content of $0 or $1,
	Exch $0  ; substr	;; we push the args to the stack FiLo
	Exch			;; exchange 1st, swap stack, exchange 2nd.
	Exch $1  ; string	;; stack: BOTTOM <= $0, $1 = TOP
	Push $R0 ; substr_len
	Push $R1 ; max_inpos	;; last possible inpos that would fit substr
	Push $R2 ; tmpstr
	StrLen $R0 $0				; get length of substr
	StrLen $R1 $1				; get length of string
	IntOp  $R1 $R1 - $R0			; get max_inpos
	StrCpy $INPOS 0				; init search pos
	${Do}					; Loop until "substr" is found or "string" reaches its end
		${If} $INPOS > $R1 		; Check if end of "string"
			StrCpy $INPOS -1	; set INPOS to NOT_FOUND
			${ExitDo}
		${EndIf}
		StrCpy $R2 `$1` $R0 $INPOS	; Trim "tmpstr" to len of "substr"
		${If} $R2 == `$0`		; Compare "tmpStr" with "substr"
			${ExitDo}
		${EndIf}
		IntOp $INPOS $INPOS + 1		; If not, $INPOS++ and continue the loop
	${Loop}
	Pop $R2			; restore registers
	Pop $R1
	Pop $R0
	${ToLog} `IN("$0", "$1") = $INPOS`
	Pop $1
	Pop $0
	!ifdef _c=false_	; restore definitions of the LogicLib
	!define _c=false
	!undef _c=false_
	!endif
	!ifdef _c=true_
	!define _c=true
	!undef _c=true_
	!endif
	; perform the jump so it will work as If operator
	IntCmp $INPOS 0 `${t}` `${f}` `${t}`	; $INPOS>=0
!macroend ; _in
### END _in ###



;; tools to manipulate component selection used e.g. in .onSelChange
!include Sections.nsh
!define SelectSection    '!insertmacro "SelectSection"'
!define UnselectSection  '!insertmacro "UnselectSection"'
!define ClearSectionFlag '!insertmacro "ClearSectionFlag"'
!define SetSectionFlag   '!insertmacro "SetSectionFlag"'
### BEGIN _is ###
;; If operator to check whether section has a flag set
;; ${If} ${setion} is ${SF_SELECTED}
Var ISVAR
!macro _is section flag t f
	Push `${flag}`		;; In order to process two args without
	Push `${section}`	;; destroying the content of $0 or $1,
	Exch $0 ; substr	;; we push the args to the stack FiLo
	Exch			;; exchange 1st, swap stack, exchange 2nd.
	Exch $1 ; string	;; stack: BOTTOM <= $0, $1 = TOP
	Call isfun
	;${ToLog} `IS($0,$1) = $ISVAR`
	Pop $1
	Pop $0
	; perform the jump so it will work as If operator
	IntCmp $ISVAR 0 `${f}` `${t}` `${t}`	; $ISVAR != 0
!macroend ; _is
; this helper function is required to isolate the jump markers
; SectionFlagIsSet does not support relative jumps that may be passes by If
Function isfun
	!insertmacro SectionFlagIsSet $0 $1 true false
true:	StrCpy $ISVAR 1
	Goto +2		;done
false:	StrCpy $ISVAR 0
FunctionEnd
### END _is ###



### BEGIN _AllUsers ###
;; If operator to check whether execution level is of a given type e.g. admin
;; ${if} for AllUsers ?
!macro _AllUsers dummy1 dummy2 t f
	${ToLog} `InstallMode $MultiUser.InstallMode`
	StrCmp `$MultiUser.InstallMode` AllUsers `${t}` `${f}`
!macroend ; _AllUsers
### END _AllUsers ###



### BEGIN REGISTRY MANIPULATION ###
;; the root HKLM or HKCU must be a constant as it is interpreted at compiletime
;; these macros write to the corresponding root (and key) base on AllUsers admin
!macro WriteKeyStr key name value
	${If} for AllUsers ?
		WriteRegStr HKLM `${key}` `${name}` `${value}`
	${Else}
		WriteRegStr HKCU `${key}` `${name}` `${value}`
	${EndIf}
!macroend ; WriteKeyStr
!define WriteKeyStr '!insertmacro "WriteKeyStr"'
!macro WriteKeyDWORD key name value
	${If} for AllUsers ?
		WriteRegDWORD HKLM `${key}` `${name}` `${value}`
	${Else}
		WriteRegDWORD HKCU `${key}` `${name}` `${value}`
	${EndIf}
!macroend ; WriteKeyDWORD
!define WriteKeyDWORD '!insertmacro "WriteKeyDWORD"'
!macro ReadKeyStr var key name
	${If} for AllUsers ?
		ReadRegStr ${var} HKLM `${key}` `${name}`
	${Else}
		ReadRegStr ${var} HKCU `${key}` `${name}`
	${EndIf}
!macroend ; ReadKeyStr
!define ReadKeyStr '!insertmacro "ReadKeyStr"'
!macro DeleteKey key
	${If} for AllUsers ?
		DeleteRegKey HKLM `${key}`
	${Else}
		DeleteRegKey HKCU `${key}`
	${EndIf}
!macroend ; DeleteKey
!define DeleteKey '!insertmacro "DeleteKey"'


## BEGIN ENVIRONMENT MANIPULATION ##
;; The key which hold the evironment variables is different for HKLM and HKCU
!define ENVREG_ALL "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"
!define ENVREG_USR "Environment"

# BEGIN WriteEnv #
;; create or overwrite environment var
!macro WriteEnv name value
	${ToLog} `SET_ENV ${name} "${value}"`
	${If} for AllUsers ?
		WriteRegStr HKLM "${ENVREG_ALL}" `${name}` `${value}`
	${Else}
		WriteRegStr HKCU "${ENVREG_USR}" `${name}` `${value}`
	${EndIf}
!macroend ; WriteEnv
!define WriteEnv '!insertmacro "WriteEnv"'
# END WriteEnv #

# BEGIN ReadEnv #
;; read environment var into var
!macro ReadEnv var name
	${If} for AllUsers ?
		ReadRegStr ${var} HKLM "${ENVREG_ALL}" `${name}`
	${Else}
		ReadRegStr ${var} HKCU "${ENVREG_USR}" `${name}`
	${EndIf}
	${ToLog} `GET_ENV ${name} "${var}"`
!macroend ; ReadEnv
!define ReadEnv '!insertmacro "ReadEnv"'
# END ReadEnv #

# BEGIN DeleteEnv #
;; delete environment var
!macro DeleteEnv name
	${ToLog} `DEL_ENV ${name}`
	${If} for AllUsers ?
		DeleteRegValue HKLM "${ENVREG_ALL}" `${name}`
	${Else}
		DeleteRegValue HKCU "${ENVREG_USR}" `${name}`
	${EndIf}
!macroend ; DeleteEnv
!define DeleteEnv '!insertmacro "DeleteEnv"'
# END DeleteEnv #

# BEGIN AddToEnv #
;; AddToEnv creates the environment var <name> with <value> if not existent
;; Otherwise, it will check is env contains <value> given env is a ';'-separated list
;; It will only append <value> to env list if entry not found
;; It will preserve any tailing ';' if present
!macro AddToEnv name value
	Push `${value}`	;; In order to process two args without
	Push `${name}`	;; destroying the content of $0 or $1,
	Exch $0 ; name	;; we push the args to the stack FiLo
	Exch		;; exchange 1st, swap stack, exchange 2nd.
	Exch $1 ; value	;; stack: BOTTOM <= $0, $1 = TOP
	Call AddToEnv
	Pop $1
	Pop $0
!macroend ; AddToEnv
Function AddToEnv ; name value
	Push $R0
	${ReadEnv} $R0 `$0`
	${If} `$R0` == "" 		; if env did not exist or was empty
		${WriteEnv} `$0` `$1`
	${ElseIfNot} `;$1;` in `;$R0;`	; if env does not contain <value>
		Push $R1
		StrCpy $R1 $R0 1 -1	; last char of env
		${If} `$R1` == ";"			;; if env terminates on ';'
			${WriteEnv} `$0` `$R0$1;`	;; append <value> and terminate with ';'
		${Else}					;; otherwise
			${WriteEnv} `$0` `$R0;$1`	;; append separator ';' and <value>
		${EndIf}
		Pop $R1
	${EndIf}
	Pop $R0
FunctionEnd ; AddToEnv
!define AddToEnv '!insertmacro "AddToEnv"'
# END AddToEnv #

# BEGIN RemoveFromEnv #
;; reverts AddToEnv
;; checks if enironment var is present
;; if present it will check if env contains <value> given env is a ';'-separated list
;; if <value> is contained in env it will cut <value>
;; it will continue until env does not contain any more <value>
;; It will preserve any tailing ';' if present
!macro RemoveFromEnv name value
	Push `${value}`	;; In order to process two args without
	Push `${name}`	;; destroying the content of $0 or $1,
	Exch $0 ; name	;; we push the args to the stack FiLo
	Exch		;; exchange 1st, swap stack, exchange 2nd.
	Exch $1 ; value	;; stack: BOTTOM <= $0, $1 = TOP
	Call un.RemoveFromEnv
	Pop $1
	Pop $0
!macroend ; RemoveFromEnv
Function un.RemoveFromEnv ; name value
	Push $R0
	${ReadEnv} $R0 `$0`
	${If}   `$R0` == ""	; env is empty or not present
	${OrIf} `$R0` == `$1`	; env contains only <value>
	${OrIf} `$R0` == `$1;`	; env contains only <value> and a terminating ';'
		${DeleteEnv} `$0`	; then delete env (is NOP if not present)
	${Else}			; otherwise remove any occurence of <value> in env
		Push $R1 ; value_len
		Push $R2 ; env_len
		Push $R3 ; tmpstr to hold pre part
		StrLen $R1 $1
		${If} `;$1;` in `;$R0;`	;; adding ';' helps isolating <value> as element
			${Do}			; $INPOS points to start pos of found <value> in env
				StrLen $R2 $R0		; current len of env
				${If} $INPOS == 0		; remove <value> and ';' from head
					IntOp  $INPOS $R1 + 1		;; offset = value_len + 1 for ';'
					StrCpy $R0 $R0 $R2 $INPOS	;; cut head
				${Else} 			; remove ';' and <value> from env at $INPOS
					IntOp  $R3 $INPOS - 1		;; set pre len so it will exclude the ';' before <value>
					StrCpy $R3 $R0 $R3		;; store pre part in tmpstr
					IntOp  $INPOS $R1 + $INPOS	;; set post offset to $INPOS + value_len 
					StrLen $R2 $R0			;; maxlen of env
					StrCpy $R0 $R0 $R2 $INPOS	;; store post part
					StrCpy $R0 `$R3$R0`		;; strcat pre and post for new env
				${EndIf}	; update INPOS and abort loop if <value> no longer be found in env
				${IfNotThen} `;$1;` in `;$R0;` ${|} ${ExitDo} ${|}
			${Loop} ; finally update env in registry
			${WriteEnv} `$0` `$R0`
		${EndIf}
		Pop $R3
		Pop $R2
		Pop $R1
	${EndIf}
	Pop $R0
FunctionEnd ; RemoveFromEnv
!define RemoveFromEnv '!insertmacro "RemoveFromEnv"'
# END RemoveFromEnv #

## END ENVIRONMENT MANIPULATION ##


### END REGISTRY MANIPULATION ###



### BEGIN GetBinDir ###
;; stores the location of the native binaries in var
;; the localtion depends on privileges and architecture
!macro GetBinDir var
	${If} for AllUsers ?
		StrCpy ${var} "$SYSDIR"
	${ElseIf} ${RunningX64}
		StrCpy ${var} "${BINDIR64}"
	${Else}
		StrCpy ${var} "${BINDIR32}"
	${EndIf}
!macroend
!define GetBinDir '!insertmacro "GetBinDir"'
### END GetBinDir ###



### BEGIN FileLowerExt ###
;; the devtools filenames require a cast to lower case
;; and in case of the mings a change of the extension
;; this marco copies all files of extin in pathin to pathout
;; then it renames the files to the desired format
!macro FileLowerExt pathin extin pathout extout
	Push ${pathout}		;; !!! this macro expects the arguments to be constants !!!
	Exch $0			;; We can push argument and then exchange with a register
	Push ${extin}		;; Given that: $0 not used in extin
	Exch $1			;; stack: BOTTOM <= $0, $1 = TOP
	Push ${extout}		;; Given that: $0 and $1 not used in extout
	Exch $2			;; stack: BOTTOM <= $0, $1, $2 = TOP
	SetOutPath "${pathout}"	;; Given that: $0, $1, and $2 not used in pathout
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
### END FileLowerExt ###



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
		FileWrite ${logfile} "${dest}\$1$\n"
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
	SetOutPath "$INSTDIR"
	writeUninstaller "$INSTDIR\uninstall.exe"
	File icons/mdsplus.ico
;	File "/oname=ChangeLog.rtf" ChangeLog
	File MDSplus-License.rtf

	# Registry information for add/remove programs
	${WriteKeyStr} "${UNINSTALL_KEY}" "DisplayName" "MDSplus${FLAVOR}"
	${WriteKeyStr} "${UNINSTALL_KEY}" "UninstallString" "$INSTDIR\uninstall.exe /$MultiUser.InstallMode"
	${WriteKeyStr} "${UNINSTALL_KEY}" "QuietUninstallString" "$INSTDIR\uninstall.exe /S /$MultiUser.InstallMode"
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
		FileOpen $R0 "$INSTDIR\installer.dat" w
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
	${IF} for AllUsers ?
		${If} ${RunningX64}
			FileOpen $R0 "$INSTDIR\installer.dat" a
			FileSeek $R0 0 END
			${InstallFiles} "${BINDIR32}" "$WINDIR\SysWOW64" $R0
		${Else}
			FileOpen $R0 "$INSTDIR\installer.dat" w
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
	!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\jScope.lnk"      javaw '-cp "$INSTDIR\java\classes\jScope.jar";"$INSTDIR\java\Classes" -Xmx1G jScope'	"$INSTDIR\java\jscope.ico"	0 SW_SHOWMINIMIZED
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\jTraverser.lnk"  javaw '-cp "$INSTDIR\java\classes\jTraverser.jar" jTraverser'				"$INSTDIR\java\jtraverser.ico"	0 SW_SHOWMINIMIZED
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\jTraverser2.lnk" javaw '-jar "$INSTDIR\java\classes\jTraverser2.jar"'					"$INSTDIR\java\jtraverser2.ico"	0 SW_SHOWMINIMIZED
	!insertmacro MUI_STARTMENU_WRITE_END
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
  !define LVOLD_DESC "https://github.com/MDSplus/mdsplus/$\n"
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
	File /x modpython.py /x setup.py mdsobjects/python/*.py
	File /workspace/releasebld/64/mdsobjects/python/_version.py
  SectionEnd ; python_cp
  Section "tests" python_tst
	SectionIn 2
	SetOutPath "$INSTDIR\python\MDSplus"
	File /r mdsobjects/python/tests
  SectionEnd ; python_tst
  Section "WSGI" python_wsgi
	SectionIn 2
	SetOutPath "$INSTDIR\python\MDSplus"
	File /r mdsobjects/python/wsgi
  SectionEnd ; python_wsgi
  Section "glade widgets" python_wdg
	SectionIn 2
	SetOutPath "$INSTDIR\python\MDSplus"
	File /r mdsobjects/python/widgets
  SectionEnd ; python_wdg
  Section "mod_python" python_mod
	SectionIn 2
	SetOutPath "$INSTDIR\python\MDSplus"
	File mdsobjects/python/modpython.py
  SectionEnd ; python_mod
  Section "add to PYTHONPATH" python_pp
	SectionIn 1 2
	${AddToEnv} "PYTHONPATH" "${PYTHONPATH}"
  SectionEnd ; python_pp
  Section "run 'python setup.py install'" python_su
	SetOutPath "$INSTDIR\python\MDSplus"
	File mdsobjects/python/setup.py
	nsExec::Exec /OEM /TIMEOUT=10000 "python setup.py install"
	Exch $R0
	Pop  $R0
  SectionEnd ; python_su
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
		${If} $0 is ${SF_SELECTED}
			${SelectSection}    ${python_cp}
			${SelectSection}    ${python_tst}
			${SelectSection}    ${python_wdg}
			${SelectSection}    ${python_wsgi}
		${EndIf}
	${ElseIf} $0 == ${python_tst}
	${OrIf}   $0 == ${python_wdg}
	${OrIf}   $0 == ${python_wsgi}
		${If} $0 is ${SF_SELECTED}
			${SelectSection}    ${python_cp}
		${Else}
			${UnselectSection}  ${python_su}
		${EndIf}
	${EndIf}
FunctionEnd
### END SECTION LOGIC ###



### BEGIN INSTALLER ###
Function Init
	${ToLog} "BEGIN INIT"
	Push $R0
	${ReadkeyStr} $R0 ${UNINSTALL_KEY} ${UNINSTALL_VAL}
	${ReadEnv} $INSTDIR MDSPLUS_DIR
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
	SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
	${ShowLog}
FunctionEnd
### END INSTALLER ###



### BEGIN UNINSTALLER ###
Function un.onInit
	!insertmacro MULTIUSER_UNINIT
functionEnd ; un.onInit

Function un.onGUIEnd
	SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
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
	Delete uninstall.exe
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
	RMDir /r "$INSTDIR"
	Pop $R2
	Pop $R1
	Pop $R0
	${ShowLog}
SectionEnd
### END UNINSTALLER ###
