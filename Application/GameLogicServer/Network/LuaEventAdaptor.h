#pragma once

#include <Container/Ptr.h>
#include <Container/Vector.h>

#include "LuaGameEngine/LuaEvent.h"

namespace FlagGG
{

class Batch;

}

using namespace FlagGG;

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

	void OnDebugUnitTransform(const Vector3& position, const Quaternion& rotation, const Vector3& scale) override;

protected:
	void EndFrameUpdate(float timeStep);

private:
	NetworkSender* sender_;

	Vector<SharedPtr<Batch>> batches_;
};
