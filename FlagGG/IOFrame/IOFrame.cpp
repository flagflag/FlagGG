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
			IOThreadPoolPtr createThreadPool(size_t thread_count)
			{
				return IOThreadPoolPtr(new NetThreadPool(thread_count));
			}

			Buffer::IOBufferPtr createBuffer()
			{
				return Buffer::IOBufferPtr(new Buffer::NetBuffer());
			}

			Acceptor::IOAcceptorPtr createAcceptor(Handler::EventHandlerPtr handler, size_t thread_count)
			{
				return Acceptor::IOAcceptorPtr(new Acceptor::TCPAcceptor(handler, thread_count));
			}

			Connector::IOConnectorPtr createConnector(Handler::EventHandlerPtr handler, IOFrame::IOThreadPoolPtr& thread_pool)
			{
				return Connector::IOConnectorPtr(new Connector::TCPConnector(handler, thread_pool));
			}
		}

		namespace UDP
		{
			IOThreadPoolPtr createThreadPool(size_t thread_count)
			{
				return nullptr;
			}

			Buffer::IOBufferPtr createBuffer()
			{
				return nullptr;
			}

			Acceptor::IOAcceptorPtr createAcceptor(Handler::EventHandlerPtr handler, size_t thread_count)
			{
				return nullptr;
			}

			Connector::IOConnectorPtr createConnector(Handler::EventHandlerPtr handler, IOFrame::IOThreadPool& thread_pool)
			{
				return nullptr;
			}
		}
	}
}