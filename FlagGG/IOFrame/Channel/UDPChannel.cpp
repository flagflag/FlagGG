#include "IOFrame/Channel/UDPChannel.h"
#include "IOFrame/Context/UDPContext.h"

#include <SLikeNet/PacketPriority.h>
#include <SLikeNet/MessageIdentifiers.h>

namespace FlagGG
{
	namespace IOFrame
	{
		namespace Channel
		{
			UDPChannel::UDPChannel(SLNet::RakPeerInterface* rakPeer) :
				rakPeer_(rakPeer),
				addrID_(nullptr)
			{}

			UDPChannel::~UDPChannel()
			{
				DestroyGUID();
			}

			bool UDPChannel::Write(Buffer::IOBufferPtr buffer)
			{
				if (!rakPeer_)
					return false;

				AsyncFrame::RecursiveLocker locker(mutex_);

				if (addrID_)
				{
					// 如果没连接，发送数据毫无意义
					if (rakPeer_->GetConnectionState(*addrID_) != SLNet::IS_CONNECTED)
						return false;
				
					UInt32 bufferSize = buffer->GetSize();
					buffer_.Resize(bufferSize + 1);
					buffer->ReadStream(&buffer_[1], bufferSize);
					buffer_[0] = ID_USER_PACKET_ENUM;
					return rakPeer_->Send(buffer_.CString(), buffer_.Length(), HIGH_PRIORITY, RELIABLE_ORDERED, 0, *addrID_, false) != 0;
				}

				return false;
			}

			bool UDPChannel::Flush()
			{
				return true;
			}

			bool UDPChannel::Connect(const char* ip, UInt16 port)
			{
				if (!rakPeer_)
					return false;

				AsyncFrame::RecursiveLocker locker(mutex_);

				if (!rakPeer_->IsActive())
				{
					SLNet::SocketDescriptor socketDescriptor;
					rakPeer_->Startup(1, &socketDescriptor, 1);
				}
				
				// 第一次连接的时候id还没创建
				if (addrID_)
				{
					auto connState = rakPeer_->GetConnectionState(*addrID_);
					// 正在连接或者已经连接
					if (connState == SLNet::IS_CONNECTING || connState == SLNet::IS_CONNECTED)
						return false;
				}

				rakPeer_->Connect(ip, port, 0, 0);

				return true;
			}

			void UDPChannel::Close()
			{
				if (!rakPeer_)
					return;

				AsyncFrame::RecursiveLocker locker(mutex_);

				if (addrID_)
				{
					auto connState = rakPeer_->GetConnectionState(*addrID_);
					if (connState == SLNet::IS_PENDING ||		// Connect() was called, but the process hasn't started yet
						connState == SLNet::IS_CONNECTING ||	// Processing the connection attempt
						connState == SLNet::IS_CONNECTED)		// Is connected and able to communicate
					{
						rakPeer_->CloseConnection(*addrID_, true);
					}
				}
			}

			void UDPChannel::Shutdown()
			{
				if (!rakPeer_)
					return;

				AsyncFrame::RecursiveLocker locker(mutex_);

				rakPeer_->Shutdown(0);
			}

			bool UDPChannel::IsConnected()
			{
				if (!rakPeer_)
					return false;

				AsyncFrame::RecursiveLocker locker(mutex_);

				if (addrID_)
					return rakPeer_->GetConnectionState(*addrID_) == SLNet::IS_CONNECTED;

				return false;
			}

			bool UDPChannel::IsClosed()
			{
				if (!rakPeer_)
					return true;

				AsyncFrame::RecursiveLocker locker(mutex_);

				if (addrID_)
				{
					auto connState = rakPeer_->GetConnectionState(*addrID_);
					return connState == SLNet::IS_DISCONNECTED || connState == SLNet::IS_NOT_CONNECTED;
				}

				return true;
			}

			void UDPChannel::CreateGUID(SLNet::Packet* packet)
			{
				AsyncFrame::RecursiveLocker locker(mutex_);

				if (addrID_)
					delete addrID_;

				addrID_ = new SLNet::AddressOrGUID(packet);
			}

			void UDPChannel::DestroyGUID()
			{
				AsyncFrame::RecursiveLocker locker(mutex_);

				if (addrID_)
				{
					delete addrID_;
					addrID_ = nullptr;
				}
			}

			SLNet::AddressOrGUID* UDPChannel::GetGUID()
			{
				// 这里不加锁，是因为调用线程一定和Create和Destory调用线程一致，此接口并没有暴露给user
				return addrID_;
			}

			void UDPChannel::OnRegisterd(Handler::EventHandlerPtr handler)
			{
				handler_ = handler ? handler : Handler::EventHandlerPtr(new Handler::NullEventHandler);

				Context::UDPContextPtr context(new Context::UDPContext(IOChannelPtr(this)));
				handler_->ChannelRegisterd(context);
			}

			void UDPChannel::OnOpend()
			{
				Context::UDPContextPtr context(new Context::UDPContext(IOChannelPtr(this)));
				handler_->ChannelOpend(context);
			}

			void UDPChannel::OnClosed()
			{
				Context::UDPContextPtr context(new Context::UDPContext(IOChannelPtr(this)));
				handler_->ChannelClosed(context);
			}
		}
	}
}
