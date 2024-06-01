@echo off
cls

Rem Build
if not exist Build\Debug mkdir Build\Debug
cd Build\Debug
cmake.exe -S ..\..\ -B . -G"MinGW Makefiles" -D CMAKE_EXPORT_COMPILE_COMMANDS=ON -D CMAKE_BUILD_TYPE=Debug
C:\Users\gerku\Documents\mingw64\bin\mingw32-make.exe
cd ../../

