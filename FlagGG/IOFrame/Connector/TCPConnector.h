﻿#ifndef FLAGGG_NO_BOOST
#ifndef __TCP_CONNECTOR__
#define __TCP_CONNECTOR__

#include "IOConnector.h"
#include "IOFrame/Channel/TCPChannel.h"
#include "IOFrame/Handler/EventHandler.h"
#include "IOFrame/Buffer/IOBuffer.h"
#include "IOFrame/ThreadPool/IOThreadPool.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Connector
		{
			class TCPConnector : public IOConnector
			{
			public:
				TCPConnector(Handler::EventHandlerPtr handler, IOFrame::IOThreadPoolPtr& thread_pool);

				~TCPConnector() override = default;

				bool Connect(const char* ip, UInt16 port) override;

				bool Disconnect() override;

				bool Write(Buffer::IOBufferPtr buffer) override;

			private:
				IOFrame::Channel::TCPChannelPtr		channel_;

				Handler::EventHandlerPtr			handler_;
			};
			
			typedef Container::SharedPtr < TCPConnector > TCPConnectorPtr;
		}
	}
}

#endif
#endif
