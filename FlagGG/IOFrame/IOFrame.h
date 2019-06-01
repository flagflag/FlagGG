#ifndef __IOFRAME__
#define __IOFRAME__

#include "IOFrame/Buffer/IOBuffer.h"
#include "IOFrame/Acceptor/IOAcceptor.h"
#include "IOFrame/Connector/IOConnector.h"
#include "IOFrame/Channel/IOChannel.h"
#include "IOFrame/Context/IOContext.h"
#include "IOFrame/Handler/IOHandler.h"
#include "IOFrame/Handler/EventHandler.h"
#include "IOFrame/ThreadPool/IOThreadPool.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace TCP
		{
			IOThreadPoolPtr FlagGG_API CreateThreadPool(size_t thread_count);

			Buffer::IOBufferPtr FlagGG_API CreateBuffer();

			Acceptor::IOAcceptorPtr FlagGG_API CreateAcceptor(Handler::EventHandler* handler, size_t thread_count);

			Connector::IOConnectorPtr FlagGG_API CreateConnector(Handler::EventHandler* handler, IOFrame::IOThreadPoolPtr& thread_pool);
		}

		namespace UDP
		{
			IOThreadPoolPtr FlagGG_API CreateThreadPool(size_t thread_count);

			Buffer::IOBufferPtr FlagGG_API CreateBuffer();

			Acceptor::IOAcceptorPtr FlagGG_API CreateAcceptor(Handler::EventHandler* handler, size_t thread_count);

			Connector::IOConnectorPtr FlagGG_API CreateConnector(Handler::EventHandler* handler, IOFrame::IOThreadPoolPtr& thread_pool);
		}
	}
}

#endif