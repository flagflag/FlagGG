mkdir Application\Game\Proto
mkdir Application\GameLogicServer\Proto
cd bin\Debug\
protoc.exe "D:\flag\Application\Proto\Game.proto" -I="D:/flag/Application/Proto/" --cpp_out="D:/flag/Application/Game/Proto/"
protoc.exe "D:\flag\Application\Proto\Game.proto" -I="D:/flag/Application/Proto/" --cpp_out=D:/flag/Application/GameLogicServer/Proto/
pause
