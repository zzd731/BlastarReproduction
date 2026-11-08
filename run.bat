@echo off
rem Blastar - build and launch. Everything is written to build\run.log,
rem and the window stays open on error so the message can be read.
setlocal
cd /d "%~dp0"
set "LOG=%~dp0build\run.log"

if not exist "%~dp0build" mkdir "%~dp0build"
echo ===== build ===== > "%LOG%"
call "%~dp0build.bat" >> "%LOG%" 2>&1
if errorlevel 1 goto fail
if not exist "%~dp0build\blastar.exe" goto fail

echo ===== run ===== >> "%LOG%"
"%~dp0build\blastar.exe" >> "%LOG%" 2>&1
set "RC=%errorlevel%"
echo ===== exit %RC% =====>> "%LOG%"
if not "%RC%"=="0" goto fail
exit /b 0

:fail
echo.
echo ------------------- run.log -------------------
type "%LOG%"
echo ----------------------------------------------
echo  log saved to: %LOG%
echo.
pause
exit /b 1
