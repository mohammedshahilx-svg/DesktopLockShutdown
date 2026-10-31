@echo off
setlocal EnableDelayedExpansion

:: -------------------------------------------------------------
:: Elevate to Administrator privileges automatically if needed
:: -------------------------------------------------------------
net session >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo Requesting Administrator privileges...
    powershell -Command "Start-Process '%~f0' -Verb RunAs"
    exit /b
)

title QuickLockSetup - Installer
cd /d "%~dp0"

echo =======================================================
echo          Desktop Lock and Shutdown Hotkey Setup
echo =======================================================
echo.

:: 1. Compile C++ file if g++ is installed and source exists
if exist "lock_shut.cpp" (
    where g++ >nul 2>&1
    if !ERRORLEVEL! equ 0 (
        echo [1/3] Compiling lock_shut.cpp into lock_shut.exe...
        taskkill /f /im lock_shut.exe >nul 2>&1
        g++ -O2 "%~dp0lock_shut.cpp" -o "%~dp0lock_shut.exe" -mwindows
        if !ERRORLEVEL! neq 0 (
            echo [WARNING] Compilation failed. Will attempt to use existing lock_shut.exe if present.
        ) else (
            echo       Compiled successfully!
        )
    ) else (
        echo [1/3] g++ compiler not found in PATH. Using pre-compiled lock_shut.exe...
    )
) else (
    echo [1/3] lock_shut.cpp not found. Using lock_shut.exe directly...
)

:: 2. Verify lock_shut.exe exists
if not exist "%~dp0lock_shut.exe" (
    echo.
    echo [ERROR] lock_shut.exe not found in this folder!
    echo Please make sure lock_shut.exe or lock_shut.cpp is placed next to this installer.
    echo.
    pause
    exit /b 1
)

:: 3. Register in Windows Task Scheduler
echo.
echo [2/3] Setting up Windows Task Scheduler...
set "TASK_NAME=LockShutKey"
set "EXE_PATH=%~dp0lock_shut.exe"

:: Create scheduled task running at user logon with highest privileges
schtasks /create /tn "%TASK_NAME%" /tr "\"%EXE_PATH%\"" /sc ONLOGON /rl HIGHEST /f >nul 2>&1

if %ERRORLEVEL% equ 0 (
    echo       Task registered successfully!
) else (
    echo [ERROR] Failed to register scheduled task.
    pause
    exit /b 1
)

:: 4. Start the background service right now
echo.
echo [3/3] Launching background service...
taskkill /f /im lock_shut.exe >nul 2>&1
schtasks /run /tn "%TASK_NAME%" >nul 2>&1

echo.
echo =======================================================
echo                 Installation Complete!
echo =======================================================
echo.
echo The hotkey listener is now active and will start automatically
echo every time you log in to your computer.
echo.
echo How to use:
echo  1. Click empty space on your Desktop wallpaper.
echo  2. Type 9696 to Lock your computer.
echo  3. Type 2580 to Shut Down your computer.
echo.
pause
