#pragma once

#include <IOFrame/IOFrame.h>
#include <Container/RefCounted.h>

using namespace FlagGG::Container;

class TCPNetwork : public FlagGG::IOFrame::Handler::EventHandler
{
public:

protected:
	void ChannelRegisterd(FlagGG::IOFrame::Context::IOContextPtr context) override;
	void ChannelOpend(FlagGG::IOFrame::Context::IOContextPtr context) override;
	void ChannelClosed(FlagGG::IOFrame::Context::IOContextPtr context) override;
	void MessageRecived(FlagGG::IOFrame::Context::IOContextPtr context, FlagGG::IOFrame::Buffer::IOBufferPtr buffer) override;
	void ErrorCatch(FlagGG::IOFrame::Context::IOContextPtr context, const FlagGG::ErrorCode& error_code) override;
};
