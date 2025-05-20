:: script for building in windows (used by GHA)

mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
IF %ERRORLEVEL% NEQ 0 (
    echo CMake configuration failed!
    exit /b %ERRORLEVEL%
)

cmake --build . -j8
IF %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    exit /b %ERRORLEVEL%
)

dir /s
exit /b 0
