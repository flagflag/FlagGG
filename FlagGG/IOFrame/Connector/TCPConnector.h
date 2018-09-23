#ifndef __TCP_CONNECTOR__
#define __TCP_CONNECTOR__

#include "IOConnector.h"
#include "IOFrame\Channel\TCPChannel.h"
#include "IOFrame\ThreadPool\NetThreadPool.h"
#include "IOFrame\Handler\EventHandler.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Connector
		{
			class TCPConnector : public IOConnector
			{
			public:
				TCPConnector(Handler::EventHandlerPtr handler, IOFrame::NetThreadPool& thread_pool);

				virtual ~TCPConnector();

				virtual bool connect(const char* ip, uint16_t port) override;

				virtual bool write(Buffer::IOBufferPtr buffer) override;

			private:
				IOFrame::Channel::TCPChannelPtr		m_channel;

				Handler::EventHandlerPtr			m_handler;
			};
			
			typedef std::shared_ptr < TCPConnector > TCPConnectorPtr;
		}
	}
}

#endif