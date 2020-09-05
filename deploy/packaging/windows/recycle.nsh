;; https://nsis.sourceforge.io/mediawiki/index.php?title=Send_to_Recycle_Bin

!define FO_DELETE	0x3
!define FOF_SILENT		0x4
!define FOF_NOCONFIRMATION	0x10
!define FOF_ALLOWUNDO		0x40
!define FOF_NOERRORUI		0x400
!define FOF_WANTNUKEWARNING	0x4000
!define FOF_RECYCLE	FOF_SILENT|FOF_NOCONFIRMATION|FOF_NOERRORUI|FOF_WANTNUKEWARNING|FOF_ALLOWUNDO
!define FOF_DELETE	FOF_SILENT|FOF_NOCONFIRMATION|FOF_NOERRORUI|FOF_WANTNUKEWARNING

!macro SHFileOperation_Delete op outvar filespec
Push `${filespec}`
Exch $0
Push $1
Push $2
StrLen $2 $0
System::Call `*(&t$2 r0,i0)p.r2` ; terminate with \0\0
System::Call `*(i$hwndparent,i${FO_DELETE},ir2,i0,i${op},i0,i0,i0)i.r1` ; SHFILEOPSTRUCTA
System::Call `shell32::SHFileOperation(ir1)i.r0`
System::Free $2
System::Free $1
Pop $2
Pop $1
Exch $0
Pop ${outvar}
!macroend ; SHFileOperation_Delete
!define Recycle '!insertmacro "SHFileOperation_Delete" ${FOF_RECYCLE}'
!define Delete  '!insertmacro "SHFileOperation_Delete" ${FOF_DELETE}'

!macro TryRecycle filespec
Push $R0
${Recycle} $R0 `${filespec}`
${If} $R0 == 120
	${Delete} $R0 `${filespec}`
${EndIf}
Pop $R0
!macroend ; TryRecycle
!define TryRecycle '!insertmacro "TryRecycle"'
