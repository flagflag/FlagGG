#include "IOFrame/Acceptor/UDPAcceptor.h"
#include "IOFrame/Context/UDPContext.h"
#include "IOFrame/Buffer/NetBuffer.h"
#include "IOFrame/IOError.h"
#include "Log.h"

#include <SLikeNet/MessageIdentifiers.h>

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Acceptor
		{
			static const unsigned MAX_NUM_CONNECIONS = 1000;

			UDPAcceptor::UDPAcceptor(Handler::EventHandlerPtr handler) :
				threadPool_(new UDPThreadPool()),
				handler_(handler)
			{}

			bool UDPAcceptor::Bind(const char* ip, uint16_t port)
			{
				auto* rakPeer = threadPool_->getService();
				if (!rakPeer->IsActive())
				{
					threadPool_->SetPacketReciver(std::bind(&UDPAcceptor::PacketReciver, this, std::placeholders::_1));

					SLNet::SocketDescriptor socketDescriptor(port, ip);
					auto result = rakPeer->Startup(MAX_NUM_CONNECIONS, &socketDescriptor, 1);
					if (result != SLNet::RAKNET_STARTED && result != SLNet::RAKNET_ALREADY_STARTED)
						return false;
					rakPeer->SetMaximumIncomingConnections(MAX_NUM_CONNECIONS);
					rakPeer->SetUnreliableTimeout(1000);
				}

				return true;
			}

			void UDPAcceptor::Start()
			{
				threadPool_->Start();
			}

			void UDPAcceptor::Stop()
			{
				threadPool_->Stop();
			}

			void UDPAcceptor::WaitForStop()
			{
				threadPool_->WaitForStop();
			}

			void UDPAcceptor::PacketReciver(SLNet::Packet* packet)
			{
				unsigned packetType = packet->data[0];
				switch (packetType)
				{
				case ID_NEW_INCOMING_CONNECTION:
					NewConnection(packet);
					break;

				case ID_DISCONNECTION_NOTIFICATION:
					DeleteConnection(packet);
					break;

				case ID_CONNECTION_LOST:
					DeleteConnection(packet);
					break;

				case ID_USER_PACKET_ENUM:
					ReciveMessage(packet);
					break;
				}
			}

			Channel::UDPChannelPtr UDPAcceptor::FindActiveChannel(SLNet::Packet* packet)
			{
				SLNet::AddressOrGUID guid(packet);
				for (auto it = activeConn_.Begin(); it != activeConn_.End(); ++it)
				{
					auto* GUID = (*it)->GetGUID();
					if (GUID && *GUID == guid)
					{
						return *it;
					}
				}
				return nullptr;
			}

			void UDPAcceptor::NewConnection(SLNet::Packet* packet)
			{
				Channel::UDPChannelPtr channel(new Channel::UDPChannel(threadPool_->getService()));
				channel->OnRegisterd(handler_);

				if (FindActiveChannel(packet))
				{
					FLAGGG_LOG_ERROR("Ready to new connection, but found channel from queue.");
					Context::UDPContextPtr context(new Context::UDPContext(channel));
					Error::CustomIOError error(-1, "");
					handler_->ErrorCatch(context, error);
					return;
				}

				channel->CreateGUID(packet);
				channel->OnOpend();
				activeConn_.Push(channel);
			}

			void UDPAcceptor::DeleteConnection(SLNet::Packet* packet)
			{
				auto channel = FindActiveChannel(packet);
				if (!channel)
				{
					FLAGGG_LOG_ERROR("Ready to delete connection, but can not found channel from queue.");
					return;
				}
				channel->OnClosed();
			}

			void UDPAcceptor::ReciveMessage(SLNet::Packet* packet)
			{
				auto channel = FindActiveChannel(packet);
				if (!channel)
				{
					FLAGGG_LOG_ERROR("Ready to recive message, but can not found channel from queue.");
					return;
				}
				Context::UDPContextPtr context(new Context::UDPContext(channel));
				Buffer::NetBufferPtr buffer(new Buffer::NetBuffer());
				buffer->WriteStream(packet->data + 1, packet->length - 1);
				handler_->MessageRecived(context, buffer);
			}
		}
	}
}
