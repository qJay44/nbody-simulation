@echo off
cls

Rem Build
if not exist Build\Debug mkdir Build\Debug\Run
cd Build\Debug
cmake.exe -S ..\..\ -B . -G"MinGW Makefiles" -D CMAKE_BUILD_TYPE=Debug
C:\Users\gerku\Documents\mingw64\bin\mingw32-make.exe

Rem Copy dlls
cd Run
setlocal
set SFML_BINARYS="C:\Users\gerku\Documents\SFML-2.6.1\bin"
echo n | copy /-y %SFML_BINARYS%\sfml-system-d-2.dll .
echo n | copy /-y %SFML_BINARYS%\sfml-graphics-d-2.dll .
echo n | copy /-y %SFML_BINARYS%\sfml-window-d-2.dll .
endlocal

Rem Copy resources and launch
robocopy ..\..\..\src\res res\ /E
MyProject.exe
cd ..\..\..\

