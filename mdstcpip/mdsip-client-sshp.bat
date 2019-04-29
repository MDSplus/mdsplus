@ECHO OFF
SETLOCAL
REM extract USER
REM 1.  set HOST to input in case input does not contain '@'
REM 2a. If '@' in HOST:     USER,HOST = HOST split at '@'
REM 2b. If '@' not in HOST: USER is set to %HOST% and HOST remains unchanged
REM 3.  if USER == HOST, default USER with USERNAME
SET HOST=%1
SET "USER=%HOST:@=" & SET "HOST=%"
IF "%USER%"=="%HOST%" SET USER=%USERNAME%

REM extract PORT
REM 1.  set PORT to default in case HOST does not contain ':'
REM 2a. if ':' in HOST:     HOST,PORT = HOST split at ':'
REM 2b. if ':' not in HOST: HOST is set to %HOST% and PORT remains unchanged
SET PORT=8000
SET "HOST=%HOST::=" & SET "PORT=%"

REM establish connection
ssh %USER%@%HOST% nc localhost %PORT%
