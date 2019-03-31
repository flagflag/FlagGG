mkdir build
cd ./build
rem Generate VS2013 project.
cmake ../ -DFLAGGG_WIN32_CONSOLE=1 -DFLAGGG_D3D11=1 -DFLAGGG_LIB_TYPE="STATIC" -G "Visual Studio 12 2013"
pause
