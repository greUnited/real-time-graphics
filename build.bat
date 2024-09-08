@echo off

cl src\main.c /Fe"bin\main.exe" user32.lib kernel32.lib gdi32.lib opengl32.lib
