@echo off

SET path=D:\Work\warcraft;%path%

REM Include paths
SET GLEWIncludePath=D:\Work\libs\glew32\include
SET GLFWIncludePath=D:\Work\libs\glfw-3.2.1_x64\include
SET STBIncludePath=D:\Work\stb
SET SHLIncludePath=D:\Work\shl
SET NVGIncludePath=D:\Work\nanovg\src

REM Lib paths
SET GLEWLibPath=D:\Work\libs\glew32\lib
SET GLFWLibPath=D:\Work\libs\glfw-3.2.1_x64\lib-vc2015

REM ProfilerFlags="-pg"
REM OptimizeFlags="-O2"
REM AssemblyFlags="-g -Wa,-ahl"
SET CommonCompilerFlags=-std=c99 -Wall -Wno-misleading-indentation -x c %ProfilerFlags% %OptimizeFlags% %AssemblyFlags% -I %GLEWIncludePath% -I %GLFWIncludePath% -I %STBIncludePath% -I %SHLIncludePath% -I %NVGIncludePath%
SET CommonLinkerFlags= -L:%GLEWLibPath% -L:%GLFWLibPath% -l user32.lib -l gdi32 -l opengl32 -l glew32.lib -l libglfw3dll

IF NOT EXIST build mkdir build
pushd build

gcc %CommonCompilerFlags% %CommonLinkerFlags% ../src/war1.c -o war1.exe
popd
