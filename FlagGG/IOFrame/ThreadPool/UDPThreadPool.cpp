#include "IOFrame/ThreadPool/UDPThreadPool.h"
#include "Utility/SystemHelper.h"

namespace FlagGG { namespace IOFrame {

UDPThreadPool::UDPThreadPool() :
	rakPeer_(SLNet::RakPeerInterface::GetInstance()),
	running_(false)
{}

UDPThreadPool::~UDPThreadPool()
{
	SLNet::RakPeerInterface::DestroyInstance(rakPeer_);
}

void UDPThreadPool::Start()
{
	if (running_)
		return;

	running_ = true;
	thread_ = new UniqueThread(std::bind(&UDPThreadPool::NetThread, this));
}

void UDPThreadPool::Stop()
{
	running_ = false;
}

void UDPThreadPool::WaitForStop()
{
	thread_->WaitForStop();
}

SLNet::RakPeerInterface* UDPThreadPool::getService()
{
	return rakPeer_;
}

void UDPThreadPool::SetPacketReciver(std::function<void(SLNet::Packet* packet)> func)
{
	reciver_ = func;
}

void UDPThreadPool::NetThread()
{	
	while (running_)
	{
		if (rakPeer_->IsActive())
		{
			for (SLNet::Packet* packet = rakPeer_->Receive(); packet; rakPeer_->DeallocatePacket(packet), packet = rakPeer_->Receive())
			{
				// 内部保证，一定在rakPeer->Startup之前设置reciver，所以不存在线程安全的问题
				if (reciver_)
					reciver_(packet);
			}
		}

		// 这里Sleep的时间有待斟酌
		Sleep(16);
	}
}

}}
