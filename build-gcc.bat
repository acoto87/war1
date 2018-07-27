@echo off

SET CommonCompilerFlags=-O -Wall -std=c99

IF NOT EXIST build mkdir build
pushd build

gcc %CommonCompilerFlags% ../src/war1.c -o war1.exe
popd
