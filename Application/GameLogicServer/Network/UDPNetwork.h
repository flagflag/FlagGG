#pragma once

#include <IOFrame/IOFrame.h>
#include <Container/RefCounted.h>
#include <Container/HashMap.h>
#include <Core/Context.h>

#include "Network/LuaEventAdaptor.h"

#include <string>

using namespace FlagGG;

class UDPNetwork : public IOFrame::Handler::EventHandler, public NetworkSender
{
public:
	explicit UDPNetwork(Context* context);

	void Send(Int64 userId, UInt32 messageType, ::google::protobuf::Message* message) override;

protected:
	void ChannelRegisterd(IOFrame::Context::IOContextPtr context) override;
	void ChannelOpend(IOFrame::Context::IOContextPtr context) override;
	void ChannelClosed(IOFrame::Context::IOContextPtr context) override;
	void MessageRecived(IOFrame::Context::IOContextPtr context, IOFrame::Buffer::IOBufferPtr buffer) override;
	void ErrorCatch(IOFrame::Context::IOContextPtr context, const ErrorCode& error_code) override;

	void HandleRequestLogin(IOFrame::Context::IOContextPtr context, const std::string& messageBody);
	void HandleRequestStartGame(IOFrame::Context::IOContextPtr context, const std::string& messageBody);
	void HandleRequestStartMove(IOFrame::Context::IOContextPtr context, const std::string& messageBody);
	void HandleRequestStopMove(IOFrame::Context::IOContextPtr context, const std::string& messageBody);

private:
	Context* context_;

	HashMap<Int64, IOFrame::Channel::IOChannelPtr> channels_;

	IOFrame::Buffer::IOBufferPtr buffer;
};
