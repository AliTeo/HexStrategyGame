@echo off
REM Windows Build and Package Script
REM Run this on a Windows machine with Visual Studio installed

echo ==========================================
echo   Building HexGame for Windows
echo ==========================================
echo.

REM Check if CMake is available
where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake not found!
    echo Please install CMake from https://cmake.org/download/
    pause
    exit /b 1
)

REM Create build directory
if not exist build_win mkdir build_win
cd build_win

REM Configure with CMake
echo Configuring project...
cmake .. -DCMAKE_BUILD_TYPE=Release

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake configuration failed!
    pause
    exit /b 1
)

REM Build
echo.
echo Building...
cmake --build . --config Release

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Build failed!
    pause
    exit /b 1
)

REM Create package
echo.
echo Creating Windows package...
if not exist ..\windows_package mkdir ..\windows_package

copy Release\HexGameClient.exe ..\windows_package\ >nul
copy Release\HexGameLauncher.exe ..\windows_package\ >nul
copy Release\*.dll ..\windows_package\ >nul 2>nul

REM Create README for distribution
echo HOW TO CONNECT > ..\windows_package\README.txt
echo ============== >> ..\windows_package\README.txt
echo. >> ..\windows_package\README.txt
echo 1. Double-click HexGameLauncher.exe >> ..\windows_package\README.txt
echo 2. Enter the server IP address >> ..\windows_package\README.txt
echo 3. Click Connect >> ..\windows_package\README.txt
echo. >> ..\windows_package\README.txt
echo That's it! Enjoy the game! >> ..\windows_package\README.txt

cd ..

echo.
echo ==========================================
echo   SUCCESS!
echo ==========================================
echo.
echo Windows package created in: windows_package\
echo.
echo Share this folder with your friend:
echo   - Zip the windows_package folder
echo   - Upload to Google Drive / Dropbox
echo   - Send the download link
echo.
echo Your friend just needs to:
echo   1. Extract the ZIP
echo   2. Double-click HexGameLauncher.exe
echo   3. Enter your server IP
echo.
pause
