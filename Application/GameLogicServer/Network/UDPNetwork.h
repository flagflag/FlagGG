#pragma once

#include <IOFrame/IOFrame.h>
#include <Container/RefCounted.h>
#include <Core/Context.h>

using namespace FlagGG::Container;
using namespace FlagGG::Core;

class UDPNetwork : public FlagGG::IOFrame::Handler::EventHandler
{
public:
	explicit UDPNetwork(Context* context);

protected:
	void ChannelRegisterd(FlagGG::IOFrame::Context::IOContextPtr context) override;
	void ChannelOpend(FlagGG::IOFrame::Context::IOContextPtr context) override;
	void ChannelClosed(FlagGG::IOFrame::Context::IOContextPtr context) override;
	void MessageRecived(FlagGG::IOFrame::Context::IOContextPtr context, FlagGG::IOFrame::Buffer::IOBufferPtr buffer) override;
	void ErrorCatch(FlagGG::IOFrame::Context::IOContextPtr context, const FlagGG::ErrorCode& error_code) override;

private:
	Context* context_;
};
