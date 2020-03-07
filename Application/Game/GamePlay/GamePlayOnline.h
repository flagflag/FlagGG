#pragma once

#include "GamePlay/GamePlayBase.h"
#include "GamePlay/World.h"
#include "Network/NetworkAdaptor.h"
#include "Network/GameProtoDistributor.h"
#include "Unit/Unit.h"

#include <Core/Context.h>
#include <Container/HashMap.h>

using namespace FlagGG::Core;

class GamePlayOnline : public GamePlayBase
{
public:
	explicit GamePlayOnline(Context* context);

	void Initialize(FlagGG::Scene::Scene* scene) override;

	void Login(const LuaFunction& callback) override;

	void StartGame() override;

	void EndGame() override;

protected:
	void OnGameMessageRecived(UInt32 messageType, ::google::protobuf::Message* message);

	void HandleUnitAppear(::google::protobuf::Message* message);

	void HandleUnitDisappear(::google::protobuf::Message* message);

	void HandleStartMove(::google::protobuf::Message* message);

	void HandleStopMove(::google::protobuf::Message* message);

private:
	Context* context_;

	SharedPtr<World> world_;

	WeakPtr<Network> network_;

	LuaFunction loginResponse_;
};
