#pragma once

#include <ServerEngine.h>
#include <Config/LJSONValue.h>
#include <IOFrame/IOFrame.h>
#include <Container/Ptr.h>

#include "Network/TCPNetwork.h"
#include "Network/UDPNetwork.h"

using namespace FlagGG::Container;

class GameLogicServer : public FlagGG::ServerEngine
{
public:
	GameLogicServer(const FlagGG::Config::LJSONValue& commandParam);

protected:
	void Start() override;

	void CreateNetwork();

	void HandleStartGame(const char* gameName);

private:
	FlagGG::Config::LJSONValue commandParam_;

	FlagGG::IOFrame::Acceptor::IOAcceptorPtr tcpAcceptor_;
	FlagGG::IOFrame::Acceptor::IOAcceptorPtr udpAcceptor_;

	SharedPtr<TCPNetwork> tcpNetwork_;
	SharedPtr<UDPNetwork> udpNetwork_;
};
