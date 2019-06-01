#include "Network/NetworkAdaptor.h"

Network::Network() :
	threadPool_(IOFrame::TCP::CreateThreadPool(1)),
	connector_(IOFrame::TCP::CreateConnector(this, threadPool_))
{

}

Network::~Network()
{
	threadPool_->Stop();
	threadPool_->WaitForStop();

	threadPool_.Reset();
	connector_.Reset();
}

void Network::Connect(const String& ip, uint16_t port)
{
	connector_->Connect(ip.CString(), port);
}

void Network::Disconnect()
{
	
}

void Network::SendMessage(const void* data, uint32_t dataSize)
{
	IOFrame::Buffer::IOBufferPtr buffer = IOFrame::TCP::CreateBuffer();
	buffer->WriteStream(data, dataSize);
	connector_->Write(buffer);
}

void Network::ChannelRegisterd(IOFrame::Context::IOContextPtr context)
{

}

void Network::ChannelOpend(IOFrame::Context::IOContextPtr context)
{

}

void Network::ChannelClosed(IOFrame::Context::IOContextPtr context)
{

}

void Network::MessageRecived(IOFrame::Context::IOContextPtr context, IOFrame::Buffer::IOBufferPtr buffer)
{

}

void Network::ErrorCatch(IOFrame::Context::IOContextPtr context, const ErrorCode& error_code)
{

}
