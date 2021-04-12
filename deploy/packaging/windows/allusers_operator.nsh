;; If operator to check whether execution level is of a given type e.g. admin
;; ${if} for AllUsers ?
!macro _AllUsers dummy1 dummy2 t f
	${ToLog} `InstallMode $MultiUser.InstallMode`
	StrCmp `$MultiUser.InstallMode` AllUsers `${t}` `${f}`
!macroend ; _AllUsers
