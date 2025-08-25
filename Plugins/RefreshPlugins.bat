@echo off

:: Get the current directory and set Plugins directory relative to it
set "PluginsDir=%cd%"

:: Check if Plugins directory exists
if not exist "%PluginsDir%" (
    echo Plugins directory not found.
    exit /b
)

:: Iterate over subdirectories in Plugins
for /d %%P in ("%PluginsDir%\*") do (
    echo Cleaning cache in %%P...
    for /d %%F in ("%%P\*") do (
        if "%%~nxF" == "Binaries" (RD /S /Q "%%F")
        if "%%~nxF" == "Intermediate" (RD /S /Q "%%F")
        if "%%~nxF" == "Saved" (RD /S /Q "%%F")
    )
)

echo Cache cleanup complete.
