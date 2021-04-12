!define BINDIR32		"$INSTDIR\bin32"
!define BINDIR64		"$INSTDIR\bin64"

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
