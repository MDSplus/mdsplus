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
		${If} $INPOS > $R1		; Check if end of "string"
			StrCpy $INPOS -1	; set INPOS to NOT_FOUND
			${ExitDo}
		${EndIf}
		StrCpy $R2 `$1` $R0 $INPOS	; Trim "tmpstr" to len of "substr"
		${If} `$R2` == `$0`		; Compare "tmpStr" with "substr"
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
