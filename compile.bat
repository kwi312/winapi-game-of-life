@echo off
echo  
cl.exe %~dp0\main.c  /link /DEFAULTLIB:user32.lib /DEFAULTLIB:kernel32.lib /DEFAULTLIB:Gdi32.lib /SUBSYSTEM:WINDOWS
echo
@echo on