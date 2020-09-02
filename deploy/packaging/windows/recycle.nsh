;; https://nsis.sourceforge.io/mediawiki/index.php?title=Send_to_Recycle_Bin

!define FO_DELETE 0x3
!define FOF_SILENT 0x4
!define FOF_NOCONFIRMATION 0x10
!define FOF_ALLOWUNDO 0x40
!define FOF_NOERRORUI 0x400
!define FOF_WANTNUKEWARNING 0x4000
!define FOF_ALL FOF_ALLOWUNDO|FOF_SILENT|FOF_NOCONFIRMATION|FOF_NOERRORUI|FOF_WANTNUKEWARNING

!macro SHFileOperation_Recycle filespec outvarwinerr
Push "${filespec}"
Exch $0
Push $1
System::Call '*(i$hwndparent,i${FO_DELETE},i,i,i${FOF_ALL},i0,i0,i0,&t${NSIS_MAX_STRLEN} r0,&t1 0)i.r0'
IntOp $1 $0 + 32
System::Call '*$0(i,i,ir1,i,i,i,i,i)'
Pop $1
System::Call 'shell32::SHFileOperation(ir0)i.s'
System::Free $0
Exch
Pop $0
!if "${outvarwinerr}" != "s"
	Pop ${outvarwinerr}
!endif
!macroend ; SHFileOperation_Recycle
!define Recycle '!insertmacro "SHFileOperation_Recycle"'
