@echo off

set source=src\main.c src\wgl_file_loading.c src\wgl_transformation_maths.c

cl /Zi /I"include" %source% /Fe"bin\main.exe" /link /LIBPATH:"lib" user32.lib kernel32.lib gdi32.lib opengl32.lib glew32.lib
