#pragma once

#include "IOFrame/Channel/IOChannel.h"
#include "IOFrame/Handler/IOHandler.h"
#include "Container/Str.h"
#include "AsyncFrame/Locker.h"

#include <SLikeNet/RakPeerInterface.h>
#include <SLikeNet/RakNetTypes.h>

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Acceptor
		{
			class UDPAcceptor;
		};

		namespace Connector
		{
			class UDPConnector;
		}

		namespace Channel
		{
			class UDPChannel : public IOChannel, public Handler::IOHandler
			{
			public:
				UDPChannel(SLNet::RakPeerInterface* rakPeer);

				~UDPChannel() override;

				//IOChannel interface:

				bool Write(Buffer::IOBufferPtr buffer) override;

				bool Flush() override;

				bool Connect(const char* ip, uint16_t port) override;

				void Close() override;

				void Shutdown() override;

				bool IsConnected() override;

				bool IsClosed() override;

				friend class Acceptor::UDPAcceptor;
				friend class Connector::UDPConnector;

			protected:
				void CreateGUID(SLNet::Packet* packet);

				void DestroyGUID();

				SLNet::AddressOrGUID* GetGUID();

				//IOHandler interface:

				void OnRegisterd(Handler::EventHandlerPtr handler) override;

				void OnOpend() override;

				void OnClosed() override;

			private:
				SLNet::RakPeerInterface*		rakPeer_;

				SLNet::AddressOrGUID*			addrID_;

				Container::String				buffer_;

				std::recursive_mutex			mutex_; // 对addrID进行保护，保证user线程访问线程安全

				Handler::EventHandlerPtr		handler_;
			};

			typedef Container::SharedPtr<UDPChannel> UDPChannelPtr;
		}
	}
}
