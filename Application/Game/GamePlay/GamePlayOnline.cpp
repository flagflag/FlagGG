#include "GamePlay/GamePlayOnline.h"
#include "Movement/DirectionMovement.h"

#include <Scene/MovementComponent.h>
#include <Log.h>

GamePlayOnline::GamePlayOnline(Context* context) :
	context_(context)
{
	network_ = context->GetVariable<Network>(NETWORK_TYPE_NAME[NETWORK_TYPE_UDP]);

	context_->RegisterEvent(EVENT_HANDLER(GameProtoEvent::MESSAGE_RECIVED, GamePlayOnline::OnGameMessageRecived, this));
}

void GamePlayOnline::Initialize(FlagGG::Scene::Scene* scene)
{
	world_ = new World(context_);
	world_->SetScene(scene);
}

void GamePlayOnline::Login(const LuaFunction& callback)
{
	if (network_)
	{
		Proto::Game::RequestLogin request;
		request.set_user_id(0u);

		Proto::Game::MessageHeader header;
		header.set_message_type(Proto::Game::MessageType_RequestLogin);
		header.set_message_body(request.SerializeAsString());

		const std::string& buffer = header.SerializeAsString();
		network_->Send(buffer.data(), buffer.length());

		loginResponse_ = callback;
	}
}

void GamePlayOnline::StartGame()
{
	if (network_)
	{
		Proto::Game::RequestStartGame request;
		request.set_user_id(0u);
		request.set_game_name("TestGame");

		Proto::Game::MessageHeader header;
		header.set_message_type(Proto::Game::MessageType_RequestStartGame);
		header.set_message_body(request.SerializeAsString());

		const std::string& buffer = header.SerializeAsString();
		network_->Send(buffer.data(), buffer.length());
	}
}

void GamePlayOnline::EndGame()
{
	if (network_)
	{
		Proto::Game::RequestStopGame request;
		request.set_user_id(0u);
		request.set_game_name("TestGame");

		Proto::Game::MessageHeader header;
		header.set_message_type(Proto::Game::MessageType_RequestStopGame);
		header.set_message_body(request.SerializeAsString());

		const std::string& buffer = header.SerializeAsString();
		network_->Send(buffer.data(), buffer.length());
	}
}

void GamePlayOnline::OnGameMessageRecived(UInt32 messageType, ::google::protobuf::Message* message)
{
	switch (messageType)
	{
	case Proto::Game::MessageType_ResponseLogin:
		loginResponse_(static_cast<Proto::Game::ResponseLogin*>(message)->result() == Proto::Game::LoginResult_Success ? true : false);
		break;

	case Proto::Game::MessageType_AppearUnit:
		HandleUnitAppear(message);
		break;

	case Proto::Game::MessageType_DisappearUnit:
		HandleUnitDisappear(message);
		break;

	case Proto::Game::MessageType_ResponseStartMove:
		HandleStartMove(message);
		break;

	case Proto::Game::MessageType_ResponseStopMove:
		HandleStopMove(message);
		break;
	}
}

void GamePlayOnline::HandleUnitAppear(::google::protobuf::Message* message)
{
	auto* notify = static_cast<Proto::Game::NotifyUnitAppear*>(message);
	SharedPtr<Unit> unit;
	
	if (notify->unit_id() == 1)
	{
		unit = static_cast<Unit*>(world_->GetScene()->GetChild(String("MainHero")));
		unit->Load("Unit/MainHero.ljson");
		unit->PlayAnimation("Animation/Warrior_Idle.ani", true);
	}
	else
	{
		unit = world_->CreateUnit(notify->unit_id());
		if (!unit)
			return;
		unit->Load("Unit/Monster.ljson");
		unit->PlayAnimation("Animation/Monster_Idle.ani", true);
	}
	
	const auto& transform = notify->transform();
	unit->SetWorldPosition(Vector3(transform.position().x(), transform.position().y(), transform.position().z()));
	unit->SetWorldRotation(Quaternion(transform.rotation().x(), transform.rotation().y(), transform.rotation().z(), transform.rotation().w()));

	String name;
	unit->SetName(name.AppendWithFormat("unit_%d", notify->unit_id()));
}

void GamePlayOnline::HandleUnitDisappear(::google::protobuf::Message* message)
{
	auto* notify = static_cast<Proto::Game::NotifyUnitDisappear*>(message);

	world_->DestroyUnit(notify->unit_id());
}

void GamePlayOnline::HandleStartMove(::google::protobuf::Message* message)
{
	auto* notify = static_cast<Proto::Game::ResponseStartMove*>(message);

	Unit* unit = static_cast<Unit*>(world_->GetScene()->GetChild(String("MainHero")));
	if (!unit)
	{
		FLAGGG_LOG_ERROR("Sync start move, but main hero is nullptr.");
		return;
	}

	auto* moveComp = unit->GetComponent<MovementComponent>();
	if (!moveComp)
		moveComp = unit->CreateComponent<MovementComponent>();

	SharedPtr<DirectionMovement> movement(new DirectionMovement());
	moveComp->AddMovement(movement);

	Quaternion direction(notify->rotation().w(), notify->rotation().x(), notify->rotation().y(), notify->rotation().z());
	movement->SetMoveDirection(direction * Vector3::FORWARD);
}

void GamePlayOnline::HandleStopMove(::google::protobuf::Message* message)
{
	auto* notify = static_cast<Proto::Game::ResponseStopMove*>(message);

	Unit* unit = static_cast<Unit*>(world_->GetScene()->GetChild(String("MainHero")));
	if (!unit)
	{
		FLAGGG_LOG_ERROR("Sync start move, but main hero is nullptr.");
		return;
	}

	auto* moveComp = unit->GetComponent<MovementComponent>();
	if (!moveComp)
		moveComp = unit->CreateComponent<MovementComponent>();

	moveComp->RemoveMovement(DirectionMovement::StaticClass());
}
