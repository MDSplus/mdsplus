@echo off

set scriptversion=2015-10-23.10
set est 0

:parse
IF "%~1"=="" GOTO endparse

IF "%~1"=="--help" (
    echo "This is test driver for win execution of tests" 
    GOTO exit
  )

IF "%~1"=="--version" (
    echo %scriptversion%
    GOTO exit
  )  

IF "%~1"=="--test-name" (
    set test_name=%~2
    SHIFT  
    SHIFT
    GOTO parse
  )

IF "%~1"=="--log-file" (
  set log_file=%~2
  SHIFT  
  SHIFT
  GOTO parse
  )

IF "%~1"=="--trs-file" (
  set trs_file=%~2
  SHIFT  
  SHIFT
  GOTO parse
)

IF "%~1"=="--color-tests" (
  REM THIS OPTION IS NOT CURRENTLY USED!
  SHIFT  
  SHIFT
  GOTO parse
)

IF "%~1"=="--expect-failure" (
  REM THIS OPTION IS NOT CURRENTLY USED!
  SHIFT  
  SHIFT
  GOTO parse
)

IF "%~1"=="--enable-hard-errors" (
  REM THIS OPTION IS NOT CURRENTLY USED!
  SHIFT  
  SHIFT
  GOTO parse
)

IF "%~1"=="--" (
  REM Explicitly terminate the list of options. 
  echo terminate
  SHIFT  
)

REM Build cmd form remaining args
:cmdbld
if "%1" == "" GOTO endparse
set command=%command% %1
SHIFT
GOTO cmdbld

:endparse
REM ready for action!
echo Input  command. %*
echo Parsed command. %command%
echo Path........... %PATH%
echo .
echo .
echo .

%command%

%command% > %log_file%

                  ( set est=1   
                  set res=FAIL )
IF ERRORLEVEL 0   ( set est=0   
                  set res=PASS )
IF ERRORLEVEL 77  ( set est=77  
                  set res=SKIP )
IF ERRORLEVEL 99  ( set est=99  
                  set res=ERROR )


REM console output
echo %res%: %test_name%

REM pause


REM Register the test result, and other relevant metadata.
echo :test-result: %res%           > %trs_file%
echo :global-test-result: %res%   >> %trs_file%
echo :recheck: %recheck%          >> %trs_file%
echo :copy-in-global-log: %gcopy% >> %trs_file%


:exit

EXIT /B %est%

















