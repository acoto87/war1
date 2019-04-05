#!/bin/sh

# Include paths
IncludePath="../deps/include"

# Lib paths
LibPath="../deps/lib/arm32"

# ProfilerFlags="-pg"
# OptimizeFlags="-O2"
# AssemblyFlags="-g -Wa,-ahl"
# DebugFlags="-D __DEBUG__"
# PrintPathsFlags="-v"
CommonCompilerFlags="-std=c99 -Wall -Wno-misleading-indentation -x c $ProfilerFlags $OptimizeFlags $AssemblyFlags $DebugFlags $PrintPathsFlags -I $IncludePath"
CommonLinkerFlags="-L $LibPath -l glfw -l m"

mkdir -p build
cd build

rm -f war1

gcc $CommonCompilerFlags $CommonLinkerFlags ../src/war1.c ../deps/include/glad/glad.c $GladSrcPath -o war1
