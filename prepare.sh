mkdir bin
cd ./bin
mkdir Debug
mkdir Release
cd ..

current_path=$(pwd)

ln -s $current_path/Res $current_path/bin/Debug/Res
ln -s $current_path/Res $current_path/bin/Release/Res
