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

				bool connect(const char* ip, uint16_t port) override;

				bool write(Buffer::IOBufferPtr buffer) override;

			private:
				IOFrame::Channel::TCPChannelPtr		m_channel;

				Handler::EventHandlerPtr			m_handler;
			};
			
			typedef std::shared_ptr < TCPConnector > TCPConnectorPtr;
		}
	}
}

#endif