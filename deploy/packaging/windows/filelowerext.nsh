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
