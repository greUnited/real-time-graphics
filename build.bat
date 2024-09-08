@echo off

set source=src\main.c

cl /Zi %source% /Fe"bin\main.exe" user32.lib kernel32.lib gdi32.lib opengl32.lib
