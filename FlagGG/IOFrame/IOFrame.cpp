#include "IOFrame/IOFrame.h"
#include "IOFrame/Buffer/Buffer.h"
#include "IOFrame/Acceptor/TCPAcceptor.h"
#include "IOFrame/Connector/TCPConnector.h"
#include "IOFrame/Channel/TCPChannel.h"
#include "IOFrame/Context/TCPContext.h"
#include "IOFrame/ThreadPool/NetThreadPool.h"

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

			Acceptor::IOAcceptorPtr CreateAcceptor(Handler::EventHandlerPtr handler, size_t thread_count)
			{
				return Acceptor::IOAcceptorPtr(new Acceptor::TCPAcceptor(handler, thread_count));
			}

			Connector::IOConnectorPtr CreateConnector(Handler::EventHandlerPtr handler, IOFrame::IOThreadPoolPtr& thread_pool)
			{
				return Connector::IOConnectorPtr(new Connector::TCPConnector(handler, thread_pool));
			}
		}

		namespace UDP
		{
			IOThreadPoolPtr CreateThreadPool(size_t thread_count)
			{
				return nullptr;
			}

			Buffer::IOBufferPtr CreateBuffer()
			{
				return nullptr;
			}

			Acceptor::IOAcceptorPtr CreateAcceptor(Handler::EventHandlerPtr handler, size_t thread_count)
			{
				return nullptr;
			}

			Connector::IOConnectorPtr CreateConnector(Handler::EventHandlerPtr handler, IOFrame::IOThreadPool& thread_pool)
			{
				return nullptr;
			}
		}
	}
}