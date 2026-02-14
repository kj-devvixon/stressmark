@echo off
REM Build script for Hardware Stress Test Suite
REM MIT License (c) 2026 kj-devvixon

echo ==========================================
echo  Hardware Stress Test Suite - Build
echo  MIT License (c) 2026 kj-devvixon
echo ==========================================
echo.

REM Check for Visual Studio
where cl >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo Error: Visual Studio compiler not found!
    echo Please run this from a Visual Studio Developer Command Prompt
    echo Or install Visual Studio Build Tools
    pause
    exit /b 1
)

REM Create build directory
echo Creating build directory...
if not exist build mkdir build
cd build

REM Choose build type
echo.
echo Select build type:
echo 1] Debug
echo 2] Release
set /p choice="Enter choice [1-2]: "

if "%choice%"=="1" (
    set BUILD_TYPE=Debug
) else (
    set BUILD_TYPE=Release
)

REM Check for CMake
where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo Error: CMake not found!
    echo Download from: https://cmake.org/download/
    pause
    exit /b 1
)

echo.
echo Building with CMake (%BUILD_TYPE%)...
cmake -G "Visual Studio 16 2019" ..
cmake --build . --config %BUILD_TYPE%

if %ERRORLEVEL% EQU 0 (
    echo.
    echo [SUCCESS] Build successful!
    echo Executable: .\build\%BUILD_TYPE%\stress_test.exe
    echo.
    set /p run="Run now? [y/N]: "
    if /i "%run%"=="y" (
        %BUILD_TYPE%\stress_test.exe
    )
) else (
    echo.
    echo [FAILED] Build failed!
    pause
    exit /b 1
)

cd ..
pause
