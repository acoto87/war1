@echo off
 
REM Include paths
SET IncludePath=..\deps\include
 
REM Lib paths
SET LibPath=..\deps\lib
 
REM ProfilerFlags="-pg"
REM OptimizeFlags="-O2"
REM AssemblyFlags="-g -Wa,-ahl"
SET CommonCompilerFlags=-std=c99 -Wall -Wno-misleading-indentation -x c %ProfilerFlags% %OptimizeFlags% %AssemblyFlags% -I %IncludePath%
SET CommonLinkerFlags=-L %LibPath% -l glfw3 -l opengl32 
 
IF NOT EXIST build mkdir build
pushd build
 
gcc %CommonCompilerFlags% %CommonLinkerFlags% ..\src\war1.c ..\deps\include\glad\glad.c -o war1.exe
popd
