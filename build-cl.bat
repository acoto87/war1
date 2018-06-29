@echo off

SET LibsDir=D:\Work\libs\
SET CommonCompilerFlags=-Od -MTd -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -FC -Z7 -I %LibsDir% /w
SET CommonLinkerFlags= -incremental:no -opt:ref -out:war1.exe -pdb:war1.pdb -libpath:%LibsDir%glew32\lib -libpath:%LibsDir%SDL2-2.0.5\lib\x64 user32.lib gdi32.lib opengl32.lib comdlg32.lib glew32.lib SDL2.lib

IF NOT EXIST build mkdir build
pushd build

REM 64-bit build
REM Optimization switches /wO2
cl %CommonCompilerFlags% ..\src\war1.cpp /link %CommonLinkerFlags%
popd
