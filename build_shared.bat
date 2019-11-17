mkdir build
cd ./build
rem Generate VS2013 project.
cmake ../ -DFLAGGG_WIN32_CONSOLE=1 -DFLAGGG_D3D11=1 -DFLAGGG_LIB_TYPE="SHARED" -DCMAKE_INSTALL_PREFIX=G:/flag/CMakeInstall -G "Visual Studio 14"
pause
