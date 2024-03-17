#include "UDPNetwork.h"
#include "EventDefine/GameEvent.h"
#include "Proto/Game.pb.h"

#include <Core/EventManager.h>
#include <Core/Forwarder.h>
#include <AsyncFrame/Mutex.h>
#include <Math/Quaternion.h>

UDPNetwork::UDPNetwork()
{
	buffer = IOFrame::UDP::CreateBuffer();
}

void UDPNetwork::ChannelRegisterd(IOFrame::Context::IOContextPtr context)
{

}

void UDPNetwork::ChannelOpend(IOFrame::Context::IOContextPtr context)
{

}

void UDPNetwork::ChannelClosed(IOFrame::Context::IOContextPtr context)
{
	for (auto it = channels_.Begin(); it != channels_.End(); ++it)
	{
		if (it->second_ == context->GetChannel())
		{
			channels_.Erase(it);

			auto* forwarder = GetSubsystem<Context>()->GetVariable<Forwarder<Mutex>>("Forwarder<Mutex>");
			forwarder->Forward([&]
			{
				GetSubsystem<EventManager>()->SendEvent<GameEvent::START_GAME_HANDLER>("...");
			});

			break;
		}
	}
}

void UDPNetwork::MessageRecived(IOFrame::Context::IOContextPtr context, IOFrame::Buffer::IOBufferPtr buffer)
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

void UDPNetwork::ErrorCatch(IOFrame::Context::IOContextPtr context, const ErrorCode& error_code)
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

void UDPNetwork::HandleRequestLogin(IOFrame::Context::IOContextPtr context, const std::string& messageBody)
{
	Proto::Game::RequestLogin request;
	request.ParseFromString(messageBody);
	Int64 userId = request.user_id();
	channels_.Insert(MakePair(userId, context->GetChannel()));

	Proto::Game::ResponseLogin response;
	response.set_user_id(userId);
	response.set_result(Proto::Game::LoginResult_Success);
	Send(userId, Proto::Game::MessageType_ResponseLogin, &response);

	auto* forwarder = GetSubsystem<Context>()->GetVariable<Forwarder<Mutex>>("Forwarder<Mutex>");
	forwarder->Forward([&, userId]
	{
		GetSubsystem<EventManager>()->SendEvent<GameEvent::USER_LOGIN_HANDLER>(userId);
	});
}

void UDPNetwork::HandleRequestStartGame(IOFrame::Context::IOContextPtr context, const std::string& messageBody)
{
	Proto::Game::RequestStartGame request;
	request.ParseFromString(messageBody);
	const std::string& gameName = request.game_name();

	auto* forwarder = GetSubsystem<Context>()->GetVariable<Forwarder<Mutex>>("Forwarder<Mutex>");
	forwarder->Forward([&, gameName]
	{
		GetSubsystem<EventManager>()->SendEvent<GameEvent::START_GAME_HANDLER>(gameName.c_str());
	});
}

void UDPNetwork::HandleRequestStartMove(IOFrame::Context::IOContextPtr context, const std::string& messageBody)
{
	Proto::Game::RequestStartMove request;
	request.ParseFromString(messageBody);

	Int64 userId = request.user_id();
	Quaternion direction(request.rotation().w(), request.rotation().x(), request.rotation().y(), request.rotation().z());

	auto* forwarder = GetSubsystem<Context>()->GetVariable<Forwarder<Mutex>>("Forwarder<Mutex>");
	forwarder->Forward([&, userId, direction]
	{
		GetSubsystem<EventManager>()->SendEvent<GameEvent::START_MOVE_HANDLER>(userId, direction);
	});
}

void UDPNetwork::HandleRequestStopMove(IOFrame::Context::IOContextPtr context, const std::string& messageBody)
{
	Proto::Game::RequestStopMove request;
	request.ParseFromString(messageBody);

	Int64 userId = request.user_id();

	auto* forwarder = GetSubsystem<Context>()->GetVariable<Forwarder<Mutex>>("Forwarder<Mutex>");
	forwarder->Forward([&, userId]
	{
		GetSubsystem<EventManager>()->SendEvent<GameEvent::STOP_MOVE_HANDLER>(userId);
	});
}
