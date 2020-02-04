#pragma once

#include <Container/RefCounted.h>

#include "LuaGameEngine/LuaEvent.h"

using namespace FlagGG::Container;

namespace google
{
	namespace protobuf
	{
		class Message;
	}
}

class NetworkSender
{
public:
	virtual ~NetworkSender() = default;

	virtual void Send(Int64 userId, UInt32 messageType, ::google::protobuf::Message* message) = 0;
};

class LuaEventAdaptor : public RefCounted, public LuaGameEngine::LuaEventHandler
{
public:
	LuaEventAdaptor(NetworkSender* sender);

	void OnKeepAlive() override;

	void OnStartGame(const char* gameName) override;

	void OnStopGame() override;

	void OnAppearUnit(Int64 unitId, LuaGameEngine::Unit* unit) override;

	void OnDisappearUnit(Int64 unitId) override;

	void OnSyncUnitTansform(Int64 unitId, LuaGameEngine::Transform* transform) override;

	void OnSyncUnitStatus(Int64 unitId, LuaGameEngine::Status status) override;

	void OnSyncUnitAttribute(Int64 unitId, LuaGameEngine::Attribute* attribute) override;

	void OnSyncUnitMovement(Int64 unitId, LuaGameEngine::Movement* movement) override;

	void OnUnitCastSpell(Int64 unitId, LuaGameEngine::Spell* spell) override;

	void OnUnitAttachBuff(Int64 unitId, LuaGameEngine::Buff* buff) override;

	void OnUnitDettachBuff(Int64 unitId, LuaGameEngine::Buff* buff) override;

private:
	NetworkSender* sender_;
};
