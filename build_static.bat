mkdir build
cd ./build
rem Generate VS2015 project.
cmake ../ -DFLAGGG_WIN32_CONSOLE=1 -DFLAGGG_D3D11=1 -DFLAGGG_LIB_TYPE="STATIC" -Dprotobuf_BUILD_TESTS=OFF -G "Visual Studio 14"
pause
