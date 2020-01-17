mkdir Application\Game\Proto
mkdir Application\GameLogicServer\Proto
cd bin\Debug\
protoc.exe "G:\flag\Application\Proto\Game.proto" -I="G:/flag/Application/Proto/" --cpp_out="G:/flag/Application/Game/Proto/"
protoc.exe "G:\flag\Application\Proto\Game.proto" -I="G:/flag/Application/Proto/" --cpp_out=G:/flag/Application/GameLogicServer/Proto/
pause
