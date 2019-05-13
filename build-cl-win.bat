@echo off

pushd .
call "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64
popd

REM Include paths
SET IncludePath=..\deps\include

REM Lib paths
SET LibPath=..\deps\lib\win64-msvc

REM Flags
REM SET DebugFlags==__DEBUG__

SET CommonCompilerFlags=-Od -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -FC -Z7 -TC -I %IncludePath% 
SET CommonLinkerFlags=-incremental:no -opt:ref -out:war1.exe -pdb:war1.pdb -libpath:%LibPath% user32.lib gdi32.lib opengl32.lib glfw3dll.lib

IF NOT EXIST build mkdir build
pushd build

REM 64-bit build

REM Language switches
REM 	-TC compile all files as .c

REM Optimization switches 
REM 	-O1 maximum optimizations (favor space)
REM 	-O2 maximum optimizations (favor speed)
REM 	-Od disable optimizations (default)
REM 	-Oi[-] enable intrinsic functions

REM Enable all warnings
REM     -Wall

REM Disable all warnings
REM 	-w

REM To show all the include files 
REM 	-showIncludes

cl %CommonCompilerFlags% ..\src\war1.c ..\deps\include\glad\glad.c /link %CommonLinkerFlags%
popd
