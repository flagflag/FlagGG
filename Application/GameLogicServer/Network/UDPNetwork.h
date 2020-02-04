#pragma once

#include <IOFrame/IOFrame.h>
#include <Container/RefCounted.h>
#include <Container/HashMap.h>
#include <Core/Context.h>

#include "Network/LuaEventAdaptor.h"

using namespace FlagGG::Container;
using namespace FlagGG::Core;

class UDPNetwork : public FlagGG::IOFrame::Handler::EventHandler, public NetworkSender
{
public:
	explicit UDPNetwork(Context* context);

	void Send(Int64 userId, UInt32 messageType, ::google::protobuf::Message* message) override;

protected:
	void ChannelRegisterd(FlagGG::IOFrame::Context::IOContextPtr context) override;
	void ChannelOpend(FlagGG::IOFrame::Context::IOContextPtr context) override;
	void ChannelClosed(FlagGG::IOFrame::Context::IOContextPtr context) override;
	void MessageRecived(FlagGG::IOFrame::Context::IOContextPtr context, FlagGG::IOFrame::Buffer::IOBufferPtr buffer) override;
	void ErrorCatch(FlagGG::IOFrame::Context::IOContextPtr context, const FlagGG::ErrorCode& error_code) override;

	void HandleRequestLogin(FlagGG::IOFrame::Context::IOContextPtr context, const std::string& messageBody);
	void HandleRequestStartGame(FlagGG::IOFrame::Context::IOContextPtr context, const std::string& messageBody);

private:
	Context* context_;

	HashMap<Int64, FlagGG::IOFrame::Channel::IOChannelPtr> channels_;

	FlagGG::IOFrame::Buffer::IOBufferPtr buffer;
};
