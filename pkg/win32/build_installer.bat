@echo off
cd /d "%~dp0"
echo Parameters passed:
echo %1
echo %2
echo %3
echo %4
make.bat %1 %2 "%3" %4
