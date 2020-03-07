#include "UDPNetwork.h"
#include "EventDefine/GameEvent.h"
#include "Proto/Game.pb.h"

#include <Core/Forwarder.h>
#include <AsyncFrame/Mutex.h>
#include <Math/Quaternion.h>

using namespace FlagGG::Core;
using namespace FlagGG::AsyncFrame;

UDPNetwork::UDPNetwork(Context* context) :
	context_(context)
{
	buffer = FlagGG::IOFrame::UDP::CreateBuffer();
}

void UDPNetwork::ChannelRegisterd(FlagGG::IOFrame::Context::IOContextPtr context)
{

}

void UDPNetwork::ChannelOpend(FlagGG::IOFrame::Context::IOContextPtr context)
{

}

void UDPNetwork::ChannelClosed(FlagGG::IOFrame::Context::IOContextPtr context)
{
	for (auto it = channels_.Begin(); it != channels_.End(); ++it)
	{
		if (it->second_ == context->GetChannel())
		{
			channels_.Erase(it);

			auto* forwarder = context_->GetVariable<Forwarder<Mutex>>("Forwarder<Mutex>");
			forwarder->Forward([&]
			{
				context_->SendEvent<GameEvent::START_GAME_HANDLER>(GameEvent::STOP_GAME, "...");
			});

			break;
		}
	}
}

void UDPNetwork::MessageRecived(FlagGG::IOFrame::Context::IOContextPtr context, FlagGG::IOFrame::Buffer::IOBufferPtr buffer)
{
	String bufferContent;
	buffer->ToString(bufferContent);

	Proto::Game::MessageHeader header;
	header.ParseFromString(std::string(bufferContent.CString(), bufferContent.Length()));

	switch (header.message_type())
	{
	case Proto::Game::MessageType_RequestLogin:
		HandleRequestLogin(context, header.message_body());
		break;

	case Proto::Game::MessageType_RequestStartGame:
		HandleRequestStartGame(context, header.message_body());
		break;

	case Proto::Game::MessageType_RequestStartMove:
		HandleRequestStartMove(context, header.message_body());
		break;

	case Proto::Game::MessageType_RequestStopMove:
		HandleRequestStopMove(context, header.message_body());
		break;
	}
}

void UDPNetwork::ErrorCatch(FlagGG::IOFrame::Context::IOContextPtr context, const FlagGG::ErrorCode& error_code)
{

}

void UDPNetwork::Send(Int64 userId, UInt32 messageType, ::google::protobuf::Message* message)
{
	auto it = channels_.Find(userId);
	if (it == channels_.End())
		return;

	Proto::Game::MessageHeader header;
	header.set_message_type(messageType);
	header.set_message_body(message->SerializeAsString());
	const std::string& stream = header.SerializeAsString();

	buffer->Clear();
	buffer->WriteStream(stream.data(), stream.size());

	it->second_->Write(buffer);
}

void UDPNetwork::HandleRequestLogin(FlagGG::IOFrame::Context::IOContextPtr context, const std::string& messageBody)
{
	Proto::Game::RequestLogin request;
	request.ParseFromString(messageBody);
	Int64 userId = request.user_id();
	channels_.Insert(MakePair(userId, context->GetChannel()));

	Proto::Game::ResponseLogin response;
	response.set_user_id(userId);
	response.set_result(Proto::Game::LoginResult_Success);
	Send(userId, Proto::Game::MessageType_ResponseLogin, &response);

	auto* forwarder = context_->GetVariable<Forwarder<Mutex>>("Forwarder<Mutex>");
	forwarder->Forward([&, userId]
	{
		context_->SendEvent<GameEvent::USER_LOGIN_HANDLER>(GameEvent::USER_LOGIN, userId);
	});
}

void UDPNetwork::HandleRequestStartGame(FlagGG::IOFrame::Context::IOContextPtr context, const std::string& messageBody)
{
	Proto::Game::RequestStartGame request;
	request.ParseFromString(messageBody);
	const std::string& gameName = request.game_name();

	auto* forwarder = context_->GetVariable<Forwarder<Mutex>>("Forwarder<Mutex>");
	forwarder->Forward([&, gameName]
	{
		context_->SendEvent<GameEvent::START_GAME_HANDLER>(GameEvent::START_GAME, gameName.c_str());
	});
}

void UDPNetwork::HandleRequestStartMove(FlagGG::IOFrame::Context::IOContextPtr context, const std::string& messageBody)
{
	Proto::Game::RequestStartMove request;
	request.ParseFromString(messageBody);

	Int64 userId = request.user_id();
	FlagGG::Math::Quaternion direction(request.rotation().w(), request.rotation().x(), request.rotation().y(), request.rotation().z());

	auto* forwarder = context_->GetVariable<Forwarder<Mutex>>("Forwarder<Mutex>");
	forwarder->Forward([&, userId]
	{
		context_->SendEvent<GameEvent::START_MOVE_HANDLER>(GameEvent::START_MOVE, userId, direction);
	});
}

void UDPNetwork::HandleRequestStopMove(FlagGG::IOFrame::Context::IOContextPtr context, const std::string& messageBody)
{
	Proto::Game::RequestStopMove request;
	request.ParseFromString(messageBody);

	Int64 userId = request.user_id();

	auto* forwarder = context_->GetVariable<Forwarder<Mutex>>("Forwarder<Mutex>");
	forwarder->Forward([&, userId]
	{
		context_->SendEvent<GameEvent::STOP_MOVE_HANDLER>(GameEvent::STOP_MOVE, userId);
	});
}
