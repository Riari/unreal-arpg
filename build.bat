@echo off

set DIR=%~dp0
call %DIR%batch_vars.bat
call "%BUILD%" ARPGEditor Win64 Development "%DIR%ARPG.uproject" -waitmutex -NoHotReload
