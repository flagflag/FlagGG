#include "GameLogicServer.h"
#include "EventDefine/CommandEvent.h"

#include <LuaGameEngine/Init.h>
#include <Log.h>

GameLogicServer::GameLogicServer(const FlagGG::Config::LJSONValue& commandParam) :
	commandParam_(commandParam)
{}

void GameLogicServer::Start()
{
	ServerEngine::Start();

	SetFrameRate(60.0f);

	CreateLuaVM();

	CreateNetwork();

	context_->RegisterEvent(EVENT_HANDLER(CommandEvent::START_GAME, GameLogicServer::HandleStartGame, this));
	context_->RegisterEvent(EVENT_HANDLER(Frame::LOGIC_UPDATE, GameLogicServer::Update, this));

	FLAGGG_LOG_INFO("GameLogicServer start.");
}

void GameLogicServer::Update(float timeStep)
{
	luaVM_->CallEvent("game.on_update", timeStep);
}

void GameLogicServer::CreateLuaVM()
{
	luaVM_ = new FlagGG::Lua::LuaVM();
	luaVM_->Open();
	if (!luaVM_->IsOpen())
	{
		FLAGGG_LOG_CRITICAL("Failed to open lua vm.");
		return;
	}

	LuaGameEngine::InitEngine(*luaVM_);

	const String luaCodePath = commandParam_["CodePath"].GetString();
	luaVM_->SetLoaderPath(luaCodePath);
	if (!luaVM_->Execute(luaCodePath + "main.lua"))
	{
		FLAGGG_LOG_ERROR("Failed to execute main.lua.");
		return;
	}
}

void GameLogicServer::CreateNetwork()
{
	tcpNetwork_ = new TCPNetwork();
	udpNetwork_ = new UDPNetwork();

	tcpAcceptor_ = FlagGG::IOFrame::TCP::CreateAcceptor(tcpNetwork_, 1);
	tcpAcceptor_->Bind("127.0.0.1", 5000);
	tcpAcceptor_->Start();

	udpAcceptor_ = FlagGG::IOFrame::UDP::CreateAcceptor(udpNetwork_);
	udpAcceptor_->Bind("127.0.0.1", 5000);
	udpAcceptor_->Start();
}

void GameLogicServer::HandleStartGame(const char* gameName)
{

}
