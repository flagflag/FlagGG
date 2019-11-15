#pragma once

#include <IOFrame/IOFrame.h>
#include <Container/Str.h>
#include <Container/List.h>
#include <Core/EventDefine.h>
#include <Core/Context.h>

using namespace FlagGG;
using namespace FlagGG::Container;
using namespace FlagGG::Core;

enum NetworkType
{
	NETWORK_TYPE_TCP = 0,
	NETWORK_TYPE_UDP,
	NETWORK_TYPE_WEB,
	NETWORK_TYPE_MAX,
};

static const char* NETWORK_TYPE_NAME[] = {
	"TCPNetwork",
	"UDPNetwork",
	"WebNetwork",
};

namespace NetworkEvent
{
	DEFINE_EVENT(OPEND, void(NetworkType, IOFrame::Context::IOContextPtr));
	DEFINE_EVENT(CLOSED, void(NetworkType, IOFrame::Context::IOContextPtr));
	DEFINE_EVENT(CATCH_ERROR, void(NetworkType, IOFrame::Context::IOContextPtr, int, Container::String));
	DEFINE_EVENT(MESSAGE_RECIVED, void(NetworkType, IOFrame::Context::IOContextPtr, IOFrame::Buffer::IOBufferPtr));
}

class Network : public IOFrame::Handler::EventHandler
{
public:
	Network(Context* context_, NetworkType type);

	~Network() override;

	void Connect(const String& ip, uint16_t port);

	void Disconnect();

	void SendMessage(const void* data, uint32_t dataSize);

	bool IsActive() const;

protected:
	void ChannelRegisterd(IOFrame::Context::IOContextPtr context) override;

	void ChannelOpend(IOFrame::Context::IOContextPtr context) override;

	void ChannelClosed(IOFrame::Context::IOContextPtr context) override;

	void MessageRecived(IOFrame::Context::IOContextPtr context, IOFrame::Buffer::IOBufferPtr buffer) override;

	void ErrorCatch(IOFrame::Context::IOContextPtr context, const ErrorCode& errorCode) override;

	void HandleFrameBegin(float timeStep);

private:
	Context* context_;

	IOFrame::IOThreadPoolPtr threadPool_;

	IOFrame::Connector::IOConnectorPtr connector_;

	NetworkType type_;

	List<std::function<void()>> mainThreadFunc_;
};
