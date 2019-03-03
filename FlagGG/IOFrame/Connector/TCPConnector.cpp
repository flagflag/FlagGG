#include "TCPConnector.h"
#include "IOFrame/ThreadPool/NetThreadPool.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Connector
		{
			TCPConnector::TCPConnector(Handler::EventHandlerPtr handler, IOFrame::IOThreadPoolPtr& thread_pool)
				: channel_(new Channel::TCPChannel(Container::DynamicCast<IOFrame::NetThreadPool>(thread_pool)->getService()))
				, handler_(handler)
			{ 
				channel_->OnRegisterd(handler);
			}

			bool TCPConnector::Connect(const char* ip, uint16_t port)
			{
				return channel_->Connect(ip, port);
			}

			bool TCPConnector::Write(Buffer::IOBufferPtr buffer)
			{
				return channel_->Write(buffer);
			}
		}
	}
}