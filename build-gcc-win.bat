@echo off
 
REM Include paths
SET IncludePath=..\deps\include
 
REM Lib paths
SET LibPath=..\deps\lib\win32
 
REM ProfilerFlags="-pg"
REM OptimizeFlags="-O2"
REM AssemblyFlags="-g -Wa,-ahl"
SET CommonCompilerFlags=-std=c99 -Wall -Wno-misleading-indentation -x c %ProfilerFlags% %OptimizeFlags% %AssemblyFlags% -I %IncludePath%
SET CommonLinkerFlags=-L %LibPath% -l glfw3 -l opengl32 
 
IF NOT EXIST build mkdir build
pushd build

del war1.exe

gcc %CommonCompilerFlags% %CommonLinkerFlags% ..\src\war1.c ..\deps\include\glad\glad.c -o war1.exe

copy %LibPath%\glfw3.dll .\glfw3.dll

popd
