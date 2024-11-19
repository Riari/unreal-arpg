@echo off

set DIR=%~dp0
call %DIR%batch_vars.bat
call "%UBT%" -projectfiles -vscode -game -engine -dotnet -progress -noIntelliSense "%DIR%ARPG.uproject"
ue-assist -c "%DIR%.vscode\compileCommands_ARPG.json" "%DIR%compile_commands.json"