@echo off
:: ============================================================
:: BambuLabs VLC Quadrant Launcher
:: Double-click or assign to a Stream Deck "System: Open" button.
:: ============================================================

setlocal

:: Resolve the directory this batch file lives in
set "SCRIPT_DIR=%~dp0"

:: ------------------------------------------------------------------
:: Check for Python (prefer py launcher, fall back to python)
:: ------------------------------------------------------------------
where py >nul 2>&1
if %ERRORLEVEL% == 0 (
    set "PYTHON=py -3"
) else (
    where python >nul 2>&1
    if %ERRORLEVEL% == 0 (
        set "PYTHON=python"
    ) else (
        echo ERROR: Python 3 is not installed or not in PATH.
        echo Download Python from https://www.python.org/downloads/
        pause
        exit /b 1
    )
)

:: ------------------------------------------------------------------
:: Install / upgrade dependencies silently on first run
:: ------------------------------------------------------------------
%PYTHON% -m pip install --quiet --upgrade psutil pywin32 >nul 2>&1

:: ------------------------------------------------------------------
:: Launch the quadrant viewer (new console window stays open on error)
:: ------------------------------------------------------------------
start "BambuLabs VLC Streams" /B %PYTHON% "%SCRIPT_DIR%bambu_vlc_quadrant.py"

endlocal
