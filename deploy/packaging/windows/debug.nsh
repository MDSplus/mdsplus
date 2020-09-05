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
;	StrCpy $LOG `$LOG$0$\r$\n`
;	FileOpen  $R0 `$INSTDIR\log.log` a
;	FileSeek  $R0 0 END
;	FileWrite $R0 `$0$\r$\n`
;	FileClose $R0
;	Pop $R0
;	Pop $0
!macroend ; ToLog
!define ToLog '!insertmacro "ToLog"'
### END DEBUG TOOLS ###
