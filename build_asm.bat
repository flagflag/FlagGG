mkdir build_asm
cd ./build_asm
rem Generate asm project.
cmake ../ -DFLAGGG_WIN32_CONSOLE=1 -DFLAGGG_D3D11=0 -DFLAGGG_LIB_TYPE="STATIC" -DCMAKE_INSTALL_PREFIX=D:/flag2/CMakeInstall -DFLAGGG_NO_BOOST=1 -DFLAGGG_PROTO=0 -Dprotobuf_BUILD_TESTS=OFF -DWEB=1 -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE="..//CMake//Emscripten.cmake"
pause
