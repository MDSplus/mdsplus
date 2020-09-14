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
