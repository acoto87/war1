@echo off

SET CommonCompilerFlags=-Wall

IF NOT EXIST build mkdir build
pushd build

gcc %CommonCompilerFlags% ../src/war1.cpp -o war1.exe
popd
