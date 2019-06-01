#pragma once

#include <IOFrame/IOFrame.h>
#include <Container/Str.h>

using namespace FlagGG;
using namespace FlagGG::Container;

class Network : public IOFrame::Handler::EventHandler
{
public:
	Network();

	~Network() override;

	void Connect(const String& ip, uint16_t port);

	void Disconnect();

	void SendMessage(const void* data, uint32_t dataSize);

protected:
	void ChannelRegisterd(IOFrame::Context::IOContextPtr context) override;

	void ChannelOpend(IOFrame::Context::IOContextPtr context) override;

	void ChannelClosed(IOFrame::Context::IOContextPtr context) override;

	void MessageRecived(IOFrame::Context::IOContextPtr context, IOFrame::Buffer::IOBufferPtr buffer) override;

	void ErrorCatch(IOFrame::Context::IOContextPtr context, const ErrorCode& error_code) override;

private:
	IOFrame::IOThreadPoolPtr threadPool_;

	IOFrame::Connector::IOConnectorPtr connector_;
};
