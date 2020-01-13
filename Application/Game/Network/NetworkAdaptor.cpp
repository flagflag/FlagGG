#include "Network/NetworkAdaptor.h"

Network::Network(Context* context, NetworkType type) :
	context_(context),
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

	context_->RegisterEvent(EVENT_HANDLER(Frame::FRAME_BEGIN, Network::HandleFrameBegin, this));
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

void Network::Connect(const String& ip, uint16_t port)
{
	connector_->Connect(ip.CString(), port);
}

void Network::Disconnect()
{
	connector_->Disconnect();
}

void Network::Send(const void* data, uint32_t dataSize)
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
		context_->SendEvent<NetworkEvent::OPEND_HANDLER>(NetworkEvent::OPEND, type_, context);
	});
}

void Network::ChannelClosed(IOFrame::Context::IOContextPtr context)
{
	mainThreadFunc_.Push([=]()
	{
		context_->SendEvent<NetworkEvent::CLOSED_HANDLER>(NetworkEvent::CLOSED, type_, context);
	});
}

void Network::MessageRecived(IOFrame::Context::IOContextPtr context, IOFrame::Buffer::IOBufferPtr buffer)
{
	mainThreadFunc_.Push([=]()
	{
		context_->SendEvent<NetworkEvent::MESSAGE_RECIVED_HANDLER>(NetworkEvent::MESSAGE_RECIVED, type_, context, buffer);
	});
}

void Network::ErrorCatch(IOFrame::Context::IOContextPtr context, const ErrorCode& errorCode)
{
	int value = errorCode.Value();
	auto msg = errorCode.Message();
	mainThreadFunc_.Push([=]()
	{
		context_->SendEvent<NetworkEvent::CATCH_ERROR_HANDLER>(NetworkEvent::CATCH_ERROR, type_, context, value, msg);
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
