#include "UDPNetwork.h"
#include "EventDefine/CommandEvent.h"
#include "Proto/Game.pb.h"

#include <Core/Forwarder.h>
#include <AsyncFrame/Mutex.h>

using namespace FlagGG::Core;
using namespace FlagGG::AsyncFrame;

UDPNetwork::UDPNetwork(Context* context) :
	context_(context)
{}

void UDPNetwork::ChannelRegisterd(FlagGG::IOFrame::Context::IOContextPtr context)
{

}

void UDPNetwork::ChannelOpend(FlagGG::IOFrame::Context::IOContextPtr context)
{

}

void UDPNetwork::ChannelClosed(FlagGG::IOFrame::Context::IOContextPtr context)
{

}

void UDPNetwork::MessageRecived(FlagGG::IOFrame::Context::IOContextPtr context, FlagGG::IOFrame::Buffer::IOBufferPtr buffer)
{
	String bufferContent;
	buffer->ToString(bufferContent);

	Proto::Game::MessageHeader header;
	header.ParseFromString(std::string(bufferContent.CString(), bufferContent.Length()));

	switch (header.message_type())
	{
	case Proto::Game::MessageType_RequestStartGame:
		{
			Proto::Game::RequestStartGame request;
			request.ParseFromString(header.message_body());
			const std::string& gameName = request.game_name();

			auto* forwarder = context_->GetVariable<Forwarder<Mutex>>("Forwarder<Mutex>");
			forwarder->Forward([&, gameName]
			{
				context_->SendEvent<CommandEvent::START_GAME_HANDLER>(CommandEvent::START_GAME, gameName.c_str());
			});
		}
		break;
	}
}

void UDPNetwork::ErrorCatch(FlagGG::IOFrame::Context::IOContextPtr context, const FlagGG::ErrorCode& error_code)
{

}
