#include "UDPNetwork.h"
#include "EventDefine/CommandEvent.h"

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
	String content;
	buffer->ToString(content);
	if (content == "START_GAME")
	{
		context_->SendEvent<CommandEvent::START_GAME_HANDLER>(CommandEvent::START_GAME, "TestGame");
	}
}

void UDPNetwork::ErrorCatch(FlagGG::IOFrame::Context::IOContextPtr context, const FlagGG::ErrorCode& error_code)
{

}
