echo off
ssh /? >nul 2>&1
if %errorlevel% == 9009 goto use_plink
  ssh %1 %2
:use_plink
  plink --help >nul 2>&1
  if %errorlevel% == 9009 goto done
    plink -agent -batch %1 "/bin/sh -l -c  %2"
:done
