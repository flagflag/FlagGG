#include "GamePlay/GamePlayOnline.h"

#include <Log.h>

GamePlayOnline::GamePlayOnline(Context* context) :
	context_(context)
{
	network_ = context->GetVariable<Network>(NETWORK_TYPE_NAME[NETWORK_TYPE_UDP]);

	context_->RegisterEvent(EVENT_HANDLER(GameProtoEvent::MESSAGE_RECIVED, GamePlayOnline::OnGameMessageRecived, this));
}

void GamePlayOnline::Initialize(FlagGG::Scene::Scene* scene)
{
	scene_ = scene;
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
	}
}

void GamePlayOnline::HandleUnitAppear(::google::protobuf::Message* message)
{
	auto* notify = static_cast<Proto::Game::NotifyUnitAppear*>(message);
	SharedPtr<Unit>& unit = tempUnits_[notify->unit_id()];
	if (unit)
	{
		FLAGGG_LOG_CRITICAL("Appear repeat unit!!!");
		return;
	}
	
	unit = new Unit(context_);
	unit->Load("Unit/MainHero.ljson");
	unit->PlayAnimation("Animation/Kachujin_Walk.ani", true);
	scene_->AddChild(unit);
	
	const auto& transform = notify->transform();
	unit->SetWorldPosition(Vector3(transform.position().x(), transform.position().y(), transform.position().z()));
	unit->SetWorldRotation(Quaternion(transform.rotation().x(), transform.rotation().y(), transform.rotation().z(), transform.rotation().w()));

	String name;
	unit->SetName(name.AppendWithFormat("unit_%d", notify->unit_id()));
}

void GamePlayOnline::HandleUnitDisappear(::google::protobuf::Message* message)
{
	auto* notify = static_cast<Proto::Game::NotifyUnitDisappear*>(message);
	
	auto it = tempUnits_.Find(notify->unit_id());
	if (it == tempUnits_.End())
	{
		FLAGGG_LOG_CRITICAL("Disappear none unit!!!");
		return;
	}

	scene_->RemoveChild(it->second_);
	tempUnits_.Erase(it);
}
