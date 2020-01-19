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

namespace FlagGG
{
	namespace IOFrame
	{
		namespace TCP
		{
			IOThreadPoolPtr CreateThreadPool(size_t thread_count)
			{
#ifndef FLAGGG_NO_BOOST
				return IOThreadPoolPtr(new NetThreadPool(thread_count));
#else
				return IOThreadPoolPtr(new UDPThreadPool());
#endif
			}

			Buffer::IOBufferPtr CreateBuffer()
			{
				return Buffer::IOBufferPtr(new Buffer::NetBuffer());
			}

			Acceptor::IOAcceptorPtr CreateAcceptor(Handler::EventHandler* handler, size_t thread_count)
			{
#ifndef FLAGGG_NO_BOOST
				return Acceptor::IOAcceptorPtr(new Acceptor::TCPAcceptor(Handler::EventHandlerPtr(handler), thread_count));
#else
				return Acceptor::IOAcceptorPtr(new Acceptor::UDPAcceptor(Handler::EventHandlerPtr(handler)));
#endif
			}

			Connector::IOConnectorPtr CreateConnector(Handler::EventHandler* handler, IOThreadPoolPtr& thread_pool)
			{
#ifndef FLAGGG_NO_BOOST
				return Connector::IOConnectorPtr(new Connector::TCPConnector(Handler::EventHandlerPtr(handler), thread_pool));
#else
				return Connector::IOConnectorPtr(new Connector::UDPConnector(Handler::EventHandlerPtr(handler), thread_pool));
#endif
			}
		}

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

			Connector::IOConnectorPtr CreateConnector(Handler::EventHandler* handler, IOThreadPoolPtr& thread_pool)
			{
				return Connector::IOConnectorPtr(new Connector::UDPConnector(Handler::EventHandlerPtr(handler), thread_pool));
			}
		}
	}
}