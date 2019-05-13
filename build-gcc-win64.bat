@echo off

REM Output path (relative to build bat file)
SET OutputPath=.\build\win64

REM Assets path (relative to output folder)
SET AssetsPath=..\..\assets

REM Include path (relative to output folder)
SET IncludePath=..\..\deps\include
 
REM Lib path (relative to output folder)
SET LibPath=..\..\deps\lib\win64

REM Compiler flags
REM ProfilerFlags="-pg"
REM OptimizeFlags="-O2"
REM AssemblyFlags="-g -Wa,-ahl"
SET CommonCompilerFlags=-std=c99 -Wall -Wno-misleading-indentation -x c %ProfilerFlags% %OptimizeFlags% %AssemblyFlags% -I %IncludePath%
SET CommonLinkerFlags=-L %LibPath% -l glfw3 -l opengl32 

REM Create output path if doesn't exists
IF NOT EXIST %OutputPath% MKDIR %OutputPath%
PUSHD %OutputPath%

REM Empty the build folder
DEL /Q *

REM Compile the project
gcc %CommonCompilerFlags% %CommonLinkerFlags% ..\..\src\war1.c ..\..\deps\include\glad\glad.c -o war1.exe
if %ERRORLEVEL% == 0 (
    REM Copy assets
	COPY %AssetsPath%\* .\

	REM Copy dependencies
	COPY %LibPath%\glfw3.dll .\glfw3.dll
)

POPD
