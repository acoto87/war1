@echo off

SET path=D:\Work\warcraft;%path%

SET GLEWIncludePath=D:\Work\libs\glew32\include
SET GLEWLibPath=D:\Work\libs\glew32\lib

SET GLFWIncludePath=D:\Work\libs\glfw-3.2.1_x64\include
SET GLFWLibPath=D:\Work\libs\glfw-3.2.1_x64\lib-mingw-w64

SET GLMIncludePath=D:\Work\cglm\include\cglm

SET STBIncludePath=D:\Work\stb

SET CommonCompilerFlags=-O -Wall -x c -I %GLEWIncludePath% -I %GLFWIncludePath% -I %GLMIncludePath% -I %STBIncludePath%
SET CommonLinkerFlags= -L:%GLEWLibPath% -L:%GLFWLibPath% -l user32.lib -l gdi32 -l opengl32 -l glew32.lib -l libglfw3dll

IF NOT EXIST build mkdir build
pushd build

gcc %CommonCompilerFlags% %CommonLinkerFlags% ../src/war1.c -o war1.exe
popd
