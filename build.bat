@echo off
rem Blastar C++ - one-click build with the MinGW GCC 15.2 already on this machine.
rem Usage: build.bat                 -> builds src\main.cpp into build\blastar.exe
rem        build.bat tools\smoke.cpp -> builds tools\smoke.cpp into build\smoke.exe
setlocal
set "CXX=C:\mingw64\bin\g++.exe"
set "ROOT=%~dp0"
set "RAYLIB=%ROOT%vendor\raylib"
set "SRC=%~1"
set "NAME=%~n1"

if "%SRC%"=="" if exist "%ROOT%src\main.cpp" (set "SRC=%ROOT%src\main.cpp" & set "NAME=blastar")
if "%SRC%"=="" (set "SRC=%ROOT%tools\smoke.cpp" & set "NAME=smoke")

if not exist "%ROOT%build" mkdir "%ROOT%build"
rem Keep compiler temp files inside the project so a locked-down %TEMP% cannot break the build.
if not exist "%ROOT%build\tmp" mkdir "%ROOT%build\tmp"
set "TMP=%ROOT%build\tmp"
set "TEMP=%ROOT%build\tmp"

if not exist "%CXX%" (
  echo BUILD FAILED: compiler not found: %CXX%
  exit /b 1
)

"%CXX%" -O2 -std=c++17 "%SRC%" ^
  -I"%RAYLIB%\include" -L"%RAYLIB%\lib" ^
  -lraylib -lopengl32 -lgdi32 -lwinmm ^
  -o "%ROOT%build\%NAME%.exe"
if errorlevel 1 (
  echo BUILD FAILED
  exit /b 1
)
echo OK: %ROOT%build\%NAME%.exe
