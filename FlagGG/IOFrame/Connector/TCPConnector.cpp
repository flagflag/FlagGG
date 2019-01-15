#include "TCPConnector.h"
#include "IOFrame\ThreadPool\NetThreadPool.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Connector
		{
			TCPConnector::TCPConnector(Handler::EventHandlerPtr handler, IOFrame::IOThreadPoolPtr& thread_pool)
				: m_channel(new Channel::TCPChannel(std::dynamic_pointer_cast<IOFrame::NetThreadPool>(thread_pool)->getService()))
				, m_handler(handler)
			{ 
				m_channel->onRegisterd(handler);
			}

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