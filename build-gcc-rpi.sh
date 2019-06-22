#!/bin/sh

# Output path (relative to build sh file)
OutputPath="./build/arm32"

# Assets path (relative to output folder)
AssetsPath="../../assets"

# Include path (relative to output folder)
IncludePath="../../deps/include"

# Lib path (relative to output folder)
LibPath="../../deps/lib/arm32"

# Compiler flags
# ProfilerFlags="-pg"
# OptimizeFlags="-O2"
# AssemblyFlags="-g -Wa,-ahl"
# DebugFlags="-pg -D __DEBUG__"
CommonCompilerFlags="-std=c99 -Wall -x c $ProfilerFlags $OptimizeFlags $AssemblyFlags $DebugFlags -I $IncludePath"
CommonLinkerFlags="-L $LibPath -lglfw -lm -lopenal -lpthread -ldl"

# Create output path if doesn't exists
mkdir -p $OutputPath
cd $OutputPath

# Empty the build folder
rm -f *

# Compile the project
if gcc $CommonCompilerFlags $CommonLinkerFlags ../../src/war1.c ../../deps/include/glad/glad.c -o war1; then
# Copy assets
cp $AssetsPath/* ./

# Generate ctags
cd ../../
echo "Generating ctags - Start";
ctags --tag-relative --extra=f --language-force=C --recurse=yes
echo "Generating ctags - Done";
fi
