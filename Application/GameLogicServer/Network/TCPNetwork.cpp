#include "TCPNetwork.h"

TCPNetwork::TCPNetwork(Context* context) :
	context_(context)
{}

void TCPNetwork::ChannelRegisterd(FlagGG::IOFrame::Context::IOContextPtr context)
{

}

void TCPNetwork::ChannelOpend(FlagGG::IOFrame::Context::IOContextPtr context)
{

}

void TCPNetwork::ChannelClosed(FlagGG::IOFrame::Context::IOContextPtr context)
{

}

void TCPNetwork::MessageRecived(FlagGG::IOFrame::Context::IOContextPtr context, FlagGG::IOFrame::Buffer::IOBufferPtr buffer)
{

}

void TCPNetwork::ErrorCatch(FlagGG::IOFrame::Context::IOContextPtr context, const FlagGG::ErrorCode& error_code)
{

}
