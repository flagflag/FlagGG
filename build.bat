mkdir build
cd ./build
rem Generate VS2017 project.
"C:\Program Files\CMake\bin\cmake" ../ -DFLAGGG_WIN32_CONSOLE=1 -G "Visual Studio 12 2013"
pause
