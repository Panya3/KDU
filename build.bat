@echo off
setlocal enabledelayedexpansion

call vcvarsall.bat x64 > nul
msbuild Source\KDU.sln /p:Configuration=Release /p:Platform=x64 /t:Build /p:PlatformToolset=v145

if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    exit /b %ERRORLEVEL%
)

if not exist bin\include mkdir bin\include
if not exist bin\lib mkdir bin\lib

copy /Y Source\Hamakaze\output\x64\Release\Hamakaze.lib bin\lib\ > nul
copy /Y Source\Taigei\output\x64\Release\Taigei.lib bin\lib\ > nul
copy /Y Source\Tanikaze\output\x64\Release\Tanikaze.lib bin\lib\ > nul

copy /Y Source\Hamakaze\hamakaze.h bin\include\ > nul
copy /Y Source\Taigei\taigei.h bin\include\ > nul
copy /Y Source\Tanikaze\tanikaze.h bin\include\ > nul
copy /Y Source\Shared\kdulog.h bin\include\ > nul
copy /Y Source\Shared\kdubase.h bin\include\ > nul

echo.
echo ========================================================
echo Build complete.
echo Output headers placed in: bin\include
echo Output libraries placed in: bin\lib
echo ========================================================
