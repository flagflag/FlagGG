#include "IOFrame/IOFrame.h"
#include "IOFrame/Buffer/NetBuffer.h"
#include "IOFrame/Acceptor/TCPAcceptor.h"
#include "IOFrame/Connector/TCPConnector.h"
#include "IOFrame/Channel/TCPChannel.h"
#include "IOFrame/Context/TCPContext.h"
#include "IOFrame/ThreadPool/NetThreadPool.h"
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
				return IOThreadPoolPtr(new NetThreadPool(thread_count));
			}

			Buffer::IOBufferPtr CreateBuffer()
			{
				return Buffer::IOBufferPtr(new Buffer::NetBuffer());
			}

			Acceptor::IOAcceptorPtr CreateAcceptor(Handler::EventHandler* handler, size_t thread_count)
			{
				return Acceptor::IOAcceptorPtr(new Acceptor::TCPAcceptor(Handler::EventHandlerPtr(handler), thread_count));
			}

			Connector::IOConnectorPtr CreateConnector(Handler::EventHandler* handler, IOThreadPoolPtr& thread_pool)
			{
				return Connector::IOConnectorPtr(new Connector::TCPConnector(Handler::EventHandlerPtr(handler), thread_pool));
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