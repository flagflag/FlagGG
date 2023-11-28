#include "IOFrame/IOFrame.h"
#include "IOFrame/Buffer/NetBuffer.h"
#ifndef FLAGGG_NO_BOOST
#include "IOFrame/Acceptor/TCPAcceptor.h"
#include "IOFrame/Connector/TCPConnector.h"
#include "IOFrame/Channel/TCPChannel.h"
#include "IOFrame/Context/TCPContext.h"
#include "IOFrame/ThreadPool/NetThreadPool.h"
#endif
#include "IOFrame/Acceptor/UDPAcceptor.h"
#include "IOFrame/Connector/UDPConnector.h"
#include "IOFrame/Channel/UDPChannel.h"
#include "IOFrame/Context/UDPContext.h"
#include "IOFrame/ThreadPool/UDPThreadPool.h"

namespace FlagGG { namespace IOFrame {

namespace TCP
{

IOThreadPoolPtr CreateThreadPool(USize threadCount)
{
#ifndef FLAGGG_NO_BOOST
	return IOThreadPoolPtr(new NetThreadPool(threadCount));
#else
	return IOThreadPoolPtr(new UDPThreadPool());
#endif
}

Buffer::IOBufferPtr CreateBuffer()
{
	return Buffer::IOBufferPtr(new Buffer::NetBuffer());
}

Acceptor::IOAcceptorPtr CreateAcceptor(Handler::EventHandler* handler, USize threadCount)
{
#ifndef FLAGGG_NO_BOOST
	return Acceptor::IOAcceptorPtr(new Acceptor::TCPAcceptor(Handler::EventHandlerPtr(handler), threadCount));
#else
	return Acceptor::IOAcceptorPtr(new Acceptor::UDPAcceptor(Handler::EventHandlerPtr(handler)));
#endif
}

Connector::IOConnectorPtr CreateConnector(Handler::EventHandler* handler, IOThreadPoolPtr& threadPool)
{
#ifndef FLAGGG_NO_BOOST
	return Connector::IOConnectorPtr(new Connector::TCPConnector(Handler::EventHandlerPtr(handler), threadPool));
#else
	return Connector::IOConnectorPtr(new Connector::UDPConnector(Handler::EventHandlerPtr(handler), threadPool));
#endif
}

} // namespace TCP

namespace UDP
{

IOThreadPoolPtr CreateThreadPool()
{
	return IOThreadPoolPtr(new UDPThreadPool());
}

Buffer::IOBufferPtr CreateBuffer()
{
	return Buffer::IOBufferPtr(new Buffer::NetBuffer());
}

Acceptor::IOAcceptorPtr CreateAcceptor(Handler::EventHandler* handler)
{
	return Acceptor::IOAcceptorPtr(new Acceptor::UDPAcceptor(Handler::EventHandlerPtr(handler)));
}

Connector::IOConnectorPtr CreateConnector(Handler::EventHandler* handler, IOThreadPoolPtr& threadPool)
{
	return Connector::IOConnectorPtr(new Connector::UDPConnector(Handler::EventHandlerPtr(handler), threadPool));
}

} // namespace UDP

}}
