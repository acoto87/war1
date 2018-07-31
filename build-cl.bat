@echo off

pushd .
call "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
popd
SET path=D:\Work\warcraft;%path%

SET GLEWIncludePath=D:\Work\libs\glew32\include
SET GLEWLibPath=D:\Work\libs\glew32\lib

SET GLFWIncludePath=D:\Work\libs\glfw-3.2.1_x64\include
SET GLFWLibPath=D:\Work\libs\glfw-3.2.1_x64\lib-vc2015

SET GLMIncludePath=D:\Work\cglm\include\cglm

SET STBIncludePath=D:\Work\stb

SET CommonCompilerFlags=-Od -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -FC -Z7 -TC -I %GLEWIncludePath% -I %GLFWIncludePath% -I %GLMIncludePath% -I %STBIncludePath% -D _DEBUG
SET CommonLinkerFlags= -incremental:no -opt:ref -out:war1.exe -pdb:war1.pdb -libpath:%GLEWLibPath% -libpath:%GLFWLibPath% user32.lib gdi32.lib opengl32.lib glew32.lib glfw3dll.lib

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

REM Disable all warnings
REM 	-w

REM To show all the include files 
REM 	-showIncludes

cl %CommonCompilerFlags% ..\src\war1.c /link %CommonLinkerFlags%
popd
