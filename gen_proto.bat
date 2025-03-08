mkdir Application\Game\Proto
mkdir Application\GameLogicServer\Proto
cd bin\Debug\
echo "%~dp0Application/Proto/" == %~dp0Application/Game/Proto/
protoc.exe "%~dp0Application\Proto\Game.proto" -I="%~dp0Application/Proto/" --cpp_out="%~dp0Application/Game/Proto/"
protoc.exe "%~dp0Application\Proto\Game.proto" -I="%~dp0Application/Proto/" --cpp_out="%~dp0Application/GameLogicServer/Proto/"
protoc.exe "%~dp0Application\Proto\Map.proto" -I="%~dp0Application/Proto/" --cpp_out="%~dp0Application/Game/Proto/"
pause
