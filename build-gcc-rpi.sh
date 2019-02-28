#!/bin/sh

# Include paths

GladIncludePath="/home/pi/Projects/glad-gles-2.0/include"
GLFWIncludePath="/usr/local/include"
STBIncludePath="/home/pi/Projects/stb"
SHLIncludePath="/home/pi/Projects/shl"
NANOVGIncludePath="/home/pi/Projects/nanovg/src"

# Lib paths
GLFWLibPath="/usr/local/lib"

GladSrcPath="/home/pi/Projects/glad-gles-2.0/src/glad.c"

# ProfilerFlags="-pg"
# OptimizeFlags="-O2"
# AssemblyFlags="-g -Wa,-ahl"
CommonCompilerFlags="-std=c99 -Wall -Wno-misleading-indentation -x c $ProfilerFlags $OptimizeFlags $AssemblyFlags -I $GladIncludePath -I $GLFWIncludePath -I $STBIncludePath -I $NANOVGIncludePath -I $SHLIncludePath"
CommonLinkerFlags="-L$GLFWLibPath -l glfw -l m"

mkdir -p build
cd build

rm -f war1

gcc $CommonCompilerFlags $CommonLinkerFlags ../src/war1.c $GladSrcPath -o war1
