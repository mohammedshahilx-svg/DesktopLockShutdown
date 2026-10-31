@echo off
setlocal EnableDelayedExpansion

title Desktop Quick-Lock - Change PINs
cd /d "%~dp0"

echo =======================================================
echo          Change Lock and Shutdown Key Codes
echo =======================================================
echo.

:: Check if source file exists
if not exist "lock_shut.cpp" (
    echo [ERROR] lock_shut.cpp was not found in this directory!
    echo Please ensure change_codes.bat is in the same folder as lock_shut.cpp.
    echo.
    pause
    exit /b 1
)

:: Prompt for new codes
:ask_lock
set /p "NEW_LOCK=Enter new 4-digit LOCK code (e.g. 9696): "
if "%NEW_LOCK%"=="" (
    echo Code cannot be empty. Please try again.
    goto ask_lock
)

:ask_shutdown
set /p "NEW_SHUTDOWN=Enter new 4-digit SHUTDOWN code (e.g. 2580): "
if "%NEW_SHUTDOWN%"=="" (
    echo Code cannot be empty. Please try again.
    goto ask_shutdown
)

echo.
echo Updating lock_shut.cpp with new codes...
echo  - Lock code:     %NEW_LOCK%
echo  - Shutdown code: %NEW_SHUTDOWN%

:: Use PowerShell to safely replace LOCK_CODE and SHUTDOWN_CODE in lock_shut.cpp
powershell -NoProfile -Command "(Get-Content -Path 'lock_shut.cpp') -replace '#define LOCK_CODE \".*\"', '#define LOCK_CODE \"%NEW_LOCK%\"' -replace '#define SHUTDOWN_CODE \".*\"', '#define SHUTDOWN_CODE \"%NEW_SHUTDOWN%\"' | Set-Content -Path 'lock_shut.cpp'"

if %ERRORLEVEL% neq 0 (
    echo [ERROR] Failed to update lock_shut.cpp.
    pause
    exit /b 1
)

echo.
echo Recompiling lock_shut.exe...
where g++ >nul 2>&1
if %ERRORLEVEL% equ 0 (
    :: Stop old instance
    taskkill /f /im lock_shut.exe >nul 2>&1
    
    :: Compile
    g++ -O2 "%~dp0lock_shut.cpp" -o "%~dp0lock_shut.exe" -mwindows
    
    if !ERRORLEVEL! equ 0 (
        echo Compilation successful!
        
        :: Restart task or process
        schtasks /query /tn "LockShutKey" >nul 2>&1
        if !ERRORLEVEL! equ 0 (
            schtasks /run /tn "LockShutKey" >nul 2>&1
        ) else (
            start "" "%~dp0lock_shut.exe"
        )
        
        echo.
        echo =======================================================
        echo           PINs updated and active immediately!
        echo =======================================================
        echo  - New Lock PIN:     %NEW_LOCK%
        echo  - New Shutdown PIN: %NEW_SHUTDOWN%
        echo.
    ) else (
        echo [ERROR] Compilation failed. Please check lock_shut.cpp.
    )
) else (
    echo.
    echo [NOTE] 'g++' compiler was not found in your system PATH.
    echo 'lock_shut.cpp' has been updated with your new PINs.
    echo To apply them, compile with g++ or your C++ compiler.
    echo.
)

pause
