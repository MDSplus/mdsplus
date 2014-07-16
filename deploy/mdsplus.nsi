!define VERSION "6.2.8"
Name "MDSplus ${VERSION}"
InstallDir $PROGRAMFILES64\MDSplus
InstallDirRegKey HKLM Software\MIT\MDSplus InstallLocation
OutFile MDSplus-${VERSION}.exe
RequestExecutionLevel admin 

!define ENVREG "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"

Section
SetOutPath "$INSTDIR"
File .\ChangeLog
File "MDSplus-License.txt"
File /r "tdi"
WriteRegStr HKLM "${ENVREG}" MDS_PATH "$INSTDIR\tdi"
ReadRegStr $0 HKLM  "${ENVREG}" "Path"
WriteRegStr HKLM "${ENVREG}" MDSPLUSDIR "$INSTDIR"
File /r /x *.a bin64
File "/oname=$INSTDIR\bin64\libwinpthread-1.dll" /usr/x86_64-w64-mingw32/sys-root/mingw/bin/libwinpthread-1.dll
File /r /x *.a bin32
File "/oname=$INSTDIR\bin32\libwinpthread-l.dll" /usr/i686-w64-mingw32/sys-root/mingw/bin/libwinpthread-1.dll
CreateDirectory "$SMPROGRAMS\MDSplus"
CreateShortCut "$SMPROGRAMS\MDSplus\tditest(64).lnk" "$INSTDIR\bin64\tditest.exe"
CreateShortCut "$SMPROGRAMS\MDSplus\tditest(32).lnk" "$INSTDIR\bin32\tditest.exe"
CreateShortCut "$SMPROGRAMS\MDSplus\TCL.lnk" "$INSTDIR\bin64\mdstcl.bat"
ExecWait '"$INSTDIR\bin64\WinInstall.exe"/Install' $0
DetailPrint "WinInstall returned $0"
SectionEnd

Section "Sample Trees"
SetOutPath "$INSTDIR"
WriteRegStr HKLM "${ENVREG}" "main_path" "$INSTDIR\trees"
WriteRegStr HKLM "${ENVREG}" "subtree_path" "$INSTDIR\trees\subtree"
File /r "trees"
SectionEnd

Section "IDL"
SetOutPath "$INSTDIR"
File /r idl
SectionEnd

Section "JAVA"
SetOutPath "$INSTDIR"
File /r "java"
SectionEnd

Section LabView
SetOutPath "$INSTDIR"
File /r "LabView"
SectionEnd

Section EPICS
SetOutPath "$INSTDIR"
File /r epics
SectionEnd

Section "MATLAB"
SetOutPath "$INSTDIR"
File /r "matlab"
SectionEnd

Section "PYTHON"
SetOutPath "$INSTDIR"
File /r "mdsobjects/python"
SectionEnd

;Page license
Page directory
Page components /ENABLECANCEL
Page instfiles

