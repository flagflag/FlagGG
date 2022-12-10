#pragma once

#include <IOFrame/IOFrame.h>
#include <Container/RefCounted.h>
#include <Core/Context.h>

using namespace FlagGG;

class TCPNetwork : public IOFrame::Handler::EventHandler
{
public:
	explicit TCPNetwork(Context* context);

protected:
	void ChannelRegisterd(IOFrame::Context::IOContextPtr context) override;
	void ChannelOpend(IOFrame::Context::IOContextPtr context) override;
	void ChannelClosed(IOFrame::Context::IOContextPtr context) override;
	void MessageRecived(IOFrame::Context::IOContextPtr context, IOFrame::Buffer::IOBufferPtr buffer) override;
	void ErrorCatch(IOFrame::Context::IOContextPtr context, const ErrorCode& error_code) override;

private:
	Context* context_;
};
