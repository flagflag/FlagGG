#include "IOFrame/Connector/UDPConnector.h"
#include "IOFrame/ThreadPool/UDPThreadPool.h"
#include "IOFrame/Context/UDPContext.h"
#include "IOFrame/Buffer/NetBuffer.h"

#include <SLikeNet/MessageIdentifiers.h>

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Connector
		{
			UDPConnector::UDPConnector(Handler::EventHandlerPtr handler, IOFrame::IOThreadPoolPtr& thread_pool) :
				channel_(new Channel::UDPChannel(Container::DynamicCast<IOFrame::UDPThreadPool>(thread_pool)->getService())),
				handler_(handler)
			{
				Container::DynamicCast<IOFrame::UDPThreadPool>(thread_pool)->SetPacketReciver(std::bind(&UDPConnector::PacketReciver, this, std::placeholders::_1));
				channel_->OnRegisterd(handler_);
			}

			bool UDPConnector::Connect(const char* ip, uint16_t port)
			{
				return channel_->Connect(ip, port);
			}

			bool UDPConnector::Disconnect()
			{
				channel_->Close();
				return true;
			}

			bool UDPConnector::Write(Buffer::IOBufferPtr buffer)
			{
				return channel_->Write(buffer);
			}

			void UDPConnector::PacketReciver(SLNet::Packet* packet)
			{
				unsigned packetType = packet->data[0];
				switch (packetType)
				{
				case ID_CONNECTION_REQUEST_ACCEPTED:
					channel_->CreateGUID(packet);
					channel_->OnOpend();
					break;

				case ID_DISCONNECTION_NOTIFICATION:
					channel_->OnClosed();
					break;

				case ID_USER_PACKET_ENUM:
					Context::UDPContextPtr context(new Context::UDPContext(channel_));
					Buffer::NetBufferPtr buffer(new Buffer::NetBuffer());
					buffer->WriteStream(packet->data + 1, packet->length - 1);
					handler_->MessageRecived(context, buffer);
					break;
				}
			}
		}
	}
}
