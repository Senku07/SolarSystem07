@echo off
mkdir build

pushd build
cl ../Src/main.cpp /I"../dp" /I"../dp/include"   ^
 /MD /Zi /Fe:Solar /link user32.lib shell32.lib kernel32.lib gdi32.lib ../dp/glfw3.lib opengl32.lib 
Solar.exe
popd

