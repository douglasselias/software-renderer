@echo off

cl

if %ERRORLEVEL% neq 0 (
  call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
)

cls

rmdir /S /Q .\build
mkdir build
pushd .\build

set annoying_warnings=/wd4189 /wd4996 /wd4100 /wd4255 /wd5045 /wd4711 /wd4710 /wd4820 /wd4514 /wd4702 /wd4101
set system_libs=user32.lib shell32.lib gdi32.lib winmm.lib opengl32.lib kernel32.lib

cl /nologo /Z7 /diagnostics:caret /sdl /Wall /WX /W4 %annoying_warnings% /MD /I..\vendor ..\main.c /link ..\vendor\raylib.lib %system_libs%

if %ERRORLEVEL% equ 0 (
  main
)

popd