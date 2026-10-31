@echo off
setlocal

:: Request admin privileges
net session >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo Requesting Administrator privileges...
    powershell -Command "Start-Process '%~f0' -Verb RunAs"
    exit /b
)

title QuickLockSetup - Uninstaller
cd /d "%~dp0"

echo =======================================================
echo          Desktop Lock and Shutdown - Uninstaller
echo =======================================================
echo.

set "TASK_NAME=LockShutKey"

echo Stopping running background process...
taskkill /f /im lock_shut.exe >nul 2>&1

echo Removing scheduled task from Windows...
schtasks /delete /tn "%TASK_NAME%" /f >nul 2>&1

echo.
echo Uninstalled successfully! The hotkey listener has been removed.
echo.
pause
