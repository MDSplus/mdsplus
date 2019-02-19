@ECHO OFF
CALL :makewinlink MDSplus
CALL :makewinlink MitDevices
CALL :makewinlink RfxDevices
CALL :makewinlink W7xDevices
GOTO EOF
:makewinlink
IF NOT EXIST "%1~" (
  IF NOT EXIST "%1" GOTO:EOF
  RENAME "%1" "%1~"
)
SET /P link=<"%1~"
MKLINK /D "%1" "%link:/=\%"
:EOF