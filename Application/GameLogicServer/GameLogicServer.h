#pragma once

#include <ServerEngine.h>
#include <Config/LJSONValue.h>
#include <IOFrame/IOFrame.h>
#include <Container/Ptr.h>
#include <Lua/LuaVM.h>
#include <Core/Forwarder.h>
#include <AsyncFrame/Mutex.h>

#include "Network/TCPNetwork.h"
#include "Network/UDPNetwork.h"

using namespace FlagGG::Container;
using namespace FlagGG::Core;
using namespace FlagGG::AsyncFrame;

class GameLogicServer : public FlagGG::ServerEngine
{
public:
	GameLogicServer(const FlagGG::Config::LJSONValue& commandParam);

protected:
	void Start() override;
	void Update(float timeStep);

	void CreateLuaVM();

	void CreateNetwork();

	void HandleStartGame(const char* gameName);

private:
	FlagGG::Config::LJSONValue commandParam_;

	SharedPtr<FlagGG::Lua::LuaVM> luaVM_;

	FlagGG::IOFrame::Acceptor::IOAcceptorPtr tcpAcceptor_;
	FlagGG::IOFrame::Acceptor::IOAcceptorPtr udpAcceptor_;

	SharedPtr<TCPNetwork> tcpNetwork_;
	SharedPtr<UDPNetwork> udpNetwork_;

	SharedPtr<Forwarder<Mutex>> forwarder_;
};
