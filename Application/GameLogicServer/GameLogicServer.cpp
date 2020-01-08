#include "GameLogicServer.h"
#include "EventDefine/CommandEvent.h"

#include <Log.h>

GameLogicServer::GameLogicServer(const FlagGG::Config::LJSONValue& commandParam) :
	commandParam_(commandParam)
{}

void GameLogicServer::Start()
{
	ServerEngine::Start();

	SetFrameRate(60.0f);

	CreateNetwork();

	context_->RegisterEvent(EVENT_HANDLER(CommandEvent::START_GAME, GameLogicServer::HandleStartGame, this));

	FLAGGG_LOG_INFO("GameLogicServer start.");
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
