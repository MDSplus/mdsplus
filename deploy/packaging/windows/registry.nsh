;; If operator to check whether execution level is of a given type e.g. admin
;; ${if} for AllUsers ?
!macro _AllUsers dummy1 dummy2 t f
	${ToLog} `InstallMode $MultiUser.InstallMode`
	StrCmp `$MultiUser.InstallMode` AllUsers `${t}` `${f}`
!macroend ; _AllUsers

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
	${If} `$R0` == ""		; if env did not exist or was empty
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
				${Else}				; remove ';' and <value> from env at $INPOS
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
