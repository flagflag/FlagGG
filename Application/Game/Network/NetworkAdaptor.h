#pragma once

#include "EventDefine/NetworkEvent.h"

#include <IOFrame/IOFrame.h>
#include <Container/Str.h>
#include <Container/List.h>
#include <Core/EventDefine.h>
#include <Core/Context.h>

using namespace FlagGG;
using namespace FlagGG::Container;
using namespace FlagGG::Core;

class Network : public IOFrame::Handler::EventHandler
{
public:
	Network(Context* context_, NetworkType type);

	~Network() override;

	void Connect(const String& ip, UInt16 port);

	void Disconnect();

	void Send(const void* data, UInt32 dataSize);

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
