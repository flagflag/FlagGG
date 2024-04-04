#include "GameLogicServer.h"
#include "EventDefine/GameEvent.h"

#include <LuaGameEngine/Init.h>
#include <Core/EventManager.h>
#include <Log.h>

GameLogicServer::GameLogicServer(const LJSONValue& commandParam) :
	commandParam_(commandParam)
{}

void GameLogicServer::Start()
{
	ServerEngine::Start();

	SetFrameRate(60.0f);

	forwarder_ = new Forwarder<Mutex>();
	GetSubsystem<Context>()->RegisterVariable<Forwarder<Mutex>>(forwarder_, "Forwarder<Mutex>");

	CreateNetwork();

	CreateLuaVM();

	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(GameEvent::USER_LOGIN, GameLogicServer::HandleUserLogin, this));
	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(GameEvent::START_GAME, GameLogicServer::HandleStartGame, this));
	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(GameEvent::STOP_GAME, GameLogicServer::HandleStopGame, this));
	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(GameEvent::START_MOVE, GameLogicServer::HandleStartMove, this));
	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(GameEvent::STOP_MOVE, GameLogicServer::HandleStopMove, this));
	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(Frame::LOGIC_UPDATE, GameLogicServer::Update, this));

	FLAGGG_LOG_INFO("GameLogicServer start.");
}

void GameLogicServer::Update(float timeStep)
{
	forwarder_->Execute();

	engine_->OnFrameUpdate(timeStep);
}

void GameLogicServer::CreateLuaVM()
{
	luaVM_ = new LuaVM();
	luaVM_->Open();
	if (!luaVM_->IsOpen())
	{
		FLAGGG_LOG_CRITICAL("Failed to open lua vm.");
		return;
	}

	luaEventAdaptor_ = new LuaEventAdaptor(udpNetwork_);
	engine_ = LuaGameEngine::CreateEngine(*luaVM_);
	engine_->RegisterEventHandler(luaEventAdaptor_);

	const String luaCodePath = commandParam_.Contains("CodePath") ? commandParam_["CodePath"].GetString() : GetProgramDir() + "Res/Script/lua/";
	luaVM_->SetLoaderPath(luaCodePath);
	if (!luaVM_->Execute(luaCodePath + "Server/main.lua"))
	{
		FLAGGG_LOG_ERROR("Failed to execute main.lua.");
	}
}

void GameLogicServer::CreateNetwork()
{
	tcpNetwork_ = new TCPNetwork();
	udpNetwork_ = new UDPNetwork();

	tcpAcceptor_ = IOFrame::TCP::CreateAcceptor(tcpNetwork_, 1);
	tcpAcceptor_->Bind("127.0.0.1", 5001);
	tcpAcceptor_->Start();

	udpAcceptor_ = IOFrame::UDP::CreateAcceptor(udpNetwork_);
	udpAcceptor_->Bind("127.0.0.1", 5000);
	udpAcceptor_->Start();
}

void GameLogicServer::HandleUserLogin(Int64 userId)
{
	LuaGameEngine::LuaUserInfo info;
	info.userId_ = userId;
	info.userName_ = "FlagGG";
	engine_->AddUser(info);
}

void GameLogicServer::HandleStartGame(const char* gameName)
{
	engine_->OnStart();
}

void GameLogicServer::HandleStopGame(const char* gameName)
{
	engine_->OnStop();
}

void GameLogicServer::HandleStartMove(Int64 userId, Quaternion direction)
{
	engine_->GetControler(userId)->StartDirectionMove(direction * Vector3::UP);
}

void GameLogicServer::HandleStopMove(Int64 userId)
{
	engine_->GetControler(userId)->StopDirectionMove();
}
