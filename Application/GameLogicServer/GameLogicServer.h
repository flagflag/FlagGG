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
#include "Network/LuaEventAdaptor.h"

using namespace FlagGG::Container;
using namespace FlagGG::Core;
using namespace FlagGG::AsyncFrame;
using namespace FlagGG::Math;

namespace LuaGameEngine
{
	class Engine;
}

class GameLogicServer : public FlagGG::ServerEngine
{
public:
	GameLogicServer(const FlagGG::Config::LJSONValue& commandParam);

protected:
	void Start() override;
	void Update(float timeStep);

	void CreateLuaVM();

	void CreateNetwork();

	void HandleUserLogin(Int64 userId);
	void HandleStartGame(const char* gameName);
	void HandleStopGame(const char* gameName);
	void HandleStartMove(Int64 userId, Quaternion direction);
	void HandleStopMove(Int64 userId);

private:
	FlagGG::Config::LJSONValue commandParam_;

	SharedPtr<FlagGG::Lua::LuaVM> luaVM_;

	FlagGG::IOFrame::Acceptor::IOAcceptorPtr tcpAcceptor_;
	FlagGG::IOFrame::Acceptor::IOAcceptorPtr udpAcceptor_;

	SharedPtr<TCPNetwork> tcpNetwork_;
	SharedPtr<UDPNetwork> udpNetwork_;
	SharedPtr<LuaEventAdaptor> luaEventAdaptor_;

	LuaGameEngine::Engine* engine_;

	SharedPtr<Forwarder<Mutex>> forwarder_;
};
