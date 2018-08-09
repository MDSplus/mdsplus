@ECHO OFF
REM GOTO:use_plink

REM test if OpenSSH is available
ssh 1>NUL 2>NUL
IF %ERRORLEVEL% == 9009 GOTO:use_plink

:use_ssh
ssh %1 "/bin/sh -l -c %2"
GOTO:done

:use_plink
REM add current user if not specified in %1
SETLOCAL EnableDelayedExpansion
SET SERVER=%1
IF "%SERVER:@=%" == "%SERVER%" SET SERVER=%USERNAME%@%SERVER%

REM if the server does not support publickey define PASSWD
IF DEFINED PASSWD  GOTO:use_plink_pw
REM allows the specification of a key file (*.ppk) for authentication
IF DEFINED SSH2RSA GOTO:use_plink_ssh2rsa
REM otherwise, use agent
GOTO:use_plink_agent

:use_plink_pw
plink -pw %PASSWD% -batch %SERVER% "/bin/sh -l -c %2"
GOTO:done

:use_plink_ssh2rsa
plink -i %SSH2RSA% -batch %SERVER% "/bin/sh -l -c %2"
GOTO:done

:use_plink_agent
plink -agent -A -batch %SERVER% "/bin/sh -l -c %2"
GOTO:done

:done
