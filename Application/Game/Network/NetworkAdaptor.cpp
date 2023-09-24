#include "Network/NetworkAdaptor.h"

#include <Core/EventManager.h>

Network::Network(NetworkType type) :
	type_(type)
{
	switch (type)
	{
	case NETWORK_TYPE_TCP:
		threadPool_ = IOFrame::TCP::CreateThreadPool(1);
		threadPool_->Start();
		connector_ = IOFrame::TCP::CreateConnector(this, threadPool_);
		break;

	case NETWORK_TYPE_UDP:
		threadPool_ = IOFrame::UDP::CreateThreadPool();
		threadPool_->Start();
		connector_ = IOFrame::UDP::CreateConnector(this, threadPool_);
		break;

	case NETWORK_TYPE_WEB:

		break;
	}

	GetSubsystem<EventManager>()->RegisterEvent(EVENT_HANDLER(Frame::BEGIN_FRAME, Network::HandleFrameBegin, this));
}

Network::~Network()
{
	if (threadPool_)
	{
		threadPool_->Stop();
		threadPool_->WaitForStop();
	}

	threadPool_.Reset();
	connector_.Reset();
}

void Network::Connect(const String& ip, UInt16 port)
{
	connector_->Connect(ip.CString(), port);
}

void Network::Disconnect()
{
	connector_->Disconnect();
}

void Network::Send(const void* data, UInt32 dataSize)
{
	IOFrame::Buffer::IOBufferPtr buffer = IOFrame::TCP::CreateBuffer();
	buffer->WriteStream(data, dataSize);
	connector_->Write(buffer);
}

bool Network::IsActive() const
{
	return true;
}

void Network::ChannelRegisterd(IOFrame::Context::IOContextPtr context)
{

}

void Network::ChannelOpend(IOFrame::Context::IOContextPtr context)
{
	mainThreadFunc_.Push([=]()
	{
		GetSubsystem<EventManager>()->SendEvent<NetworkEvent::OPEND_HANDLER>(type_, context);
	});
}

void Network::ChannelClosed(IOFrame::Context::IOContextPtr context)
{
	mainThreadFunc_.Push([=]()
	{
		GetSubsystem<EventManager>()->SendEvent<NetworkEvent::CLOSED_HANDLER>(type_, context);
	});
}

void Network::MessageRecived(IOFrame::Context::IOContextPtr context, IOFrame::Buffer::IOBufferPtr buffer)
{
	mainThreadFunc_.Push([=]()
	{
		GetSubsystem<EventManager>()->SendEvent<NetworkEvent::MESSAGE_RECIVED_HANDLER>(type_, context, buffer);
	});
}

void Network::ErrorCatch(IOFrame::Context::IOContextPtr context, const ErrorCode& errorCode)
{
	int value = errorCode.Value();
	auto msg = errorCode.Message();
	mainThreadFunc_.Push([=]()
	{
		GetSubsystem<EventManager>()->SendEvent<NetworkEvent::CATCH_ERROR_HANDLER>(type_, context, value, msg);
	});
}

void Network::HandleFrameBegin(float timeStep)
{
	for (auto it = mainThreadFunc_.Begin(); it != mainThreadFunc_.End();)
	{
		(*it)();
		it = mainThreadFunc_.Erase(it);
	}
}
