#include "TCPConnector.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Connector
		{
			TCPConnector::TCPConnector(Handler::EventHandlerPtr handler, IOFrame::NetThreadPool& thread_pool)
				: m_channel(new Channel::TCPChannel(thread_pool.getService()))
				, m_handler(handler)
			{ 
				m_channel->onRegisterd(handler);
			}

			TCPConnector::~TCPConnector()
			{ }

			bool TCPConnector::connect(const char* ip, uint16_t port)
			{
				return m_channel->connect(ip, port);
			}

			bool TCPConnector::write(Buffer::IOBufferPtr buffer)
			{
				return m_channel->write(buffer);
			}
		}
	}
}