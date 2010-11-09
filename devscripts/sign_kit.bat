@echo off
rem define environement variable "mds_sign_it" to be a command to sign your installation kit.
rem for example:
rem
rem C:\> set mds_sign_it="\Program Files\Microsoft SDKs\Windows\v7.1\Bin\signtool.exe" /f C:\certs\twfdoecert.p12 /p mypassword
rem
rem The signtool.exe is found in the Windows SDK kit from Microsoft.
rem 
if defined mds_sign_it (%mds_sign_it% %*) else echo "mds_sign_it not defined. Project not signed"