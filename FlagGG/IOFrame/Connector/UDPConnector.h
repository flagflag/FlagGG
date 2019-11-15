#pragma once

#include "IOConnector.h"
#include "IOFrame/Channel/UDPChannel.h"
#include "IOFrame/Handler/EventHandler.h"
#include "IOFrame/Buffer/IOBuffer.h"
#include "IOFrame/ThreadPool/IOThreadPool.h"

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Connector
		{
			class UDPConnector : public IOConnector
			{
			public:
				UDPConnector(Handler::EventHandlerPtr handler, IOFrame::IOThreadPoolPtr& thread_pool);

				bool Connect(const char* ip, uint16_t port) override;

				bool Disconnect() override;

				bool Write(Buffer::IOBufferPtr buffer) override;

			protected:
				void PacketReciver(SLNet::Packet* packet);

			private:
				Channel::UDPChannelPtr		channel_;

				Handler::EventHandlerPtr	handler_;
			};

			typedef Container::SharedPtr < UDPConnector > UDPConnectorPtr;
		}
	}
}
