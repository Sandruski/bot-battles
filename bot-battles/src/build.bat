@echo off
mkdir ..\build
pushd ..\build
cl /Zi /W4 /WX ..\src\game\Main.cpp SDL.lib
cl /Zi /W4 /WX ..\src\engine\Engine.cpp
cl /Zi /W4 /WX ..\src\engine\ModuleWindow.cpp
popd
