@echo off
cd /d "%~dp0"
echo Parameters passed:
echo %1
echo %2
echo %3
echo %4

REM Determine the correct configuration based on what's available
if exist "..\..\pgadmin\Debug\pgAdmin3.exe" (
    set CONFIG=Debug
    echo Found Debug build, using Debug configuration
) else if exist "..\..\pgadmin\Release\pgAdmin3.exe" (
    set CONFIG=Release
    echo Found Release build, using Release configuration
) else (
    set CONFIG=Release
    echo No build found, defaulting to Release configuration
)

make.bat %1 %2 "%3" %CONFIG%
